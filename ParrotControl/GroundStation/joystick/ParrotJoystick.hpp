#ifndef PARROTJOYSTICK_HPP_
#define PARROTJOYSTICK_HPP_

#include <string>
#include <vector>

#include <SDL/SDL.h>

#include <boost/shared_ptr.hpp>

#include "ParrotController.hpp"
#include "JoyObjects.hpp"

typedef std::vector< boost::shared_ptr<JoyObject> > JoyObjectVector;

class ParrotJoystick : public ParrotController {
protected:
  std::string name;
  SDL_Joystick* joystick;

  JoyObjectVector joystickObjects;
  std::vector< boost::shared_ptr<JoyAxis> > axes;
  std::vector< boost::shared_ptr<JoyButton> > buttons;

  boost::shared_ptr<JoyObject> heightControl, spinControl, strafeControl, forwardControl, emergencyButton, startButton, optional1, optional2;

  virtual void initMostTravelCheck();
  virtual boost::shared_ptr<JoyObject> getMostTraveled();

  virtual boost::shared_ptr<JoyObject> selectControl(std::string text);

  virtual boost::shared_ptr<JoyObject> findControl(const std::string& name);
public:
  virtual void update();

  virtual void calibrateAndAssociate();

  virtual float getHeightSetting() const;
  virtual float getSpinSetting() const;
  virtual float getForwardSetting() const;
  virtual float getStrafeSetting() const;
  virtual bool doStart() const;
  virtual bool isEmergency() const;

  virtual bool optionalButton1() const;
  virtual bool optionalButton2() const;

  virtual bool saveSettings(const std::string& fileName);
  virtual bool loadSettings(const std::string& fileName);

  ParrotJoystick(int joystickID);
  virtual ~ParrotJoystick();
};

#endif /* PARROTJOYSTICK_HPP_ */
