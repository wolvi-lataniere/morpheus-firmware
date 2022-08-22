
#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include "morpheus-states.h"
#include <zephyr/usb/usb_device.h>

#define MORPHEUS_USER_NODE DT_PATH(zephyr_user)

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec relay = GPIO_DT_SPEC_GET(MORPHEUS_USER_NODE, poweron_gpios); 
static const struct gpio_dt_spec wakepin = GPIO_DT_SPEC_GET(MORPHEUS_USER_NODE, wakepin_gpios);

/* Commands definition for the command queue */
enum MorpheusCommands {
    MSMC_NONE,
    MSMC_SLEEP_PIN,
    MSMC_SLEEP_TIME
};

/* Forward declaration of state table */
K_MSGQ_DEFINE(morpheus_cmdq, sizeof(MorpheusCommands), 2, 1);


/* Morpheus state object */
struct morpheus_s_object {
        /* Other state specific data add here */
        uint16_t pre_delay;
        bool wait_pin_state;
        uint32_t sleep_time;
} ms_obj;

/**
 * @brief Handle the SleepPin instruction routine
 * 
 * This function puts executes the SleepPin procedure:
 *   - Wait for PRE_DELAY seconds,
 *   - Cut the Pi power,
 *   - Wait for the GPIO input pin to reach WAIT_PIN_STATE level,
 *   - Wake up the Raspberry Pi,
 * 
 * NOTE: This function is blocking and should be executed in a separate thread.
 * 
 * @param pre_delay Duration (in seconds) to wait before cutting power
 * @param wait_pin_state Wake-up pin active state (true for high)
 */
static void __morpheus_sleep_pin_routine(uint16_t pre_delay, bool wait_pin_state)
{
    // Wait for the pre-sleep delay
    k_sleep(K_SECONDS(pre_delay));
    
    // Cut the power
    gpio_pin_set_dt(&relay, 0);
    gpio_pin_set_dt(&led, 0);

    // Wait for the GPIO to reach desired state
    while (gpio_pin_get_dt(&wakepin) == wait_pin_state)
    {
        k_sleep(K_MSEC(100));
    }

    // Restore the power
    gpio_pin_set_dt(&relay, 1);
    gpio_pin_set_dt(&led, 1);
}

/**
 * @brief Handle the SleepTime instruction routine
 * 
 * This function puts executes the SleepTime procedure:
 *   - Wait for PRE_DELAY seconds,
 *   - Cut the Pi power,
 *   - Wait for SLEEP_TIME seconds,
 *   - Wake up the Raspberry Pi,
 * 
 * NOTE: This function is blocking and should be executed in a separate thread.
 * 
 * @param pre_delay Duration (in seconds) to wait before cutting power
 * @param sleep_time Duration (in seconds) to wait before restoring power
 */
static void __morpheus_sleep_time_routine(uint16_t pre_delay, uint32_t sleep_time)
{
    // Wait for the pre-sleep delay
    k_sleep(K_SECONDS(pre_delay));
    
    // Cut the power
    gpio_pin_set_dt(&relay, 0);
    gpio_pin_set_dt(&led, 0);

    // Wait for the sleep-time delay
    k_sleep(K_SECONDS(sleep_time));

    // Restore the power
    gpio_pin_set_dt(&relay, 1);
    gpio_pin_set_dt(&led, 1);
}

/**
 * @brief Main Morpheus loop
 * 
 * This function fetches instructions from the command queue and starts the relevant routine.
 */
void morpheus_state_thread_entry(void*, void*, void*) {
    MorpheusCommands cmd = MSMC_NONE;

    while(1) {
        // Poll the commands
        if (k_msgq_get(&morpheus_cmdq, &cmd, K_SECONDS(1)) == 0)
        {
            switch (cmd) 
            {
            case MSMC_NONE:
                break;

            case MSMC_SLEEP_PIN:
                __morpheus_sleep_pin_routine(ms_obj.pre_delay, ms_obj.wait_pin_state);
                break;

            case MSMC_SLEEP_TIME:
                __morpheus_sleep_time_routine(ms_obj.pre_delay, ms_obj.sleep_time);
                break;
            
            default:
                break;
            }
        } 
    }
}

K_THREAD_STACK_DEFINE(morpheus_stack_area, 1024);
struct k_thread morpheus_thread_data;

void morpheus_state_init()
{
    /* Configure GPIOs */
   gpio_pin_configure_dt(&wakepin, GPIO_INPUT);
   gpio_pin_configure_dt(&relay, GPIO_OUTPUT_ACTIVE);
   gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
   gpio_pin_set_dt(&relay, 1); /// Relay is active by default
   gpio_pin_set_dt(&led, 1); /// Relay is active by default

    // Set some default values
    ms_obj.pre_delay = 90;
    ms_obj.sleep_time=0;
    ms_obj.wait_pin_state = false;

    k_thread_create(&morpheus_thread_data, morpheus_stack_area,
        K_THREAD_STACK_SIZEOF(morpheus_stack_area),
        morpheus_state_thread_entry, NULL, NULL, NULL,
        4, 0, K_SECONDS(10));
}

void morpheus_sleep_pin(uint16_t pre_delay, bool pin_state)
{
    MorpheusCommands cmd = MSMC_SLEEP_PIN;
    ms_obj.wait_pin_state = pin_state;
    ms_obj.pre_delay = pre_delay;
    k_msgq_put(&morpheus_cmdq, &cmd, K_MSEC(100));
}

void morpheus_sleep_time(uint16_t pre_delay, uint32_t sleep_time)
{
    MorpheusCommands cmd = MSMC_SLEEP_TIME;
    ms_obj.sleep_time = sleep_time;
    ms_obj.pre_delay = pre_delay;
    k_msgq_put(&morpheus_cmdq, &cmd, K_MSEC(100));
}
