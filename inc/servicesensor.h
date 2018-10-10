#ifndef __servicesensor_H__
#define __servicesensor_H__

#include <dlog.h>
#include <sensor.h>
#include <stdio.h>
#include <recorder.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "servicesensor"


#endif /* __servicesensor_H__ */
void sendMessage(const char *str);
void sensor_cb(sensor_h sensor, sensor_event_s *event, void *user_data);
void add_listener(int index,sensor_type_e type, sensor_event_cb cb_func, void *data);
char* get_write_filepath(char *filename);
void startListener(void* data);
void write_file(const char* filepath, const char* buf);
FILE *get_file_by_type(sensor_type_e type);
static void _state_changed_cb(recorder_state_e previous, recorder_state_e current, bool by_policy, void *user_data);
void print_error(int error);
void _recording_status_cb(unsigned long long elapsed_time, unsigned long long file_size, void *user_data);
