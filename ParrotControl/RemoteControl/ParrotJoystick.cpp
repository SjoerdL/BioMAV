#include "ParrotJoystick.hpp"

#include <iostream>
#include <fstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void ParrotJoystick :: initMostTravelCheck() {
  for (JoyObjectVector::iterator it = joystickObjects.begin(); it < joystickObjects.end(); it++) {
    (*it)->resetTravelCounter();
  }
}

shared_ptr<JoyObject> ParrotJoystick :: getMostTraveled() {
  float mostTraveled = 0;
  shared_ptr<JoyObject> res;

  for (JoyObjectVector::iterator it = joystickObjects.begin(); it < joystickObjects.end(); it++) {
    if ((*it)->getTravelCounter() > mostTraveled) {
      res = *it;
      mostTraveled = (*it)->getTravelCounter();
    }
  }

  return res;
}

shared_ptr<JoyObject> ParrotJoystick :: selectControl(std::string text) {
  cout << "Select control for " << text << endl;

  update();
  initMostTravelCheck();

  shared_ptr<JoyObject> res;
  while ((res == NULL) || (res->getTravelCounter() < 5)) {
    SDL_Delay(100);
    update();
    shared_ptr<JoyObject> newRes = getMostTraveled();
    if (newRes != res) {
      res = newRes;
      cout << "  New control: " << res->getName() << endl;
    }
  }

  cout << "Move control " << res->getName() << " to maximum (positive) position";
  {string s; getline(cin, s);}
  update();
  res->markMaximum();
  cout << "Move control " << res->getName() << " to center location and press enter";
  {string s; getline(cin, s);}
  update();
  res->markCenter();

  return res;
}

shared_ptr<JoyObject> ParrotJoystick :: findControl(const std::string& name) {
  shared_ptr<JoyObject> controlObject;
  for (JoyObjectVector::iterator it = joystickObjects.begin(); it < joystickObjects.end(); it++) {
    if ((*it)->getName() == name) {
      controlObject = *it;
    }
  }
  return controlObject;
}

void ParrotJoystick :: update() {
  SDL_JoystickUpdate();

  for (JoyObjectVector::iterator it = joystickObjects.begin(); it < joystickObjects.end(); it++) {
    (*it)->update();
  }
}

void ParrotJoystick :: calibrateAndAssociate() {
  heightControl = selectControl("Height");
  spinControl = selectControl("Spin");
  strafeControl = selectControl("Strafe");
  forwardControl = selectControl("Forward");
  emergencyButton = selectControl("Emergency");
  startButton = selectControl("Start");
}

float ParrotJoystick :: getHeightSetting() const {
  if (heightControl == NULL)
    return 0;
  return MAX(0, MIN(1, heightControl->getPosition()));
}

float ParrotJoystick :: getSpinSetting() const {
  if (spinControl == NULL)
    return 0;
  return MAX(-1, MIN(1, spinControl->getPosition()));
}

float ParrotJoystick :: getForwardSetting() const {
  if (forwardControl == NULL)
    return 0;
  return MAX(-1, MIN(1, forwardControl->getPosition()));
}

float ParrotJoystick :: getStrafeSetting() const {
  if (strafeControl == NULL)
    return 0;
  return MAX(-1, MIN(1, strafeControl->getPosition()));
}

bool ParrotJoystick :: doStart() const {
  if (startButton == NULL)
    return false;
  return startButton->getPosition() > 0.5;
}

bool ParrotJoystick :: isEmergency() const {
  if (emergencyButton == NULL)
    return true;
  return emergencyButton->getPosition() > 0.5;
}

bool ParrotJoystick :: saveSettings(const std::string& fileName) {
  ofstream settingsFile (fileName.c_str(), ios_base::trunc | ios_base::out);
  if (settingsFile.is_open()) {
    settingsFile << name << endl;

    settingsFile << "ASSIGNMENTS" << endl;

    settingsFile << "Forward Control:";
    if (forwardControl != NULL) {
      settingsFile << " " << forwardControl->getName();
    }
    settingsFile << endl;

    settingsFile << "Strafe Control:";
    if (strafeControl != NULL) {
      settingsFile << " " << strafeControl->getName();
    }
    settingsFile << endl;

    settingsFile << "Spin Control:";
    if (spinControl != NULL) {
      settingsFile << " " << spinControl->getName();
    }
    settingsFile << endl;

    settingsFile << "Height Control:";
    if (heightControl != NULL) {
      settingsFile << " " << heightControl->getName();
    }
    settingsFile << endl;

    settingsFile << "Start Control:";
    if (startButton != NULL) {
      settingsFile << " " << startButton->getName();
    }
    settingsFile << endl;

    settingsFile << "Emergency Control:";
    if (emergencyButton != NULL) {
      settingsFile << " " << emergencyButton->getName();
    }
    settingsFile << endl;

    settingsFile << "CALIBRATION" << endl;

    for (JoyObjectVector::iterator it = joystickObjects.begin(); it < joystickObjects.end(); it++) {
      settingsFile << (*it)->getName() << ": " << (*it)->serializeSettings() << endl;
    }

    settingsFile.close();

    return true;
  }

  return false;
}

bool ParrotJoystick :: loadSettings(const std::string& fileName) {
  bool res = false;

  ifstream settingsFile (fileName.c_str());

  if (settingsFile.is_open()) {
    string line;
    getline(settingsFile, line);
    trim(line);

    if ((!settingsFile.eof()) && (settingsFile.good())) {
      if (line != name) {
        cerr << "Warning! Configuration file does not seem to be made for this joystick" << endl;
      }

      getline(settingsFile, line);
      trim(line);
      if ((line == "ASSIGNMENTS") && (!settingsFile.eof()) && (settingsFile.good())) {
        vector<string> assignmentLines;
        do {
          getline(settingsFile, line);
          trim(line);
          if ((line != "CALIBRATION") && (!line.empty())) {
            assignmentLines.push_back(line);
          }
        } while ((line != "CALIBRATION") && (!settingsFile.eof()) && (settingsFile.good()));

        if (line == "CALIBRATION") {
          res = true;
          while ((!settingsFile.eof()) && res) {
            getline(settingsFile, line);
            trim(line);

            // Process calibration lines
            if (!line.empty()) {
              res = false;
              vector<string> splits;
              split(splits, line, is_any_of(":"));
              if (splits.size() == 2) {
                // Find control
                string controlName = splits[0];
                string configString = splits[1];
                trim(controlName);
                trim(configString);

                shared_ptr<JoyObject> controlObject = findControl(controlName);

                if (controlObject != NULL) {
                  res = controlObject->loadSettings(configString);
                }
              }

              if (!res) {
                cerr << "Erronous joystick configuration line: " << line << endl;
                break;
              }
            }
          }

          // Now process the saved control assignments
          if (res) {
            forwardControl.reset();
            strafeControl.reset();
            spinControl.reset();
            heightControl.reset();
            startButton.reset();
            emergencyButton.reset();

            for (vector<string>::iterator it = assignmentLines.begin(); it < assignmentLines.end(); it++) {
              res = false;
              vector<string> splits;
              split(splits, *it, is_any_of(":"));

              if (splits.size() == 2) {
                string controlTarget = splits[0];
                trim(controlTarget);
                string controlName = splits[1];
                trim(controlName);

                shared_ptr<JoyObject> controlObject = findControl(controlName);
                if (controlObject != NULL) {
                  if (controlTarget == "Forward Control") {
                    forwardControl = controlObject;
                    res = true;
                  }
                  if (controlTarget == "Strafe Control") {
                    strafeControl = controlObject;
                    res = true;
                  }
                  if (controlTarget == "Spin Control") {
                    spinControl = controlObject;
                    res = true;
                  }
                  if (controlTarget == "Height Control") {
                    heightControl = controlObject;
                    res = true;
                  }
                  if (controlTarget == "Start Control") {
                    startButton = controlObject;
                    res = true;
                  }
                  if (controlTarget == "Emergency Control") {
                    emergencyButton = controlObject;
                    res = true;
                  }
                }
              }

              if (!res) {
                cerr << "Erronous joystick configuration line: " << *it << endl;
                break;
              }
            }

            if (res) {
              cout << "Using " << (forwardControl == NULL ? "None" : forwardControl->getName()) << " as forward control" << endl;
              cout << "Using " << (strafeControl == NULL ? "None" : strafeControl->getName()) << " as strafe control" << endl;
              cout << "Using " << (spinControl == NULL ? "None" : spinControl->getName()) << " as spin control" << endl;
              cout << "Using " << (heightControl == NULL ? "None" : heightControl->getName()) << " as height control" << endl;
              cout << "Using " << (startButton == NULL ? "None" : startButton->getName()) << " as start button" << endl;
              cout << "Using " << (emergencyButton == NULL ? "None" : emergencyButton->getName()) << " as emergency button" << endl;
            }
          }
        }
      }
    }

    settingsFile.close();
  }

  return res;
}

ParrotJoystick :: ParrotJoystick(int joystickID) {
  joystick = NULL;
  joystick = SDL_JoystickOpen(joystickID);
  if (joystick == NULL) {
    throw string("Could not open joystick");
  }
  name = SDL_JoystickName(joystickID);

  cout << "Opened joystick " << name << endl;
  cout << "Joystick has:" << endl;
  cout << "  " << SDL_JoystickNumAxes(joystick) << " axes" << endl;
  cout << "  " << SDL_JoystickNumBalls(joystick) << " trackballs" << endl;
  cout << "  " << SDL_JoystickNumHats(joystick) << " hats" << endl;
  cout << "  " << SDL_JoystickNumButtons(joystick) << " buttons" << endl;

  if ((SDL_JoystickNumAxes(joystick) >= 5) && (SDL_JoystickNumButtons(joystick) >= 2)) {
    cout << " => Joystick can be used with AR drone" << endl;
  } else {
    cout << " => Joystick cannot be used with AR drone" << endl;
    throw string("Joystick has too few axes/buttons");
  }

  // Add axes and buttons
  for (int i = 0; i < SDL_JoystickNumAxes(joystick); i++) {
    shared_ptr<JoyAxis> axis (new JoyAxis(joystick, i));
    axes.push_back(axis);
    joystickObjects.push_back(axis);
  }

  for (int i = 0; i < SDL_JoystickNumButtons(joystick); i++) {
    shared_ptr<JoyButton> axis (new JoyButton(joystick, i));
    buttons.push_back(axis);
    joystickObjects.push_back(axis);
  }
}

ParrotJoystick :: ~ParrotJoystick() {
  if (joystick != NULL) {
    SDL_JoystickClose(joystick);
  }
}
