#include "DroneControlC.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

// Defines for ARDrone SDK
#ifndef __linux__
#define __linux__
#endif

#define NO_ARDRONE_MAINLOOP
#define _MOBILE
#define GNU_LINUX
#define D_REENTRANT
#define COMPIL_MODE_PROD
#define NDEBUG
#define __LINUX__
#define TARGET_CPU_ARM 0
#define TARGET_CPU_X86 1
#define USE_WIFI
#define USE_VLIB
#define USE_LINUX

#include <config.h>
#include <ardrone_api.h>
#include <ardrone_tool/ardrone_tool.h>
#include <VP_Api/vp_api_thread_helper.h>
#include <ardrone_tool/Control/ardrone_control.h>
#include <ardrone_tool/UI/ardrone_input.h>

#include <VP_SDK/VP_Os/linux/vp_os_serial.h>
#include <VP_Stages/vp_stages_yuv2rgb.h>
#include <VP_Stages/vp_stages_buffer_to_picture.h>
#include <VLIB/Stages/vlib_stage_decode.h>

#include <VP_Api/vp_api.h>
#include <VP_Api/vp_api_error.h>
#include <VP_Api/vp_api_stage.h>
#include <VP_Api/vp_api_picture.h>
#include <VP_Stages/vp_stages_io_file.h>
#include <VP_Stages/vp_stages_i_camif.h>

#include <ardrone_tool/Video/video_com_stage.h>
#include <ardrone_tool/ardrone_tool.h>
#include <ardrone_tool/Com/config_com.h>

#include <pthread.h>

#include <SDL/SDL.h>

#define DRONE_NB_STAGES 10

#define DRONE_LAND_WAIT_LIMIT 15000
#define DRONE_EMERGENCY_WAIT_LIMIT 5000

#define EMERGENCY_SIGNAL_PULSE_DURATION 200

// This limit seems necessary - otherwise the application crashes - BUGGY DRONE INTERFACE OMG!
#define MAX_AT_COMMANDS_PER_NAVDATA 2

PROTO_THREAD_ROUTINE(drone_main_thread, data);
void ardrone_at_set_navdata_all(); // seems to be defined nowhere! AWESOME


pthread_mutex_t drone_sharedDataMutex = PTHREAD_MUTEX_INITIALIZER;
void* drone_userdata = NULL;
int drone_doRun = 0;
int drone_doRunArdrone = 0;
int drone_landed = 0;
int drone_emergency = 0;
int drone_targetEmergency = 0;
int drone_receivedFirstNavdata = 0;
int drone_atCommands = 0;

pthread_mutex_t drone_navdataCallbackMutex = PTHREAD_MUTEX_INITIALIZER;
NavdataCallback drone_navdataCallback;

pthread_mutex_t drone_imageDataMutex = PTHREAD_MUTEX_INITIALIZER;
VideoImageCallback drone_videoCallback = NULL;


int drone_init(void* userdata) {
  drone_userdata = userdata;

  C_RESULT res;
  drone_doRunArdrone = 1;
  res = ardrone_tool_setup_com(NULL);
  if (SUCCEED(res)) {
    char ipAddress[256] = "192.168.1.1";
    ardrone_tool_init(ipAddress, strlen(ipAddress), NULL);

    drone_doRun = 1;
    drone_targetEmergency = 1;
    drone_navdataCallback = NULL;
    drone_videoCallback = NULL;
    drone_atCommands = 0;
    START_THREAD(drone_main_thread, NULL);
    START_THREAD(drone_video_stage_thread, NULL);

    return 1;
  } else {
    drone_doRunArdrone = 0;
  }

  return 0;
}

bool_t ardrone_tool_exit( void ) {
//  printf("Exit? %s\n", (drone_doRunArdrone ? "No" : "Yes"));
  return !drone_doRunArdrone;
}

C_RESULT ardrone_tool_shutdown_custom() {
  printf("Custom shutdown called\n");
  return C_OK;
}

void drone_hover() {
  pthread_mutex_lock(&drone_sharedDataMutex);
  if (!drone_targetEmergency && !drone_emergency) {
    if (drone_atCommands++ < MAX_AT_COMMANDS_PER_NAVDATA) {
      ardrone_at_set_progress_cmd(0, 0, 0, 0, 0);
    }
  }
  pthread_mutex_unlock(&drone_sharedDataMutex);
}

void drone_fly(float pitch, float strafe, float upwards, float yaw) {
  pthread_mutex_lock(&drone_sharedDataMutex);
  if (!drone_targetEmergency && !drone_emergency) {
    if (drone_atCommands++ < MAX_AT_COMMANDS_PER_NAVDATA) {
      ardrone_at_set_progress_cmd(1, strafe, pitch, upwards, yaw);
    }
  }
  pthread_mutex_unlock(&drone_sharedDataMutex);
}

void drone_takeoff() {
  pthread_mutex_lock(&drone_sharedDataMutex);
  if (!drone_targetEmergency && !drone_emergency) {
    if (drone_atCommands++ < MAX_AT_COMMANDS_PER_NAVDATA) {
      ardrone_tool_set_ui_pad_start(1);
    }
  }
  pthread_mutex_unlock(&drone_sharedDataMutex);
}

void drone_land() {
  pthread_mutex_lock(&drone_sharedDataMutex);
  if (!drone_targetEmergency && !drone_emergency) {
    if (drone_atCommands++ < MAX_AT_COMMANDS_PER_NAVDATA) {
      ardrone_tool_set_ui_pad_start(0);
    }
  }
  pthread_mutex_unlock(&drone_sharedDataMutex);
}

void drone_setEmergency(int b) {
  drone_targetEmergency = b != 0;
}

int drone_getEmergency() {
  return drone_targetEmergency;
}

int drone_free() {
  pthread_mutex_lock(&drone_sharedDataMutex);
  pthread_mutex_lock(&drone_navdataCallbackMutex);
  drone_doRun = 0;
  drone_navdataCallback = NULL;
  pthread_mutex_unlock(&drone_navdataCallbackMutex);
  pthread_mutex_unlock(&drone_sharedDataMutex);

  printf("Joining main thread\n");
  JOIN_THREAD(drone_main_thread);

  drone_doRunArdrone = 0;

  if (drone_receivedFirstNavdata) {
    ardrone_tool_shutdown();
  } else {
    fprintf(stderr, "Warning! Did not shut down ardrone_tool because there was never a connection with the drone (freeze protection)!");
  }

  drone_userdata = NULL;

  return 1;
}

void drone_registerNavdataCallback(NavdataCallback callback) {
  pthread_mutex_lock(&drone_sharedDataMutex);
  pthread_mutex_lock(&drone_navdataCallbackMutex);
  drone_navdataCallback = callback;
  pthread_mutex_unlock(&drone_navdataCallbackMutex);
  pthread_mutex_unlock(&drone_sharedDataMutex);
}

DEFINE_THREAD_ROUTINE(drone_main_thread, data) {
  ardrone_tool_set_ui_pad_select(0); // Do not send any emergency signal
  ardrone_at_set_navdata_all();

//  Uint32 emergencyPulseStartTime = SDL_GetTicks() + EMERGENCY_SIGNAL_PULSE_DURATION;
//
//  int doRun = 1;
//
//  while (doRun) {
//    ardrone_tool_update();
//
//    pthread_mutex_lock(&drone_sharedDataMutex);
//    int emergencyPulseMS = SDL_GetTicks() - emergencyPulseStartTime;
//    if ((drone_emergency != drone_targetEmergency) && ((emergencyPulseMS > 2 * EMERGENCY_SIGNAL_PULSE_DURATION) || (emergencyPulseMS < EMERGENCY_SIGNAL_PULSE_DURATION))) {
//      ardrone_tool_set_ui_pad_select(1);
//
//      if (emergencyPulseMS > 2 * EMERGENCY_SIGNAL_PULSE_DURATION) {
//        emergencyPulseStartTime = SDL_GetTicks();
//      }
//    } else {
//      ardrone_tool_set_ui_pad_select(0);
//    }
//
//    doRun = drone_doRun;
//    pthread_mutex_unlock(&drone_sharedDataMutex);
//  }
//
  Uint32 startTime;

  // Enforce landing...
  startTime = SDL_GetTicks();
  while ((!drone_landed) && (SDL_GetTicks() - startTime < DRONE_LAND_WAIT_LIMIT)) {
    drone_land();
    ardrone_tool_update();
  }
  if (!drone_landed) {
    fprintf(stderr, "Warning! Drone could not land within %d seconds! Trying to set emergency flag...\n", DRONE_LAND_WAIT_LIMIT / 1000);
    startTime = SDL_GetTicks();
    while ((!(drone_emergency) || (drone_landed)) && (SDL_GetTicks() - startTime < DRONE_EMERGENCY_WAIT_LIMIT)) {
      drone_setEmergency(1);
      ardrone_tool_update();
    }

    if ((!(drone_emergency) || (drone_landed))) {
      fprintf(stderr, "Error! Drone could not land and no emergency flag could be set. If it is still flying: Good luck!\n");
    }
  }

  return 0;
}

C_RESULT drone_navdata_init( void* cfg ) {
  pthread_mutex_lock(&drone_sharedDataMutex);
  drone_receivedFirstNavdata = 0;
  pthread_mutex_unlock(&drone_sharedDataMutex);

  return C_OK;
}

C_RESULT drone_navdata_process( const navdata_unpacked_t* const pnd ) {
  pthread_mutex_lock(&drone_sharedDataMutex);
  pthread_mutex_lock(&drone_navdataCallbackMutex);

  drone_atCommands = 0;
  drone_receivedFirstNavdata = 1;

  drone_emergency = (pnd->ardrone_state & ARDRONE_EMERGENCY_MASK) != 0;
  drone_landed = (pnd->ardrone_state & ARDRONE_FLY_MASK) == 0;

  NavdataCallback navdataCallback = drone_navdataCallback;
  void* userData = drone_userdata;
  pthread_mutex_unlock(&drone_sharedDataMutex);

  if (navdataCallback != NULL) {
    navdataCallback(pnd, userData);
  }

  pthread_mutex_unlock(&drone_navdataCallbackMutex);

  return C_OK;
}

C_RESULT drone_navdata_release( void ) {
  return C_OK;
}

/********************* VIDEO PROCESSING ************************/
void drone_registerVideoCallback(VideoImageCallback callback) {
  pthread_mutex_lock(&drone_imageDataMutex);
  drone_videoCallback = callback;
  pthread_mutex_unlock(&drone_imageDataMutex);
}


C_RESULT output_video_stage_open( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out) {
  return (SUCCESS);
}

C_RESULT output_video_stage_transform( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out) {
  pthread_mutex_lock(&drone_sharedDataMutex);
  pthread_mutex_lock(&drone_imageDataMutex);
  void* userData = drone_userdata;
  pthread_mutex_unlock(&drone_sharedDataMutex);

  if (drone_videoCallback != NULL) {
    drone_videoCallback(in->buffers[0], QVGA_WIDTH, QVGA_HEIGHT, userData);
  }
  pthread_mutex_unlock(&drone_imageDataMutex);

  return (SUCCESS);
}

C_RESULT output_video_stage_close( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out) {
  return (SUCCESS);
}

const vp_api_stage_funcs_t vp_video_output_funcs = {
  NULL,
  (vp_api_stage_open_t)output_video_stage_open,
  (vp_api_stage_transform_t)output_video_stage_transform,
  (vp_api_stage_close_t)output_video_stage_close
};


DEFINE_THREAD_ROUTINE(drone_video_stage_thread, data)
{
  PIPELINE_HANDLE pipeline_handle;

  printf("Vision thread started\n");

  C_RESULT res;

  vp_api_io_pipeline_t    pipeline;
  vp_api_io_data_t        out;
  vp_api_io_stage_t       stages[DRONE_NB_STAGES];

  vp_api_picture_t picture;

  video_com_config_t              icc;
  vlib_stage_decoding_config_t    vec;
  vp_stages_yuv2rgb_config_t      yuv2rgbconf;

  /// Picture configuration
  picture.format        = PIX_FMT_YUV420P;

  picture.width         = QVGA_WIDTH;
  picture.height        = QVGA_HEIGHT;
  picture.framerate     = 30;

  picture.y_buf   = vp_os_malloc( QVGA_WIDTH * QVGA_HEIGHT     );
  picture.cr_buf  = vp_os_malloc( QVGA_WIDTH * QVGA_HEIGHT / 4 );
  picture.cb_buf  = vp_os_malloc( QVGA_WIDTH * QVGA_HEIGHT / 4 );

  picture.y_line_size   = QVGA_WIDTH;
  picture.cb_line_size  = QVGA_WIDTH / 2;
  picture.cr_line_size  = QVGA_WIDTH / 2;

  vp_os_memset(&icc,          0, sizeof( icc ));
  vp_os_memset(&vec,          0, sizeof( vec ));
  vp_os_memset(&yuv2rgbconf,  0, sizeof( yuv2rgbconf ));

  icc.com                 = COM_VIDEO();
  icc.buffer_size         = 100000;
  icc.protocol            = VP_COM_UDP;
  COM_CONFIG_SOCKET_VIDEO(&icc.socket, VP_COM_CLIENT, VIDEO_PORT, wifi_ardrone_ip);

  vec.width               = QVGA_WIDTH;
  vec.height              = QVGA_HEIGHT;
  vec.picture             = &picture;
  vec.block_mode_enable   = TRUE;
  vec.luma_only           = FALSE;

  yuv2rgbconf.rgb_format = VP_STAGES_RGB_FORMAT_RGB24;

  pipeline.nb_stages = 0;

  stages[pipeline.nb_stages].type    = VP_API_INPUT_SOCKET;
  stages[pipeline.nb_stages].cfg     = (void *)&icc;
  stages[pipeline.nb_stages].funcs   = video_com_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_FILTER_DECODER;
  stages[pipeline.nb_stages].cfg     = (void*)&vec;
  stages[pipeline.nb_stages].funcs   = vlib_decoding_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_FILTER_YUV2RGB;
  stages[pipeline.nb_stages].cfg     = (void*)&yuv2rgbconf;
  stages[pipeline.nb_stages].funcs   = vp_stages_yuv2rgb_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_OUTPUT_SDL;
  stages[pipeline.nb_stages].cfg     = NULL;
  stages[pipeline.nb_stages].funcs   = vp_video_output_funcs;

  pipeline.nb_stages++;

  pipeline.stages = &stages[0];

  pthread_mutex_lock(&drone_sharedDataMutex);
  int doRun = drone_doRun;
  pthread_mutex_unlock(&drone_sharedDataMutex);

  /* Processing of a pipeline */
  if (doRun) {
    res = 0;
    res = vp_api_open(&pipeline, &pipeline_handle);

    if (SUCCEED(res)) {
      int loop = SUCCESS;
      out.status = VP_API_STATUS_PROCESSING;

      while (doRun && (loop == SUCCESS)) {
          pthread_mutex_lock(&drone_sharedDataMutex);
          doRun = drone_doRun;
          pthread_mutex_unlock(&drone_sharedDataMutex);

          if( SUCCEED(vp_api_run(&pipeline, &out)) ) {
            if ((out.status == VP_API_STATUS_PROCESSING || out.status == VP_API_STATUS_STILL_RUNNING)) {
              loop = SUCCESS;
            }
          }
      }

      fprintf(stderr, "BUG and WORKAROUND: vp_api_close was never called because it is buggy\n");
//      vp_api_close(&pipeline, &pipeline_handle); // <- TODO: do not call this - fix this function in SDK
    }
  }

  vp_os_free(picture.y_buf);
  vp_os_free(picture.cr_buf);
  vp_os_free(picture.cb_buf);

  printf("Vision thread terminated\n");

  return (THREAD_RET) 0;
}


BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(drone_navdata_init, drone_navdata_process, drone_navdata_release, NULL)
END_NAVDATA_HANDLER_TABLE

BEGIN_THREAD_TABLE //Mandatory
  THREAD_TABLE_ENTRY( drone_main_thread, 20 )
  THREAD_TABLE_ENTRY( drone_video_stage_thread, 20 )
  THREAD_TABLE_ENTRY( navdata_update, 20 ) // Mandatory
  THREAD_TABLE_ENTRY( ardrone_control, 20 ) // Mandatory
END_THREAD_TABLE //Mandatory


