#ifndef SIMPLEPOLEPOSITIONDETECTOR_HPP_
#define SIMPLEPOLEPOSITIONDETECTOR_HPP_

#define POLE_DETECTION_THRESHOLD 1.65

class PolePositionDetector {
  private:
    int width;
    bool visible;
    float angle;
    
    float mean, stddev;
  public:
    void processData(float* activations);
    
    bool isVisible();
    float getAngle();

    float getMean();
    float getStdDev();
    
    PolePositionDetector(int width);
    virtual ~PolePositionDetector();
};

#endif // SIMPLEPOLEPOSITIONDETECTOR_HPP_

