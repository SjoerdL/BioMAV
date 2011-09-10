#ifndef DRONECONTROL_C_
#define DRONECONTROL_C_

#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

typedef void(*NavdataCallback)(const navdata_unpacked_t* const navData, void* userdata);
typedef void(*VideoImageCallback)(char* imageData, int width, int height, void* userdata);

int drone_init(void* userdata);
int drone_free();

void drone_setEmergency(int b);
void drone_takeoff();
void drone_land();

int drone_getEmergency();

void drone_hover();
void drone_fly(float pitch, float strafe, float upwards, float yaw);

void drone_registerNavdataCallback(NavdataCallback callback);

void drone_registerVideoCallback(VideoImageCallback callback);

#endif // DRONECONTROL_C_

