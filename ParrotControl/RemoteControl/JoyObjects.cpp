#include "JoyObjects.hpp"

#include <iostream>
#include <sstream>

#include <cmath>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

using namespace std;

void JoyObject :: update() {
  if (firstPosition) {
    firstPosition = false;
  } else {
    travelCounter += fabs(lastPosition - getPosition());
  }
  lastPosition = getPosition();
}

void JoyObject :: resetTravelCounter() {
  travelCounter = 0;
  lastPosition = getPosition();
}

float JoyObject :: getTravelCounter() {
  return travelCounter;
}

JoyObject& JoyObject :: operator=(const JoyObject& other) {
  this->firstPosition = other.firstPosition;
  this->lastPosition = other.lastPosition;
  this->travelCounter = other.travelCounter;

  return *this;
}

JoyObject :: JoyObject() {
  travelCounter = 0;
  firstPosition = true;
}


void JoyAxis :: update() {
  lastValue = SDL_JoystickGetAxis(joystick, axis);
  min = MIN(lastValue, min);
  max = MAX(lastValue, max);
  center = MAX(min, MIN(center, max));
  JoyObject::update();
}

void JoyAxis :: markCenter() {
  center = lastValue;
}

void JoyAxis :: markMaximum() {
  invert = (abs(lastValue - min) < abs(lastValue - max));
}

float JoyAxis :: getPosition() {
  if (lastValue < center) {
    return (invert ? -1 : 1) * (float) (lastValue - center) / (float) MAX(abs(center - min), 1);
  } else {
    return (invert ? -1 : 1) * (float) (lastValue - center) / (float) MAX(abs(center - max), 1);
  }
}

string JoyAxis :: getName() {
  stringstream res (stringstream::in | stringstream::out);

  res << "Axis " << axis  + 1;

  return string(res.str());
}

JoyAxis& JoyAxis :: operator= (const JoyAxis& other) {
  JoyObject::operator=(other);

  this->axis = other.axis;
  this->center = other.center;
  this->joystick = other.joystick;
  this->lastValue = other.lastValue;
  this->max = other.max;
  this->min = other.min;
  this->invert = other.invert;

  return *this;
}

std::string JoyAxis :: serializeSettings() {
  stringstream res (stringstream::in | stringstream::out);

  res << min << " " << center << " " << max << " " << invert;

  return res.str();
}

bool JoyAxis :: loadSettings(const std::string& settings) {
  stringstream reader (stringstream::in | stringstream::out);

  reader << settings << " ";
  reader >> min;
  reader >> center;
  reader >> max;
  reader >> invert;

  return reader.good() && !reader.fail();
}

JoyAxis :: JoyAxis(SDL_Joystick* joystick, int axis) : JoyObject() {
  min = 0;
  max = 0;
  center = 0;
  lastValue = 0;
  invert = false;

  this->joystick = joystick;
  this->axis = axis;
}

JoyAxis :: ~JoyAxis() {
}


void JoyButton :: update() {
  lastValue = SDL_JoystickGetButton(joystick, button) != 0;

  JoyObject::update();
}

void JoyButton :: markCenter() {
  invert = lastValue;
}

void JoyButton :: markMaximum() {
}

float JoyButton :: getPosition() {
  if ((lastValue && !invert) || (!lastValue && invert)) {
    return 1;
  } else {
    return 0;
  }
}

string JoyButton :: getName() {
  stringstream res (stringstream::in | stringstream::out);

  res << "Button " << button + 1;

  return string(res.str());
}

std::string JoyButton :: serializeSettings() {
  stringstream res (stringstream::in | stringstream::out);

  res << invert;

  return res.str();
}

bool JoyButton :: loadSettings(const std::string& settings) {
  stringstream reader (stringstream::in | stringstream::out);

  reader << settings << " ";
  reader >> invert;

  return reader.good() && !reader.fail();
}

JoyButton& JoyButton :: operator= (const JoyButton& other) {
  JoyObject::operator=(other);

  this->joystick = other.joystick;
  this->button = other.button;
  this->lastValue = other.lastValue;
  this->invert = other.invert;

  return *this;
}

JoyButton :: JoyButton(SDL_Joystick* joystick, int button) : JoyObject() {
  this->joystick = joystick;
  this->button = button;
  invert = false;
}

JoyButton :: ~JoyButton() {
}
