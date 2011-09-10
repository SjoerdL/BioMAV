package nl.ru.ai.projects.parrot.dronecontrol.simulator;

import java.util.HashSet;
import java.util.Set;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.dronecontrol.SensorDataObserver;

public class SimulatedDrone implements ParrotDroneInterface, PolePositionPollInterface {
  static {
    System.loadLibrary("ParrotSimulator");
  }
  
  private int droneRefID = -1;
  
  private Simulator simulator = null;
  private Set<SensorDataObserver> sensorDataObservers = new HashSet<SensorDataObserver>();
  
  private native Object[] runLuaCode0(String code);
  
  private byte[] getImage(int i) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return simulator.getImage(droneRefID, i);
      }
    }
    return null;
  }
  
  private boolean isPoleVisible0(int i) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return (Boolean) runLuaCode0("return drone:isPoleVisible(" + i + ");")[0];
      }
    }
    return false;
  }
  
  private int getPoleCount0() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return (int) ((double) (Double) runLuaCode0("return drone:getPoleCount();")[0]);
      }
    }
    return 0;
  }
  
  private Double getPoleAngle0(int i) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return (Double) runLuaCode0("return drone:getPoleAngle(" + i + ");")[0];
      }
    }
    return null;
  }

  void notifyNewSensorData() {
    synchronized (sensorDataObservers) {
      for (SensorDataObserver dataObserver : sensorDataObservers) {
        dataObserver.newSensorData(this);
      }
    }
  }
  
  private SimulatedDrone(Simulator sim, int droneRefID) {
    this.droneRefID = droneRefID;
    simulator = sim;
  }
  
  public synchronized Object[] runScript(String code) {
    return runLuaCode0(code);
  }
                
  @Override
  public void clearEmergencyFlag() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:setEmergency(false);");
      }
    }
  }

  @Override
  public void flyForward(double speed) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:flyForward(" + speed + ");");
      }
    }
  }

  @Override
  public void flySideways(double speed) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:flySideways(" + speed + ");");
      }
    }
  }

  @Override
  public void setHeight(double height) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:setHeight(" + height + ");");
      }
    }
  }

  @Override
  public FlyingState getFlyingState() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        int state = (Integer) runLuaCode0("return drone:getFlyingState();")[0];
        if (state == 0) {
          return FlyingState.DISCONNECTED;
        }
        if (state == 1) {
          return FlyingState.EMERGENCY;
        }
        if (state == 2) {
          return FlyingState.LANDED;
        }
        if (state == 3) {
          return FlyingState.FLYING;
        }
        throw new RuntimeException("Unsuspected result from lua function: " + state);
      }
    }
    return FlyingState.EMERGENCY;
  }

  @Override
  public void hover() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:hover();");
      }
    }
  }

  @Override
  public void land() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:land();");
      }
    }
  }

  @Override
  public void setEmergencyFlag() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:setEmergency(true);");
      }
    }
  }

  @Override
  public void spin(double speed) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:spin(" + speed + ");");
      }
    }
  }

  @Override
  public void takeoff() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:takeoff();");
      }
    }
  }

  @Override
  public long getPolePositionTimeStamp() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return simulator.getLastRenderTimeStamp(); // TODO: (?) Perhaps kind of dirty... render is coupled to the simulation cycles but well...
      }
    }
    return -1;
  }

  @Override
  public Double[] getPolePositions() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        Double[] result = new Double[getPoleCount0()];
    
        for (int i = 0; i < result.length; i++) {
          if (isPoleVisible0(i)) {
            result[i] = getPoleAngle0(i);
          } else {
            result[i] = null;
          }
        }
  
        return result;
      }
    }
    return null;
  }

  @Override
  public Double[] getPoleDistances() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        Double[] result = new Double[getPoleCount0()];

        for (int i = 0; i < result.length; i++) {
          if (isPoleVisible0(i)) {
            result[i] = (Double) runLuaCode0("return drone:getPoleDistance(" + i + ");")[0];
          } else {
            result[i] = null;
          }
        }
  
        return result;
      }
    }
    return null;
  }

  @Override
  public byte[] getFrontCameraImage() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return getImage(0);
      }
    }
    return null;
  }

  @Override
  public long getFrontCameraTimeStamp() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return simulator.getLastRenderTimeStamp();
      }
    }
    return -1;
  }

  @Override
  public byte[] getGroundCameraImage() {
    return getImage(1);
  }

  @Override
  public long getGroundCameraTimeStamp() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return simulator.getLastRenderTimeStamp();
      }
    }
    return -1;
  }
  
  @Override
  public long getLastSensoryDataTimeStamp() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        return simulator.getCurrentTimeStamp();
      }
    }
    return -1;
  }
  
  @Override
  public double[] getCurrentRotationVector() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        Object[] da = (Object[]) runLuaCode0("return drone:getRotationSensorData();");
        double[] rawDa = new double[3];
        rawDa[0] = (Double) da[0];
        rawDa[1] = (Double) da[1];
        rawDa[2] = (Double) da[2];
        return rawDa;
      }
    }
    return null;
  }
  
  @Override
  public double[] getGroundSpeed() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        Object[] da = (Object[]) runLuaCode0("return drone:getGroundSpeed();");
        double[] rawDa = new double[3];
        rawDa[0] = (Double) da[0];
        rawDa[1] = (Double) da[1];
        rawDa[2] = (Double) da[2];
        return rawDa;
      }
    }
    return null;
  }

  @Override
  public void addSensorDataObserver(SensorDataObserver observer) {
    synchronized (sensorDataObservers) {
      if (!sensorDataObservers.contains(observer)) {
        sensorDataObservers.add(observer);
      }
    }
  }

  @Override
  public void droneSleep(long milliseconds) throws InterruptedException {
    simulator.simSleep(milliseconds);
  }
  
  public double[] getDronePosition() {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        Object[] da = (Object[]) runLuaCode0("return drone:javaGetPosition();");
        double[] rawDa = new double[3];
        rawDa[0] = (Double) da[0];
        rawDa[1] = (Double) da[1];
        rawDa[2] = (Double) da[2];
        return rawDa;
      }
    }
    return null;
  }
  
  public void setDronePosition(double x, double y, double z) {
    synchronized (simulator) {
      if (simulator.isInitialized()) {
        runLuaCode0("drone:setPosition(Vector:new(" + x + ", " + y + ", " + z + "));");
      }
    }
  }
}
