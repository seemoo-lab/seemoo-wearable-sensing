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
#include <audio_io.h>
#include <sound_manager.h>

bool running = false;
typedef struct {
	sensor_listener_h listener[50];
	sensor_h sensor[50];
	app_control_h reply;
	app_control_h app_control_g;

} appdata_s;

/******
 * Recorder data
 */


audio_in_h input;
FILE *fp_audio;

typedef struct wavfile_header_s
{
    char    ChunkID[4];     /*  4   */
    int32_t ChunkSize;      /*  4   */
    char    Format[4];      /*  4   */

    char    Subchunk1ID[4]; /*  4   */
    int32_t Subchunk1Size;  /*  4   */
    int16_t AudioFormat;    /*  2   */
    int16_t NumChannels;    /*  2   */
    int32_t SampleRate;     /*  4   */
    int32_t ByteRate;       /*  4   */
    int16_t BlockAlign;     /*  2   */
    int16_t BitsPerSample;  /*  2   */
    char    Subchunk2ID[4]; /*  4   */
    int32_t Subchunk2Size;	/*  4   */
} wavfile_header_t;

#define SUBCHUNK1SIZE   (16)
#define AUDIO_FORMAT    (1) /*For PCM*/
#define NUM_CHANNELS    (1) // only mono
#define SAMPLE_RATE 	(16000)

#define BITS_PER_SAMPLE (16)

#define BYTE_RATE       (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE/8)
#define BLOCK_ALIGN     (NUM_CHANNELS * BITS_PER_SAMPLE/8)






/**
 * Sensor struct
 *
 *
 */
int listenerSize= 0;

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
	char size [50] = "";
	sensor_type_e type;
	sensor_get_type(sensor, &type);

	if (type == SENSOR_LIGHT) {
		sendMessage("light sensor called");
	}


	if (type == SENSOR_PRESSURE) {
		//just take first val since somehow we get 4 values by the api
		sprintf(fBuffer, "%f ",event->values[0]);
	} else {
		for (int i = 0; i < event->value_count; i++){
			//fArray[i] = &fBuffer[i];
			sprintf(fBuffer, "%f ",event->values[i]);
			strcat(out, fBuffer);
		}
	}
	sprintf(timeStamp, "%d-%02d-%02dT%02d:%02d:%02d",l_time->tm_year+1900,l_time->tm_mon,l_time->tm_mday,l_time->tm_hour,l_time->tm_min,l_time->tm_sec);
	strcat(out, timeStamp);
	strcat(out, "\r\n");
	fputs(out, get_file_by_type(type));
}


void stop_audio(){
	audio_in_unprepare(input);
	audio_in_unset_stream_cb(input);
	write_PCM16_stereo_header(fp_audio);
	audio_in_destroy(input);
	fclose(fp_audio);

}


/**
 * Writes PCM16 header to existing audio File and overwrites the first 44 bytes so concider calling
 * write_fake_header first
 */
int write_PCM16_stereo_header(FILE*   file_p)
{
    int ret;

    wavfile_header_t wav_header;
    int32_t subchunk2_size;
    int32_t chunk_size;

    size_t write_count;

    fseek(file_p, 0, SEEK_END);


    subchunk2_size  = ftell(file_p) - 44;
    chunk_size      = subchunk2_size + 36;

    wav_header.ChunkID[0] = 'R';
    wav_header.ChunkID[1] = 'I';
    wav_header.ChunkID[2] = 'F';
    wav_header.ChunkID[3] = 'F';

    wav_header.ChunkSize = chunk_size;

    wav_header.Format[0] = 'W';
    wav_header.Format[1] = 'A';
    wav_header.Format[2] = 'V';
    wav_header.Format[3] = 'E';

    wav_header.Subchunk1ID[0] = 'f';
    wav_header.Subchunk1ID[1] = 'm';
    wav_header.Subchunk1ID[2] = 't';
    wav_header.Subchunk1ID[3] = ' ';

    wav_header.Subchunk1Size = SUBCHUNK1SIZE;
    wav_header.AudioFormat = AUDIO_FORMAT;
    wav_header.NumChannels = NUM_CHANNELS;
    wav_header.SampleRate = SAMPLE_RATE;
    wav_header.ByteRate = BYTE_RATE;
    wav_header.BlockAlign = BLOCK_ALIGN;
    wav_header.BitsPerSample = BITS_PER_SAMPLE;

    wav_header.Subchunk2ID[0] = 'd';
    wav_header.Subchunk2ID[1] = 'a';
    wav_header.Subchunk2ID[2] = 't';
    wav_header.Subchunk2ID[3] = 'a';
    wav_header.Subchunk2Size = subchunk2_size;
    fseek(file_p, 0, SEEK_SET);

    write_count = fwrite( &wav_header,
                            sizeof(wavfile_header_t), 1,
                            file_p);

    ret = (1 != write_count)? -1 : 0;

    return ret;
}


void
message_port_cb(int local_port_id, const char *remote_app_id, const char *remote_port,
                bool trusted_remote_port, bundle *message, void *user_data)
{
	sendMessage("Callback Called");
	/*
	char *m;
	bundle_get_str(message, "key", &m);
	appdata_s *ad = (appdata_s *) user_data;
	char out[50]="";
	*/
	stop_audio();
	stopListener(user_data);
	service_app_exit();

}


bool is_supported(sensor_type_e type){
	bool b;
	sensor_is_supported(type, &b);
	return b;
}


void add_listener(int index, sensor_type_e type, sensor_event_cb cb_func, void *data, int interval){
	appdata_s *ad = (appdata_s *) data;
	int min_interval;
	//char sec[50];
	sensor_get_default_sensor(type,&ad->sensor[listenerSize]);
	sensor_create_listener(ad->sensor[listenerSize], &ad->listener[listenerSize]);
	sensor_get_min_interval(ad->sensor[listenerSize], &min_interval);
	sensor_listener_set_event_cb(ad->listener[listenerSize], interval, cb_func, NULL);
	sensor_listener_set_option(ad->listener[listenerSize], SENSOR_OPTION_ALWAYS_ON);
	sensor_listener_start(ad->listener[listenerSize]);
	listenerSize++;

}



bool service_app_create(void *data)
{
    // Todo: add your code here.
	//Actually do nothing when started by another app it will be called in
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
			add_listener(0,SENSOR_ACCELEROMETER, sensor_cb,data,15); // 20 ms = 50 hz
		}

		if (is_supported(SENSOR_LINEAR_ACCELERATION)){
			sendMessage("Lin Accel Supported");
			//add_listener(1,SENSOR_LINEAR_ACCELERATION, sensor_cb,data);
		}
		if (is_supported(SENSOR_GYROSCOPE)){
			sendMessage("Gyro Supported");
			add_listener(2,SENSOR_GYROSCOPE, sensor_cb,data,15);
		}
		if (is_supported(SENSOR_LIGHT)){
			sendMessage("Light Supported");
			int min_interval;
			char inter [1000] = "";
			sensor_get_min_interval(ad->sensor[listenerSize], &min_interval);
			sprintf(inter, "%d",min_interval);
			sendMessage(inter);

			add_listener(3,SENSOR_LIGHT, sensor_cb,data,0);
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
			add_listener(6,SENSOR_PRESSURE, sensor_cb,data,0);
		}
		if (is_supported(SENSOR_TEMPERATURE)){
			sendMessage("Temp Supported");
			//add_listener(7,SENSOR_TEMPERATURE, sensor_cb,data);
		}

}



void write_file(const char* filepath, const char* buf)
{
    FILE *fp;
    fp = fopen(filepath,"w");
    fputs(buf,fp);
    fclose(fp);
}


char* get_write_filepath(char *filename)
{

    char *write_filepath[1000];
    char *resource_path = "/home/owner/media/Others/";// get the application data directory path

    if(resource_path)
    {
        sprintf(write_filepath,"%s%s",resource_path,filename);
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
		//recorder_commit(recorder);
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

void
_audio_io_stream_read_cb(audio_in_h handle, size_t nbytes, void *userdata)
{
    const void * buffer = NULL;

    if (nbytes > 0) {
        /*
           Retrieve a pointer to the internal input buffer
           and the number of recorded audio data bytes
        */
        int error_code = audio_in_peek(handle, &buffer, &nbytes);
        if (error_code != AUDIO_IO_ERROR_NONE) {
            dlog_print(DLOG_ERROR, LOG_TAG, "audio_in_peek() failed! Error code = %d", error_code);

            return;
        }

        /* Store the recorded audio data in the file */
        fwrite(buffer, sizeof(char), nbytes, fp_audio);

        /* Remove the recorded audio data from the internal input buffer */
        error_code = audio_in_drop(handle);
        if (error_code != AUDIO_IO_ERROR_NONE)
            dlog_print(DLOG_ERROR, LOG_TAG, "audio_in_drop() failed! Error code = %d", error_code);
    }
}

void write_fake_header(FILE *file){
	char bytes [44];
	for (int i =0 ; i< 43; i++){
		bytes[i] = 0;
	}

	fputs(bytes, file);

}

void start_recording(void *data){


	char * filename;
	filename = get_write_filepath("audio.wav");
	fp_audio = fopen(filename, "w");

	audio_in_create(SAMPLE_RATE, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_S16_LE, &input); //set up recorder for PCM recording audio
	audio_in_set_stream_cb(input, _audio_io_stream_read_cb, NULL); //set callback for recording
	audio_in_prepare(input); // starts the audio recording

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
		startListener(data);
	}
	if(!strcmp(operation, "stop")){
		sendMessage("StopCalled: outdated"); //outdated; should not be called anymore
	}
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
