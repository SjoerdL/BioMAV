#ifndef JOYOBJECTS_HPP_
#define JOYOBJECTS_HPP_

#include <SDL.h>

#include <string>

class JoyObject {
private:
  bool firstPosition;
  float lastPosition;
  float travelCounter;
public:
  virtual void update();

  virtual void resetTravelCounter();
  virtual float getTravelCounter();

  virtual void markCenter() = 0;
  virtual void markMaximum() = 0;
  virtual float getPosition() = 0;

  virtual std::string getName() = 0;

  virtual std::string serializeSettings() = 0;
  virtual bool loadSettings(const std::string& settings) = 0;

  JoyObject& operator=(const JoyObject& other);

  JoyObject();
};

class JoyAxis : public JoyObject {
protected:
  SDL_Joystick* joystick;
  int axis;

  int min, max, center;
  bool invert;
  int lastValue;
public:
  virtual void update();

  virtual void markCenter();
  virtual void markMaximum();
  virtual float getPosition();
  virtual std::string getName();

  virtual std::string serializeSettings();
  virtual bool loadSettings(const std::string& settings);

  JoyAxis& operator= (const JoyAxis& other);

  JoyAxis(SDL_Joystick* joystick, int axis);
  virtual ~JoyAxis();
};

class JoyButton : public JoyObject {
protected:
  SDL_Joystick* joystick;
  int button;

  bool lastValue, invert;
public:
  virtual void update();

  virtual void markCenter();
  virtual void markMaximum();
  virtual float getPosition();
  virtual std::string getName();

  virtual std::string serializeSettings();
  virtual bool loadSettings(const std::string& settings);

  JoyButton& operator= (const JoyButton& other);

  JoyButton(SDL_Joystick* joystick, int button);
  virtual ~JoyButton();
};

#endif /* JOYOBJECTS_HPP_ */
