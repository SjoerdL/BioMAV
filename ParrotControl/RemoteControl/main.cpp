/*
 * main.cpp
 *
 *  Created on: 21.04.2011
 *      Author: paul
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>

#include <IL/il.h>

#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include "ParrotJoystick.hpp"
#include "DroneControl.hpp"

using namespace std;

#define MAIN_WIN_WIDTH  640
#define MAIN_WIN_HEIGHT 480

#define CVIS_FRAMECOLOR 0xFF0000FF
#define CVIS_TEXTCOLOR 0xFFFFFFFF
#define CVIS_BARCOLOR 0xFF0000FF
#define CVIS_BARBGCOLOR 0x7F00FFFF

fstream outputFile ("navdataLog.txt", fstream::out);

int controllerVisualizationTimer = SDL_GetTicks();

int logStartTime = SDL_GetTicks();
int lastLogTime = SDL_GetTicks();
int imageCounter = 1;
const char logHeader[] = "LogTime\temergency?\tstartMode?\thovering?\tsetForwards\tsetSidewards\tsetYaw\tsetHeightSpeed\tNavDataTime\tBattery\tHeight\tEuler.Theta\tEuler.Phi\tGroundspeed.x\tGroundspeed.y\tGroundspeed.z\tAcc.x\tAcc.y\tAcc.z\tGyro.x\tGyro.y\tGyro.z\timage.timeStamp\timage.filename";

void logData(DroneControl& drone) {
  if (SDL_GetTicks() - lastLogTime < 50) {
    return;
  }
  lastLogTime = SDL_GetTicks();

  float forward, sideward, yaw, height;
  drone.getSetMovementVector(forward, sideward, yaw, height);

  outputFile << (SDL_GetTicks() - logStartTime) << "\t";
  outputFile << drone.isEmergencySet() << "\t";
  outputFile << drone.getStartMode() << "\t";
  outputFile << drone.isHoverSet() << "\t";
  outputFile << forward << "\t";
  outputFile << sideward << "\t";
  outputFile << yaw << "\t";
  outputFile << height << "\t";

  outputFile << drone.getNavdataTimestamp() << "\t";
  outputFile << drone.getBatteryLevel() << "\t";
  outputFile << drone.getHeight() << "\t";
  outputFile << drone.getEulerTheta() << "\t";
  outputFile << drone.getEulerPhi() << "\t";
  outputFile << drone.getGroundspeedX() << "\t";
  outputFile << drone.getGroundspeedY() << "\t";
  outputFile << drone.getGroundspeedZ() << "\t";
  outputFile << drone.getAccX() << "\t";
  outputFile << drone.getAccY() << "\t";
  outputFile << drone.getAccZ() << "\t";
  outputFile << drone.getGyroX() << "\t";
  outputFile << drone.getGyroY() << "\t";
  outputFile << drone.getGyroZ() << "\t";
  outputFile << drone.getImageTimestamp() << "\t";
  outputFile << imageCounter << ".jpg" << "\t";

  stringstream imgFileName (stringstream::in | stringstream::out);
  imgFileName << imageCounter << ".jpg";

  ilSaveImage(imgFileName.str().c_str());

  imageCounter++;
  outputFile << endl;
}

void drawControllerVisualization(SDL_Surface* surface, int x, int y, const ParrotController& controller) {
  if (SDL_GetTicks() - controllerVisualizationTimer > 500) {
    controllerVisualizationTimer = SDL_GetTicks();
  }

  rectangleColor(surface, x, y, x + 250, y + 200, CVIS_FRAMECOLOR);

  if (controller.isEmergency()) {
    stringColor(surface, x + 10, y + 5, "Emergency mode set", (SDL_GetTicks() - controllerVisualizationTimer) < 250 ? 0xFF0000FF : CVIS_TEXTCOLOR);
  } else {
    stringColor(surface, x + 10, y + 5, "Emergency mode cleared", CVIS_TEXTCOLOR);

    stringColor(surface, x + 10, y + 25, controller.doStart() ? "Flight mode: Start" : "Flight mode: Land", CVIS_TEXTCOLOR);

    y += 35;

    // Height setting
    boxColor(surface, x + 30, y + 20, x + 40, y + 120, CVIS_BARBGCOLOR);
    boxColor(surface, x + 31, y + 120 - (int) (controller.getHeightSetting() * 100.0), x + 39, y + 120, CVIS_BARCOLOR);
    stringColor(surface, x + 10, y + 5, "Height", CVIS_TEXTCOLOR);

    // Spin left/right
    boxColor(surface, x + 10, y + 150, x + 60, y + 160, CVIS_BARBGCOLOR);
    boxColor(surface, x + 10 + 25, y + 150, x + 10 + 25 + (int) (25 * controller.getSpinSetting()), y + 160, CVIS_BARCOLOR);
    stringColor(surface, x + 20, y + 135, "Yaw", CVIS_TEXTCOLOR);

    // Movement cross
    boxColor(surface, x + 95, y + 20, x + 95 + 100, y + 20 + 100, CVIS_BARBGCOLOR);
    int tx = x + 95 + 50 + (int) (44 * controller.getStrafeSetting());
    int ty = y + 20 + 50 - (int) (44 * controller.getForwardSetting());
    boxColor(surface, tx - 5, ty - 5, tx + 5, ty + 5, CVIS_BARCOLOR);
    stringColor(surface, x + 70, y + 5, "Movement direction", CVIS_TEXTCOLOR);
  }
}

int main(int args, char** argv) {
  ilInit();
  ILuint ilimage = ilGenImage();
  ilBindImage(ilimage);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == 0) {
    logStartTime = SDL_GetTicks();
    lastLogTime = SDL_GetTicks();
    outputFile << "#" << logHeader << endl;

    try {
      int joystickCount = SDL_NumJoysticks();

      if (args <= 1) {
        // Enumerate available joysticks
        cout << "Joystick ID required. " << joystickCount << " available joysticks:" << endl;
        for (int i = 0; i < joystickCount; i++) {
          cout << "  " << i << ": " << SDL_JoystickName(i) << endl;
        }
      } else {
        int joystickID = atoi(argv[1]);
        if (!((joystickID >= 0) && (joystickID < joystickCount))) {
          cerr << "Wrong joystick ID specified: " << joystickID << " must be between 0 and " << joystickCount - 1 << endl;
        } else {
          // Init joystick
          ParrotJoystick joystick (joystickID);
          if (!joystick.loadSettings("joystick.cfg")) {
            joystick.calibrateAndAssociate();
            joystick.saveSettings("joystick.cfg");
          }

          // Init graphics
          SDL_Surface* frameBuffer = SDL_SetVideoMode(MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT, 0, 0);
          SDL_Surface* droneImageBuffer = NULL;

          if (frameBuffer == NULL) {
            cerr << "Failed to initialize frame buffer - quitting" << endl;
          } else {
            SDL_WM_SetCaption("Parrot AR Drone Remote Control", NULL);

            DroneControl drone;
            int lastDroneError = drone.getErrorState();

            cout << "Drone object created!" << endl;

            bool runLoop = true;
            while (runLoop) {
              joystick.update();

              boxColor(frameBuffer, 0, 0, MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT, 0x000000FF);
              drawControllerVisualization(frameBuffer, 0, 0, joystick);
              if (droneImageBuffer != NULL) {
                SDL_Rect dstRect;
                dstRect.x = 20;
                dstRect.y = 220;
                SDL_BlitSurface(droneImageBuffer, NULL, frameBuffer, &dstRect);
              }
              SDL_Flip(frameBuffer);

              // Process joystick stuff
              drone.setEmergency(joystick.isEmergency());
              drone.setStartMode(joystick.doStart());
//              cout << "Settings: " << fabs(joystick.getForwardSetting()) << " " << fabs(joystick.getStrafeSetting()) << " " << fabs(joystick.getSpinSetting()) << endl;
              if (fabs(joystick.getForwardSetting()) + fabs(joystick.getStrafeSetting() + fabs(joystick.getSpinSetting())) <= 0.01) {
                drone.hover();
              } else {
                drone.setForward(joystick.getForwardSetting());
                drone.setSidewards(joystick.getStrafeSetting());
                drone.setYawSpeed(joystick.getSpinSetting());
              }
              drone.setHeight(joystick.getHeightSetting());

              // Show drone problem
              if (lastDroneError != drone.getErrorState()) {
                lastDroneError = drone.getErrorState();
                cout << "Drone problem: ";
                switch (drone.getErrorState()) {
                  case DRONE_ERROR_NO_ERROR:
                    cout << "No error" << endl;
                    break;
                  case DRONE_ERROR_BATTERY_LOW:
                    cout << "Battery low!" << endl;
                    break;
                  case DRONE_ERROR_ANGLE_ERROR:
                    cout << "Angles out of range!" << endl;
                    break;
                  case DRONE_ERROR_MOTOR_ERROR:
                    cout << "Motor problem!" << endl;
                    break;
                  case DRONE_ERROR_COMMUNICATION_LOST:
                    cout << "Communication lost!" << endl;
                    break;
                  case DRONE_ERROR_EMERGENCY:
                  default:
                    cout << "General emergency!" << endl;
                }
              }

              // Process image
              char* imageData = drone.getImageData();
              if (imageData != NULL) {
                int width = drone.getImageWidth();
                int height = drone.getImageHeight();
                if ((droneImageBuffer == NULL) || (droneImageBuffer->w != width) || (droneImageBuffer->h != height)) {
                  if (droneImageBuffer != NULL) {
                    SDL_FreeSurface(droneImageBuffer);
                  }
                  droneImageBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000);
                }
                ilLoadDataL(imageData, 3 * width * height, width, height, 1, 3);
                SDL_LockSurface(droneImageBuffer);
                for (int y = 0; y < height; y++) {
                  memcpy(&(((char*) droneImageBuffer->pixels)[y * droneImageBuffer->pitch]), &(imageData[y * width * 3]), width * 3);
                }
                SDL_UnlockSurface(droneImageBuffer);
              }

              // Log data!
              logData(drone);

              SDL_Event event;
              if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                  runLoop = false;
                }
              } else {
                SDL_Delay(1);
              }
            }
          }

          if (droneImageBuffer != NULL) {
            SDL_FreeSurface(droneImageBuffer);
          }
        }
      }
    }
    catch (string s) {
      cerr << "Error: " << s << endl;
    }

    SDL_Quit();
  }

  ilShutDown();

  return 0;
}

