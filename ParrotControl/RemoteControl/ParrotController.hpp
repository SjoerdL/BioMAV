#ifndef PARROTCONTROLLER_HPP_
#define PARROTCONTROLLER_HPP_

class ParrotController {
public:
  virtual float getHeightSetting() const = 0;
  virtual float getSpinSetting() const = 0;
  virtual float getForwardSetting() const = 0;
  virtual float getStrafeSetting() const = 0;
  virtual bool doStart() const = 0;
  virtual bool isEmergency() const = 0;
};

#endif /* PARROTCONTROLLER_HPP_ */
