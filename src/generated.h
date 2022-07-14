
// This file is autogenerated, please do not edit manually
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// Instructions Codes list
typedef enum __instructions_enum {
    INST_GETVERSION = 0,
	INST_SLEEPPIN = 3,
	INST_SLEEPTIME = 4
} Instructions;


// Feedback codes list
typedef enum __feedbacks_enum {
    FB_GETVERSION = 0,
	FB_SLEEPPIN = 3,
	FB_SLEEPTIME = 4
} Feedbacks;




struct s_inst_getversion_params {
    
};

struct s_inst_sleeppin_params {
    uint16_t pre_sleep_time;
	bool wake_pin_active_state;
};

struct s_inst_sleeptime_params {
    uint16_t pre_sleep_time;
	uint32_t duration;
};

struct s_fb_getversion_params {
    uint8_t major;
	uint8_t minor;
	uint8_t patch;
};
    
struct s_fb_sleeppin_params {
    bool success;
};
    
struct s_fb_sleeptime_params {
    uint8_t feedback;
};
    
int build_feedback_getversion_frame(char* buffer, int *len, struct s_fb_getversion_params* parameters);

int build_instruction_getversion_frame(char* buffer, int *len, struct s_inst_getversion_params* parameters);

int build_feedback_sleeppin_frame(char* buffer, int *len, struct s_fb_sleeppin_params* parameters);

int build_instruction_sleeppin_frame(char* buffer, int *len, struct s_inst_sleeppin_params* parameters);

int build_feedback_sleeptime_frame(char* buffer, int *len, struct s_fb_sleeptime_params* parameters);

int build_instruction_sleeptime_frame(char* buffer, int *len, struct s_inst_sleeptime_params* parameters);


    // External parsing functions
    extern int parse_feedback_frame(char* buffer, int len, Feedbacks* code, void **parameters);
    extern int parse_instruction_frame(char* buffer, int len, Instructions* code, void **parameters);