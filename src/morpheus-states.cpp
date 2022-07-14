
#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/smf.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include "morpheus-states.h"


#define MORPHEUS_USER_NODE DT_PATH(zephyr_user)
static const struct gpio_dt_spec relay = GPIO_DT_SPEC_GET(MORPHEUS_USER_NODE, poweron_gpios); 
static const struct gpio_dt_spec wakepin = GPIO_DT_SPEC_GET(MORPHEUS_USER_NODE, wakepin_gpios);

/// State machine definitions
enum MorpheusSMStates {
    MSMS_POWERON,
    MSMS_PRE_SLEEP_PIN,
    MSMS_SLEEP_PIN,
    MSMS_PRE_SLEEP_TIME,
    MSMS_SLEEP_TIME
};

enum MorpheusCommands {
    MSMC_NONE,
    MSMC_SLEEP_PIN,
    MSMC_SLEEP_TIME
};

/* Forward declaration of state table */
K_MSGQ_DEFINE(morpheus_cmdq, sizeof(MorpheusCommands), 2, 1);

/* Morpheus state object */
struct morpheus_s_object {
        /* This must be first */
        struct smf_ctx ctx;

        /* Other state specific data add here */
        uint16_t pre_delay;
        int64_t delay_start_time;
        bool wait_pin_state;
        uint32_t sleep_time;
} ms_obj;

static void msms_poweron_entry(void* obj);
static void msms_poweron_run(void* obj);
static void msms_poweron_exit(void* obj);
static void msms_pre_sleep_pin_entry(void* obj);
static void msms_pre_sleep_pin_run(void* obj);
static void msms_sleep_pin_entry(void* obj);
static void msms_sleep_pin_run(void* obj);
static void msms_pre_sleep_time_entry(void* obj);
static void msms_pre_sleep_time_run(void* obj);
static void msms_sleep_time_entry(void* obj);
static void msms_sleep_time_run(void* obj);

/** Populate the states table **/
static const struct smf_state morpheus_states[] {
    [MSMS_POWERON] = SMF_CREATE_STATE(msms_poweron_entry, msms_poweron_run, msms_poweron_exit),
    [MSMS_PRE_SLEEP_PIN] = SMF_CREATE_STATE(msms_pre_sleep_pin_entry, msms_pre_sleep_pin_run, NULL),
    [MSMS_SLEEP_PIN] = SMF_CREATE_STATE(msms_sleep_pin_entry, msms_sleep_pin_run, NULL),
    [MSMS_PRE_SLEEP_TIME] = SMF_CREATE_STATE(msms_pre_sleep_time_entry, msms_pre_sleep_time_run, NULL),
    [MSMS_SLEEP_TIME] = SMF_CREATE_STATE(msms_sleep_time_entry, msms_sleep_time_run, NULL),
};
/*
 * Power ON state
 */
static void msms_poweron_entry(void* obj)
{
   gpio_pin_configure_dt(&wakepin, GPIO_INPUT);
   gpio_pin_configure_dt(&relay, GPIO_OUTPUT_ACTIVE);
   gpio_pin_set_dt(&relay, 1);
}

static void msms_poweron_run(void* obj)
{
   MorpheusCommands cmd = MSMC_NONE;

   if (k_msgq_get(&morpheus_cmdq, &cmd, K_MSEC(1)) == 0)
   {
    switch (cmd) 
    {
    case MSMC_NONE:
        break;

    case MSMC_SLEEP_PIN:
        smf_set_state(SMF_CTX(&ms_obj), &morpheus_states[MSMS_PRE_SLEEP_PIN]);
        break;

    case MSMC_SLEEP_TIME:
        smf_set_state(SMF_CTX(&ms_obj), &morpheus_states[MSMS_PRE_SLEEP_TIME]);
        break;
    
    default:
        break;
    }
   }    
}

static void msms_poweron_exit(void* obj)
{

}

/*
 * Pre SleepPin state
 */
static void msms_pre_sleep_pin_entry(void* obj)
{
    ms_obj.delay_start_time = k_uptime_get();
}

static void msms_pre_sleep_pin_run(void* obj)
{
    if ((k_uptime_get() - ms_obj.delay_start_time ) >= (ms_obj.pre_delay * 1000))
    {
        smf_set_state(SMF_CTX(&ms_obj), &morpheus_states[MSMS_SLEEP_PIN]);
    }
}

/*
 * SleepPin state
 */
static void msms_sleep_pin_entry(void* obj)
{
    gpio_pin_set_dt(&relay, 0);
}

static void msms_sleep_pin_run(void* obj)
{
    if (gpio_pin_get_dt(&wakepin) != ms_obj.wait_pin_state)
    {
        smf_set_state(SMF_CTX(&ms_obj), &morpheus_states[MSMS_POWERON]);
    }
}

/*
 * Pre SleepTime state
 */
static void msms_pre_sleep_time_entry(void* obj)
{
    ms_obj.delay_start_time = k_uptime_get();
}

static void msms_pre_sleep_time_run(void* obj)
{
    int64_t current_time = k_uptime_get();
    if ((current_time - ms_obj.delay_start_time ) >= (ms_obj.pre_delay * 1000))
    {
        smf_set_state(SMF_CTX(&ms_obj), &morpheus_states[MSMS_SLEEP_TIME]);
    }
}

/*
 * SleepTime state
 */
static void msms_sleep_time_entry(void* obj)
{
    gpio_pin_set_dt(&relay, 0);
    ms_obj.delay_start_time = k_uptime_get();
}

static void msms_sleep_time_run(void* obj)
{
    int64_t current_time = k_uptime_get();
    if ((current_time - ms_obj.delay_start_time ) >= (ms_obj.sleep_time * 1000))
    {
        smf_set_state(SMF_CTX(&ms_obj), &morpheus_states[MSMS_POWERON]);
    }
}


void morpheus_state_thread_entry(void*, void*, void*) {
    int ret;
     /* Run the state machine */
    while(1) {
            /* State machine terminates if a non-zero value is returned */
            ret = smf_run_state(SMF_CTX(&ms_obj));
            if (ret) {
                    /* handle return code and terminate state machine */
                    break;
            }
            k_msleep(10);
    }
}

K_THREAD_STACK_DEFINE(morpheus_stack_area, 256);
struct k_thread morpheus_thread_data;

void morpheus_state_init()
{
    /* Set initial state */
    smf_set_initial(SMF_CTX(&ms_obj), &morpheus_states[MSMS_POWERON]);
    k_thread_create(&morpheus_thread_data, morpheus_stack_area,
        K_THREAD_STACK_SIZEOF(morpheus_stack_area),
        morpheus_state_thread_entry, NULL, NULL, NULL,
        4, 0, K_NO_WAIT);
}

void morpheus_sleep_pin(uint16_t pre_delay, bool pin_state)
{
    MorpheusCommands cmd = MSMC_SLEEP_PIN;
    ms_obj.wait_pin_state = pin_state;
    ms_obj.pre_delay = pre_delay;
    k_msgq_put(&morpheus_cmdq, &cmd, K_NO_WAIT);
}

void morpheus_sleep_time(uint16_t pre_delay, uint32_t sleep_time)
{
    MorpheusCommands cmd = MSMC_SLEEP_TIME;
    ms_obj.sleep_time = sleep_time;
    ms_obj.pre_delay = pre_delay;
    k_msgq_put(&morpheus_cmdq, &cmd, K_NO_WAIT);
}
