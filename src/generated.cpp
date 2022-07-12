
// This file is autogenerated, please do not edit manually
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "generated.h"
#include <zephyr/zephyr.h>

int build_feedback_getversion_frame(char* buffer, int *len, struct s_fb_getversion_params* parameters)
{
    int position = 0;

    if ((buffer == NULL) || (len == NULL) || (parameters == NULL))
      return -1;
    
    if (position < *len) buffer[position++] = 0;
    else return -1;
        
    if (position < *len) buffer[position++] = (uint8_t) parameters->major;
    else return -1;
    

    if (position < *len) buffer[position++] = (uint8_t) parameters->minor;
    else return -1;
    

    if (position < *len) buffer[position++] = (uint8_t) parameters->patch;
    else return -1;
    
    *len = position;

    return 0;
}
        
int build_feedback_sleeppin_frame(char* buffer, int *len, struct s_fb_sleeppin_params* parameters)
{
    int position = 0;

    if ((buffer == NULL) || (len == NULL) || (parameters == NULL))
      return -1;
    
    if (position < *len) buffer[position++] = 3;
    else return -1;
        
    if (position < *len) buffer[position++] = (uint8_t) parameters->success;
    else return -1;
    
    *len = position;

    return 0;
}
        
int build_feedback_sleeptime_frame(char* buffer, int *len, struct s_fb_sleeptime_params* parameters)
{
    int position = 0;

    if ((buffer == NULL) || (len == NULL) || (parameters == NULL))
      return -1;
    
    if (position < *len) buffer[position++] = 4;
    else return -1;
        
    if (position < *len) buffer[position++] = (uint8_t) parameters->feedback;
    else return -1;
    
    *len = position;

    return 0;
}
        
int build_instruction_getversion_frame(char* buffer, int *len, struct s_inst_getversion_params* parameters)
{
    int position = 0;

    if ((buffer == NULL) || (len == NULL) || (parameters == NULL))
      return -1;
    
    if (position < *len) buffer[position++] = 0;
    else return -1;
        
    *len = position;

    return 0;
}
        
int build_instruction_sleeppin_frame(char* buffer, int *len, struct s_inst_sleeppin_params* parameters)
{
    int position = 0;

    if ((buffer == NULL) || (len == NULL) || (parameters == NULL))
      return -1;
    
    if (position < *len) buffer[position++] = 3;
    else return -1;
        
    if ((position + 2) < *len) {
        memcpy(&buffer[position], &parameters->pre_sleep_time, 2);
        position += 2;
    }
    else return -1;
    
    *len = position;

    return 0;
}
        
int build_instruction_sleeptime_frame(char* buffer, int *len, struct s_inst_sleeptime_params* parameters)
{
    int position = 0;

    if ((buffer == NULL) || (len == NULL) || (parameters == NULL))
      return -1;
    
    if (position < *len) buffer[position++] = 4;
    else return -1;
        
    if ((position + 4) < *len) {
        memcpy(&buffer[position], &parameters->duration, 4);
        position += 4;
    }
    else return -1;
    
    *len = position;

    return 0;
}
        
int parse_feedback_getversion_frame(char* buffer, int len, struct s_fb_getversion_params* parameters)
{
    int position = 0;
    const size_t p_size = sizeof(struct s_fb_getversion_params);


    if (buffer == NULL)
        return -1;

    if ((p_size > 0) && (parameters == NULL))
        return -1;
        
    // Check the code
    if (buffer[position++] != 0) return -1;
        
    if (position < len) parameters->major = (uint8_t) buffer[position++] ;
    else return -1;
    

    if (position < len) parameters->minor = (uint8_t) buffer[position++] ;
    else return -1;
    

    if (position < len) parameters->patch = (uint8_t) buffer[position++] ;
    else return -1;
    
    return 0;
}
        
int parse_feedback_sleeppin_frame(char* buffer, int len, struct s_fb_sleeppin_params* parameters)
{
    int position = 0;
    const size_t p_size = sizeof(struct s_fb_sleeppin_params);


    if (buffer == NULL)
        return -1;

    if ((p_size > 0) && (parameters == NULL))
        return -1;
        
    // Check the code
    if (buffer[position++] != 3) return -1;
        
    if (position < len) parameters->success = (bool) buffer[position++] ;
    else return -1;
    
    return 0;
}
        
int parse_feedback_sleeptime_frame(char* buffer, int len, struct s_fb_sleeptime_params* parameters)
{
    int position = 0;
    const size_t p_size = sizeof(struct s_fb_sleeptime_params);


    if (buffer == NULL)
        return -1;

    if ((p_size > 0) && (parameters == NULL))
        return -1;
        
    // Check the code
    if (buffer[position++] != 4) return -1;
        
    if (position < len) parameters->feedback = (uint8_t) buffer[position++] ;
    else return -1;
    
    return 0;
}
        
int parse_instruction_getversion_frame(char* buffer, int len, struct s_inst_getversion_params* parameters)
{
    int position = 0;
    const size_t p_size = sizeof(struct s_inst_getversion_params);


    if (buffer == NULL)
        return -1;

    if ((p_size > 0) && (parameters == NULL))
        return -1;

    // Check the code
    if (buffer[position++] != 0) return -1;
        
    return 0;
}
        
int parse_instruction_sleeppin_frame(char* buffer, int len, struct s_inst_sleeppin_params* parameters)
{
    int position = 0;
    const size_t p_size = sizeof(struct s_inst_sleeppin_params);


    if (buffer == NULL)
        return -1;

    if ((p_size > 0) && (parameters == NULL))
        return -1;

    // Check the code
    if (buffer[position++] != 3) return -1;
        
    if ((position + 2) < len) {
        memcpy(&parameters->pre_sleep_time, &buffer[position], 2);
        position += 2;
    }
    else return -1;
    
    return 0;
}
        
int parse_instruction_sleeptime_frame(char* buffer, int len, struct s_inst_sleeptime_params* parameters)
{
    int position = 0;
    const size_t p_size = sizeof(struct s_inst_sleeptime_params);


    if (buffer == NULL)
        return -1;

    if ((p_size > 0) && (parameters == NULL))
        return -1;

    // Check the code
    if (buffer[position++] != 4) return -1;
        
    if ((position + 4) < len) {
        memcpy(&parameters->duration, &buffer[position], 4);
        position += 4;
    }
    else return -1;
    
    return 0;
}
        
int parse_feedback_frame(char* buffer, int len, Feedbacks* code, void **parameters)
{
    if ((buffer == NULL) || (code == NULL) || (parameters == NULL))
        return -1;

    switch (buffer[0])
    {
    
        case 0:
            {
                const size_t psize = sizeof(struct s_fb_getversion_params);
                *parameters = k_malloc(psize);
                memset(*parameters, 0, psize);
                *code = FB_GETVERSION;
                return parse_feedback_getversion_frame(buffer, len, (struct s_fb_getversion_params*)*parameters);
            }
        
        case 3:
            {
                const size_t psize = sizeof(struct s_fb_sleeppin_params);
                *parameters = k_malloc(psize);
                memset(*parameters, 0, psize);
                *code = FB_SLEEPPIN;
                return parse_feedback_sleeppin_frame(buffer, len, (struct s_fb_sleeppin_params*)*parameters);
            }
        
        case 4:
            {
                const size_t psize = sizeof(struct s_fb_sleeptime_params);
                *parameters = k_malloc(psize);
                memset(*parameters, 0, psize);
                *code = FB_SLEEPTIME;
                return parse_feedback_sleeptime_frame(buffer, len, (struct s_fb_sleeptime_params*)*parameters);
            }
        
    default: 
        return -1;
    }
}

    
int parse_instruction_frame(char* buffer, int len, Instructions* code, void **parameters)
{
    if ((buffer == NULL) || (code == NULL) || (parameters == NULL))
        return -1;

    switch (buffer[0])
    {
    
        case 0:
            {
                const size_t psize = sizeof(struct s_inst_getversion_params);
                *parameters = k_malloc(psize);
                memset(*parameters, 0, psize);
                *code = INST_GETVERSION;
                return parse_instruction_getversion_frame(buffer, len, (struct s_inst_getversion_params*)*parameters);
            }
        
        case 3:
            {
                const size_t psize = sizeof(struct s_inst_sleeppin_params);
                *parameters = k_malloc(psize);
                memset(*parameters, 0, psize);
                *code = INST_SLEEPPIN;
                return parse_instruction_sleeppin_frame(buffer, len, (struct s_inst_sleeppin_params*)*parameters);
            }
        
        case 4:
            {
                const size_t psize = sizeof(struct s_inst_sleeptime_params);
                *parameters = k_malloc(psize);
                memset(*parameters, 0, psize);
                *code = INST_SLEEPTIME;
                return parse_instruction_sleeptime_frame(buffer, len, (struct s_inst_sleeptime_params*)*parameters);
            }
        
    default: 
        return -1;
    }
}

    