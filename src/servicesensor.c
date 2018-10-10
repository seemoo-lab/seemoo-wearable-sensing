#include <tizen.h>
#include <service_app.h>
#include <sensor.h>
#include "servicesensor.h"
#include "message_port.h"
#include "stdio.h"
#include "time.h"
#include "glib.h"
#include "stdlib.h"
#include "storage.h"
#include "recorder.h"
#include "streamrecorder.h"


bool running = false;
typedef struct {
	sensor_listener_h listener[50];
	sensor_h sensor[50];
	app_control_h reply;
	app_control_h app_control_g;

} appdata_s;



int listenerSize= 0;

recorder_h recorder;

FILE *fp;
FILE *fpaccel;
FILE *fpgyr;
FILE *fplight;
FILE *fppress;

FILE *get_file_by_type(sensor_type_e type){
	switch(type){
		case SENSOR_ACCELEROMETER : return fpaccel;
		case SENSOR_GRAVITY: 		return fp;
		case SENSOR_LINEAR_ACCELERATION:return fp;
		case SENSOR_MAGNETIC:		return fp;
		case SENSOR_ROTATION_VECTOR:return fp;
		case SENSOR_ORIENTATION:	return fp;
		case SENSOR_GYROSCOPE:		return fpgyr;
		case SENSOR_LIGHT:			return fplight;
		case SENSOR_PROXIMITY:		return fp;
		case SENSOR_PRESSURE:		return fppress;
		case SENSOR_ULTRAVIOLET:	return fp;
		case SENSOR_TEMPERATURE:	return fp;
		case SENSOR_HUMIDITY:       return fp;
		}
	return fp;
}



void getNamebyType(sensor_type_e type, char** name){
	switch(type){
	case SENSOR_ACCELEROMETER : name[0] = "ACCELEROMETER"; break;
	case SENSOR_GRAVITY: 		name[0] = "GRAVITY";break;
	case SENSOR_LINEAR_ACCELERATION:name[0] = "LINEAR_ACCELERATION";break;
	case SENSOR_MAGNETIC:		name[0] = "MAGNETIC";break;
	case SENSOR_ROTATION_VECTOR:name[0] = "ROTATION_VECTOR";break;
	case SENSOR_ORIENTATION:	name[0] = "ORIENTATION";break;
	case SENSOR_GYROSCOPE:		name[0] = "GYROSCOPE";break;
	case SENSOR_LIGHT:			name[0] = "LIGHT";break;
	case SENSOR_PROXIMITY:		name[0] = "PROXIMITY";break;
	case SENSOR_PRESSURE:		name[0] = "PRESSURE";break;
	case SENSOR_ULTRAVIOLET:	name[0] = "ULTRAVIOLET";break;
	case SENSOR_TEMPERATURE:	name[0] = "TEMPERATURE";break;
	case SENSOR_HUMIDITY:       name[0] = "HUMIDITY";break;
	}
}

int i = 0;

void
sensor_cb(sensor_h sensor, sensor_event_s *event, void *user_data)
{
	appdata_s *ad = (appdata_s *) user_data;

	time_t raw_time;
	time(&raw_time);
	struct tm* l_time = localtime(&raw_time);
	char fBuffer[100] = "";
	char timeStamp[100] = "";
	char out [1000] = "";
	sensor_type_e type;
	sensor_get_type(sensor, &type);
	for (int i = 0; i < event->value_count; i++){
		//fArray[i] = &fBuffer[i];
		sprintf(fBuffer, "%f",event->values[i]);
		strcat(out, fBuffer);
		strcat(out, " ");
	}
	sprintf(timeStamp, "%d-%02d-%02dT%02d:%02d:%02d",l_time->tm_year+1900,l_time->tm_mon,l_time->tm_mday,l_time->tm_hour,l_time->tm_min,l_time->tm_sec);
	strcat(out, timeStamp);
	strcat(out, "\r\n");
	fputs(out, get_file_by_type(type));
}


void
message_port_cb(int local_port_id, const char *remote_app_id, const char *remote_port,
                bool trusted_remote_port, bundle *message, void *user_data)
{
	sendMessage("Callback Called");
	char *m;
	bundle_get_str(message, "key", &m);
	appdata_s *ad = (appdata_s *) user_data;
	char out[50]="";
	recorder_state_e state;
		recorder_get_state(recorder, &state);
		if (state == RECORDER_STATE_RECORDING) {
			sendMessage("Recording");
	}
	sprintf(out, "%d", state);
	sendMessage(out);
	int err = recorder_commit(recorder);
	//if(state == RECORDER_DEVICE_STATE_PAUSED){
		//	sendMessage("record Paused");
	//} else {print_error(err);}
	if (err == RECORDER_ERROR_NONE){
		sendMessage("Yay");
	} else {print_error(err);}
	sendMessage("After Paused Called");


/*
	recorder_get_state(recorder, &state); // Invalid Operation

	err = recorder_commit(recorder);
	if (err == RECORDER_ERROR_NONE) {
		sendMessage("comitted");
	} else{print_error(err);}
	recorder_unprepare(recorder);
	recorder_destroy(recorder);
	/*
			stopListener(data);
			app_control_h reply;
			app_control_create(&reply);
			app_control_add_extra_data(reply, APP_CONTROL_DATA_SUBJECT, "finished");
			app_control_reply_to_launch_request(reply, app_control, APP_CONTROL_RESULT_SUCCEEDED);

			service_app_exit();*/


	//sendMessage(m);

}


bool is_supported(sensor_type_e type){
	bool b;
	sensor_is_supported(type, &b);
	return b;
}


void add_listener(int index, sensor_type_e type, sensor_event_cb cb_func, void *data){
	sendMessage("in Register listener");
	appdata_s *ad = (appdata_s *) data;
	int min_interval;
	//char sec[50];
	sensor_get_default_sensor(type,&ad->sensor[listenerSize]);
	sensor_create_listener(ad->sensor[listenerSize], &ad->listener[listenerSize]);
	sensor_get_min_interval(ad->sensor[listenerSize], &min_interval);
	sensor_listener_set_event_cb(ad->listener[listenerSize], min_interval, cb_func, NULL);
	sensor_listener_set_option(ad->listener[listenerSize], SENSOR_OPTION_ALWAYS_ON);
	sensor_listener_start(ad->listener[listenerSize]);
	listenerSize++;

	//dlog_print(DLOG_DEBUG, "MYTAG", "sensor should run now");
	/*bundle *b = bundle_create();
	bundle_add_str(b, "command", "add listener");

	if (str == SENSOR_ERROR_NONE){
		bundle_add_str(b, "data","Everything is fine");
	} else {
		bundle_add_str(b, "data","something went wrong ");
	}

	message_port_send_message("UM2FcDkLYZ.HelloAccessoryProvider", "MY_PORT", b);
	bundle_free(b);
	*/
}



bool service_app_create(void *data)
{
    // Todo: add your code here.
	//Actually do nothing
	//dlog_print(DLOG_DEBUG, "MYTAG", "Create Called");
	//startListener(data);
    return true;
}

void sendMessage(const char *str){
	bundle *b = bundle_create();
	bundle_add_str(b, "Message", str);
	message_port_send_message("UM2FcDkLYZ.HelloAccessoryProvider", "MY_PORT", b);
	bundle_free(b);
}



void close_all_files(){
	fclose(fpgyr);
	fclose(fpaccel);
	fclose(fplight);
	fclose(fppress);
}

void service_app_terminate(void *data)
{
	appdata_s *ad = (appdata_s *) data;
	bundle *b = bundle_create();

	bundle_add_str(b, "command", "service_app_terminate");
	message_port_send_message("UM2FcDkLYZ.HelloAccessoryProvider", "MY_PORT", b);
	bundle_free(b);
	close_all_files();

    return;
}

void open_all_files(){
	fpaccel = fopen(get_write_filepath("accel.txt"), "w");
	fpgyr =fopen(get_write_filepath("gyro.txt"), "w");
	fplight=fopen(get_write_filepath("light.txt"), "w");
	fppress= fopen(get_write_filepath("bar.txt"), "w");
}

void startListener(void *data){
	appdata_s *ad = (appdata_s *) data;
	open_all_files();
	if (is_supported(SENSOR_ACCELEROMETER)) {
			sendMessage("Accel Supported");
			add_listener(0,SENSOR_ACCELEROMETER, sensor_cb,data);
		}

		if (is_supported(SENSOR_LINEAR_ACCELERATION)){
			sendMessage("Lin Accel Supported");
			//add_listener(1,SENSOR_LINEAR_ACCELERATION, sensor_cb,data);
		}
		if (is_supported(SENSOR_GYROSCOPE)){
			sendMessage("Gyro Supported");
			add_listener(2,SENSOR_GYROSCOPE, sensor_cb,data);
		}
		if (is_supported(SENSOR_LIGHT)){
			sendMessage("Light Supported");
			add_listener(3,SENSOR_LIGHT, sensor_cb,data);
		}
		if (is_supported(SENSOR_HUMIDITY)){
			sendMessage("Humidity Supported");
			//add_listener(4,SENSOR_HUMIDITY, sensor_cb,data);
		}
		if (is_supported(SENSOR_MAGNETIC)){
			sendMessage("Magnetic Supported");
			//add_listener(5,SENSOR_MAGNETIC, sensor_cb,data);
		}
		if (is_supported(SENSOR_PRESSURE)){
			sendMessage("Pressure Supported");
			add_listener(6,SENSOR_PRESSURE, sensor_cb,data);
		}
		if (is_supported(SENSOR_TEMPERATURE)){
			sendMessage("Temp Supported");
			//add_listener(7,SENSOR_TEMPERATURE, sensor_cb,data);
		}

}

void write_file(const char* filepath, const char* buf)
{
	sendMessage(filepath);
    FILE *fp;
    fp = fopen(filepath,"w");
    sendMessage("File opened");
    fputs(buf,fp);
    sendMessage("put buffer in file");
    fclose(fp);
    sendMessage("Closed");
}


char* get_write_filepath(char *filename)
{

    char *write_filepath[1000];
    char *resource_path = "/home/owner/media/Others/";//app_get_shared_data_path(); // get the application data directory path

    //sendMessage(app_get_external_data_path());
    sendMessage(resource_path);
    //mkdir(resource_path, S_IRWXU|S_IRWXO );
    //char * error;
    //sprintf(i, "%d",i);
    if(resource_path)
    {
        sprintf(write_filepath,"%s%s",resource_path,filename);
        //free(resource_path);
    }

    return write_filepath;
}


void stopListener(void *data){
	appdata_s *ad = (appdata_s *) data;

	for (int i = 0; i < listenerSize; i++) {
			sensor_listener_stop(ad->listener[i]);
			sensor_destroy_listener(ad->listener[i]);
	}
}


void print_error(int error){
	switch(error){
				case RECORDER_ERROR_INVALID_PARAMETER: sendMessage("Invalid parameter"); return;
				case RECORDER_ERROR_SOUND_POLICY: sendMessage("Sound Policy"); return;
				case RECORDER_ERROR_RESOURCE_CONFLICT: sendMessage("Recource Conflict"); return;
				case RECORDER_ERROR_PERMISSION_DENIED: sendMessage("The access to the resources can not be granted"); return;
				case RECORDER_ERROR_INVALID_OPERATION: sendMessage("Invalid operation"); return;
				case RECORDER_ERROR_INVALID_STATE: sendMessage("Invalid state"); return;
				case RECORDER_ERROR_NOT_SUPPORTED: sendMessage("not supported");
			}
}


void _state_changed_cb(recorder_state_e previous, recorder_state_e current, bool by_policy, void *user_data)
{

	char b[100]  ="";
	sprintf(b, "prev %d, curr %d",previous,current);
	sendMessage(b);
	if (current == RECORDER_DEVICE_STATE_RECORDING) {
		sendMessage("Recording ....");
	}
	if (current == RECORDER_DEVICE_STATE_IDLE) {
			sendMessage("idel");
		}
	if (current == RECORDER_STATE_PAUSED) {
		sendMessage("recorder paused");
		recorder_commit(recorder);
	}
	if (current == RECORDER_STATE_CREATED) {
		sendMessage("created");
	}
	if (current == RECORDER_DEVICE_STATE_IDLE){
		sendMessage("idel");
	}
	if(current == RECORDER_STATE_NONE){
		sendMessage("NONE");
	}
	if (current == RECORDER_DEVICE_STATE_RECORDING) {
		sendMessage("recording");
	}

}

void _recording_status_cb(unsigned long long elapsed_time, unsigned long long file_size, void *user_data)
{
	sendMessage("Recording status");
}


void start_recording(void *data){


	char * filename;
	appdata_s *ad = (appdata_s *) data;
	int er = recorder_create_audiorecorder(&recorder);
	recorder_set_state_changed_cb(recorder, _state_changed_cb, NULL);
	if (er == RECORDER_ERROR_NONE){
		sendMessage("recorder created !!!");
	} else {print_error(er);}
	er = recorder_attr_set_audio_device(recorder, RECORDER_AUDIO_DEVICE_MIC);
	recorder_attr_set_size_limit(recorder, 0);
	if (er == RECORDER_ERROR_NONE){
		sendMessage("Mic set");
	} else {print_error(er);}
	er = recorder_set_audio_encoder(recorder, RECORDER_AUDIO_CODEC_AMR);
	if (er == RECORDER_ERROR_NONE){
		sendMessage("set Codec");
	} else {print_error(er);}
	er = recorder_set_file_format(recorder, RECORDER_FILE_FORMAT_3GP);
	if (er == RECORDER_ERROR_NONE){
		sendMessage("set format");
	} else {print_error(er);}
	er = recorder_attr_set_audio_samplerate(recorder, 16000);
	recorder_attr_set_audio_encoder_bitrate(recorder, 288000);
	if (er == RECORDER_ERROR_NONE){
		sendMessage("set Sampling rate");
	} else {print_error(er);}

	filename = get_write_filepath("audio.3gp");
	er = recorder_set_filename(recorder, filename);
	if (er == RECORDER_ERROR_NONE){
		sendMessage(filename);
	} else {print_error(er);}

	recorder_set_recording_status_cb(recorder, _recording_status_cb, data);

	int prep_error = recorder_prepare(recorder);
	if(prep_error == RECORDER_ERROR_NONE){
		sendMessage("prep Worked !!!");
	} else {print_error(er);}
	int error = recorder_start(recorder);
	if( error == RECORDER_ERROR_NONE){
		sendMessage("start recording called to the end !!!");

	} else {print_error(er);}
	char out[50] = " ";
	recorder_state_e state;
	recorder_get_state(recorder, &state);
	if (state == RECORDER_STATE_RECORDING) {
		sendMessage("Recording");

	}
	sprintf(out, "%d", state);
	sendMessage(out);


}


void service_app_control(app_control_h app_control, void *data)
{
	appdata_s *ad = (appdata_s *) data;
	char *operation;
	app_control_get_operation(app_control, &operation);
	if(!strcmp(operation, "start")){
		sendMessage("start Called");
		message_port_register_local_port("MY_PORT", message_port_cb, data);
		//startListener(data);
		sendMessage("start recording");
		start_recording(data);
	}
	if(!strcmp(operation, "stop")){
		sendMessage("StopCalled");
	}

    // Todo: add your code here.
	//appdata_s *ad = (appdata_s *) data; //<3
	//char *operation;
	//char *app_id;
	//int res = app_control_clone(&ad->app_control_g, app_control);
	/*
	bundle *b = bundle_create();
	bundle_add_str(b, "command", "service_app_control");
	bundle_add_str(b, "data", "app_control");
	int ret = message_port_send_message("UM2FcDkLYZ.HelloAccessoryProvider", "MY_PORT", b);
	bundle_free(b);
	//ad->app_control_g = app_control;
	app_control_get_operation(app_control, &operation);


	app_control_create(&ad->reply);
	//app_control_create(&reply);

	app_control_add_extra_data(ad->reply, APP_CONTROL_DATA_SUBJECT, "0");
	*/

	//app_control_reply_to_launch_request(ad->reply, app_control, APP_CONTROL_RESULT_SUCCEEDED);

	//app_control_reply_to_launch_request(reply, ad->app_control_g, APP_CONTROL_RESULT_SUCCEEDED);
	//app_control_re
	//app_control_destroy(reply);
//	if (is_supported(SENSOR_ACCELEROMETER)) {
//		add_listener(SENSOR_ACCELEROMETER, sensor_cb,data);
//	}


		/*if (!strcmp(operation, "http://tizen.org/appcontrol/operation/service")) {
			//char *phone_number;
			//app_control_get_extra_data(app_control, "UI_App_ID", &phone_number);



		}*/



    return;
}

static void
service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	return;
}

static void
service_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
service_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char* argv[])
{
    char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);

	return service_app_main(argc, argv, &event_callback, ad);
}
