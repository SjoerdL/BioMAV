#ifndef VERTICALACTIVATIONCOUNTER_HPP_
#define VERTICALACTIVATIONCOUNTER_HPP_

class VerticalActivationCounter {
  private:
    int width, height;
    float* buffer;
    
    int startX;
    int direction;
    
  public:
    float calculateActivation();
    
    VerticalActivationCounter& operator=(const VerticalActivationCounter& other);
  
    VerticalActivationCounter(float* buffer, int width, int height, int startX, int direction);
    virtual ~VerticalActivationCounter();
};

#endif // VERTICALACTIVATIONCOUNTER_HPP_
