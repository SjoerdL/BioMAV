package nl.ru.ai.projects.parrot.dronecontrol.groundstation;

import java.nio.ByteBuffer;

import sun.reflect.generics.reflectiveObjects.NotImplementedException;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.SensorDataObserver;

public class GroundStation implements ParrotDroneInterface {
  static {
    System.loadLibrary("GroundStation");
  }
  
  private static GroundStation instance = null;
  
  private static native void createInstance0();
  
  public synchronized static GroundStation getInstance() {
    if (instance == null) {
      instance = new GroundStation();
    }
    return instance;
  }
  
  private static final int FLYINGSTATE_LANDED = 0;
  private static final int FLYINGSTATE_FLYING = 1;
  private static final int FLYINGSTATE_EMERGENCY = 2;
  private static final int FLYINGSTATE_DISCONNECTED = 3;
  
  private native ByteBuffer getFrontCameraImage0();
  private native long getFrontCameraTimeStamp0();
  private native double[] getRotationVector0();
  private native long getSensoryTimeStamp0();
  private native void clearEmergencyFlag0();
  private native void flyForward0(double speed);
  private native void flySideways0(double speed);
  private native int getFlyingState0();
  private native void hover0();
  private native void land0();
  private native void takeoff0();
  private native void setEmergencyFlag0();
  private native void setHeight0(double speed);
  private native void spin0(double speed);
  
  private GroundStation() {
    createInstance0();
  }

  @Override
  public byte[] getFrontCameraImage() {
    ByteBuffer buffer = getFrontCameraImage0();
    return buffer.array();
  }

  @Override
  public long getFrontCameraTimeStamp() {
    return getFrontCameraTimeStamp0();
  }

  @Override
  public byte[] getGroundCameraImage() {
    throw new NotImplementedException();
  }

  @Override
  public long getGroundCameraTimeStamp() {
    throw new NotImplementedException();
  }

  @Override
  public double[] getCurrentRotationVector() {
    return getRotationVector0();
  }

  @Override
  public long getLastSensoryDataTimeStamp() {
    return getSensoryTimeStamp0();
  }

  @Override
  public void clearEmergencyFlag() {
    clearEmergencyFlag0();
  }

  @Override
  public void flyForward(double speed) {
    flyForward0(Math.min(1, Math.max(-1, speed)));
  }

  @Override
  public void flySideways(double speed) {
    flySideways0(Math.min(1, Math.max(-1, speed)));
  }

  @Override
  public FlyingState getFlyingState() {
    switch (getFlyingState0()) {
      case FLYINGSTATE_DISCONNECTED:
        return FlyingState.DISCONNECTED;
      case FLYINGSTATE_FLYING:
        return FlyingState.FLYING;
      case FLYINGSTATE_EMERGENCY:
        return FlyingState.EMERGENCY;
      case FLYINGSTATE_LANDED:
        return FlyingState.LANDED;
      default:
        throw new Error("Native interface returned erronous state");
    }
  }

  @Override
  public void hover() {
    hover0();
  }

  @Override
  public void land() {
    land0();
  }

  @Override
  public void setEmergencyFlag() {
    setEmergencyFlag0();
  }

  @Override
  public void setHeight(double height) {
    setHeight0(Math.min(1, Math.max(0, height)));
  }

  @Override
  public void spin(double speed) {
    spin0(Math.min(1, Math.max(-1, speed)));
  }

  @Override
  public void takeoff() {
    takeoff0();
  }

  @Override
  public void droneSleep(long arg0) throws InterruptedException {
    throw new NotImplementedException();
  }

  @Override
  public double[] getGroundSpeed() {
    throw new NotImplementedException();
  }

  @Override
  public void addSensorDataObserver(SensorDataObserver arg0) {
    throw new NotImplementedException();
  }
}
