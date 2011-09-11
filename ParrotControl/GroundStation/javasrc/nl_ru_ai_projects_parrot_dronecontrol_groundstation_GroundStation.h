/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation */

#ifndef _Included_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
#define _Included_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
#ifdef __cplusplus
extern "C" {
#endif
#undef nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_LANDED
#define nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_LANDED 0L
#undef nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_FLYING
#define nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_FLYING 1L
#undef nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_EMERGENCY
#define nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_EMERGENCY 2L
#undef nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_DISCONNECTED
#define nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_FLYINGSTATE_DISCONNECTED 3L
/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    createInstance0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_createInstance0
  (JNIEnv *, jclass);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    getFrontCameraImage0
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getFrontCameraImage0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    getFrontCameraTimeStamp0
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getFrontCameraTimeStamp0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    getRotationVector0
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getRotationVector0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    getSensoryTimeStamp0
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getSensoryTimeStamp0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    clearEmergencyFlag0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_clearEmergencyFlag0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    flyForward0
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_flyForward0
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    flySideways0
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_flySideways0
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    getFlyingState0
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_getFlyingState0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    hover0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_hover0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    land0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_land0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    takeoff0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_takeoff0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    setEmergencyFlag0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_setEmergencyFlag0
  (JNIEnv *, jobject);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    setHeight0
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_setHeight0
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation
 * Method:    spin0
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_nl_ru_ai_projects_parrot_dronecontrol_groundstation_GroundStation_spin0
  (JNIEnv *, jobject, jdouble);

#ifdef __cplusplus
}
#endif
#endif