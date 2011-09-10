package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.SensorDataObserver;
import nl.ru.ai.projects.parrot.dronecontrol.SensoryDataInterface;

import java.io.IOException;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import nl.ru.ai.projects.parrot.dronecontrol.ControlInterface;

public class DroneGroundStation implements ParrotDroneInterface, NavdataChannelObserver, Runnable {
  public static final int MIN_HEIGHT = 200;
  public static final int MAX_HEIGHT = 2000;
  public static final int MAX_HEIGHT_DIFFERENCE = 200;
  
  private ATControlCommandInterface controlCommandInterface = null;
  private DroneConfigurationChannel droneConfigurationChannel = null;
  private NavdataChannel navdataChannel = null;
  private VisionStandin vision = null;
  
  private float[] lastRotationVector = null;
  private double[] integratedRotationVector = {0, 0, 0};
  private double[] speedVector = {0, 0, 0};

  private Thread controlAbstractionThread = null;
  private boolean doHover = true;
  private double setHeight = 0.5;
  private double setPitch = 0, setRoll = 0, setYaw = 0;
  
  private boolean doStart = false, setEmergency = true;
  
  public DroneGroundStation() throws SocketException {
    controlCommandInterface = new ATControlCommandInterface();
    droneConfigurationChannel = new DroneConfigurationChannel(controlCommandInterface);
    vision = new VisionStandin();
    navdataChannel = new NavdataChannel(droneConfigurationChannel);
    navdataChannel.addNavdataChannelObserver(this);
  }
  
  @Override
  public void run() {
    boolean lDoHover, lDoStart, lEmergency;
    double lHeight;
    double lPitch, lRoll, lYaw;
    double currentHeight;
    
    double startHeight = 0;
    
    long lastEmergencyStrobe = -1;
    long lastNavdataTimeStamp = navdataChannel.getLastNavdataTimestamp();
    long lastNewNavdataTimeStamp = System.currentTimeMillis();
    
    controlCommandInterface.land();

    while (!Thread.currentThread().isInterrupted()) {
      synchronized (this) {
        lDoStart = doStart;
        lEmergency = setEmergency;
      }
      if (lEmergency != (getFlyingState() == FlyingState.EMERGENCY)) {
        if (System.currentTimeMillis() - lastEmergencyStrobe > 250) {
          controlCommandInterface.strobeEmergency();
          lastEmergencyStrobe = System.currentTimeMillis();
        }
      } else {
        if (getFlyingState() == FlyingState.LANDED) {
          if (lDoStart) {
            controlCommandInterface.takeoff();
          }
        }
        if (getFlyingState() == FlyingState.FLYING) {
          if (!lDoStart) {
            controlCommandInterface.land();
          } 
          synchronized (this) {
            lDoHover = doHover;
            lHeight = setHeight * (MAX_HEIGHT - MIN_HEIGHT) + MIN_HEIGHT;
            lPitch = setPitch;
            lRoll = setRoll;
            lYaw = setYaw;
            
            if (navdataChannel.getLastNavdataTimestamp() > lastNavdataTimeStamp) {
              lastNavdataTimeStamp = navdataChannel.getLastNavdataTimestamp();
              lastNewNavdataTimeStamp = System.currentTimeMillis();
            }

            currentHeight = lHeight;
            if ((navdataChannel.getLastDemoData() != null) && (System.currentTimeMillis() - lastNewNavdataTimeStamp < 500)) {
              currentHeight = navdataChannel.getLastDemoData().altitude;
            }
          }
          
          if (Math.abs(currentHeight - lHeight) > MAX_HEIGHT_DIFFERENCE) {
            if ((Math.signum(startHeight - lHeight) != Math.signum(currentHeight - lHeight)) || (Math.abs(startHeight - lHeight) < MAX_HEIGHT_DIFFERENCE)) {
              startHeight = currentHeight;
            }
          }
          if (Math.abs(startHeight - lHeight) > MAX_HEIGHT_DIFFERENCE) {
            double upSpeed = Math.max(0.05, Math.min(1, 1.0 * Math.abs(lHeight - currentHeight) / 1000.0));
            if (startHeight > lHeight) {
              upSpeed = -upSpeed;
              if (currentHeight < lHeight) {
                startHeight = lHeight;
              }
            } else {
              if (currentHeight > lHeight) {
                startHeight = lHeight;
              }
            }
            
            if (lDoHover) {
              controlCommandInterface.flyForward(0);
              controlCommandInterface.flySideways(0);
              controlCommandInterface.spin(0);
            } else {
              controlCommandInterface.flyForward(lPitch);
              controlCommandInterface.flySideways(lRoll);
              controlCommandInterface.spin(lYaw);
            }
            controlCommandInterface.setUpSpeed(upSpeed);
          } else {
            if (lDoHover) {
              controlCommandInterface.hover();
            } else {
              controlCommandInterface.flyForward(lPitch);
              controlCommandInterface.flySideways(lRoll);
              controlCommandInterface.spin(lYaw);
              controlCommandInterface.setUpSpeed(0);
            }
          }
        }
      }
      
      try {
        Thread.sleep(10);
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }
  }

  public void connect(String remoteAddress) {
    disconnect();
    
    try {
      InetAddress remoteInetAddress = InetAddress.getByName(remoteAddress);
      controlCommandInterface.connect(remoteInetAddress);
      try {
        navdataChannel.connect(remoteInetAddress);
        
        try {
          vision.connect(remoteInetAddress, this);
        }
        catch (IOException e) {
          navdataChannel.disconnect();
          throw e;
        }

        controlAbstractionThread = new Thread(this);
        controlAbstractionThread.start();
      } catch (IOException e) {
        controlCommandInterface.disconnect();
        throw e;
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
  
  public void disconnect() {
    if (controlAbstractionThread != null) {
      controlAbstractionThread.interrupt();
      try {
        controlAbstractionThread.join();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      controlAbstractionThread = null;
    }
    
    vision.disconnect();
    navdataChannel.disconnect();
    controlCommandInterface.disconnect();
    
    lastRotationVector = null;
    
    integratedRotationVector[0] = 0;
    integratedRotationVector[1] = 0;
    integratedRotationVector[2] = 0;
    
    speedVector[0] = 0;
    speedVector[1] = 0;
    speedVector[2] = 0;
  }
  
  @Override
  public synchronized FlyingState getFlyingState() {
    if (navdataChannel.getLastNavdataTimestamp() < 0) {
      return FlyingState.DISCONNECTED;
    }
    if ((navdataChannel.getLastDroneStatus() & NavdataPacket.ARDRONE_VBAT_LOW) != 0) {
      return FlyingState.BATTERY_LOW;
    }
    if ((navdataChannel.getLastDroneStatus() & NavdataPacket.ARDRONE_EMERGENCY_MASK) != 0) {
      return FlyingState.EMERGENCY;
    }
    if ((navdataChannel.getLastDroneStatus() & NavdataPacket.ARDRONE_FLY_MASK) != 0) {
      return FlyingState.FLYING;
    }
    return FlyingState.LANDED;
  }

  @Override
  public synchronized void takeoff() {
    doStart = true;
  }

  @Override
  public synchronized void land() {
    doStart = false;
  }

  @Override
  public synchronized void setEmergencyFlag() {
    setEmergency = true;
    land();
  }

  @Override
  public synchronized void clearEmergencyFlag() {
    setEmergency = false;
  }

  @Override
  public synchronized void flyForward(double speed) {
    doHover = false;
    setPitch = speed;
  }

  @Override
  public synchronized void flySideways(double speed) {
    doHover = false;
    setRoll = speed;
  }

  @Override
  public synchronized void setHeight(double height) {
    setHeight = height;
  }

  @Override
  public synchronized void spin(double speed) {
    doHover = false;
    setYaw = speed;
  }

  @Override
  public synchronized void hover() {
    doHover = true;
    setPitch = 0;
    setRoll = 0;
    setYaw = 0;
  }

  @Override
  public synchronized long getLastSensoryDataTimeStamp() {
    return navdataChannel.getLastNavdataTimestamp();
  }

  @Override
  public synchronized double[] getCurrentRotationVector() {
    double[] result = integratedRotationVector;
    integratedRotationVector = new double[3];
    return result;
  }

  @Override
  public synchronized void newNavdata(NavdataChannel channel) {
    if (channel.getLastDemoData() != null) {
      float[] sVector = channel.getLastDemoData().velocity;
      speedVector[0] = sVector[0];
      speedVector[1] = sVector[1];
      speedVector[2] = sVector[2];
//      System.out.println(speedVector[0] + " " + speedVector[1] + " " + speedVector[2]);

      float[] rotationVector = channel.getLastDemoData().euler;
      
      if (lastRotationVector != null) {
        double[] rotationDelta = {0, 0, 0};
        for (int i = 0; i < 3; i++) {
          rotationDelta[i] = (rotationVector[i] - lastRotationVector[i]) / 1000.0;
          if (rotationDelta[i] < -270) {
            rotationDelta[i] += 360;
          }
          if (rotationDelta[i] > 270) {
            rotationDelta[i] -= 360;
          }
        }
        
        integratedRotationVector[0] += rotationDelta[0] / 180 * Math.PI;
        integratedRotationVector[1] += rotationDelta[1] / 180 * Math.PI;
        integratedRotationVector[2] += rotationDelta[2] / 180 * Math.PI;
      }
      
      lastRotationVector = rotationVector;
    }
  }

  @Override
  public double[] getGroundSpeed() {
    return speedVector.clone();
  }

  @Override
  public void droneSleep(long milliseconds) throws InterruptedException {
    Thread.sleep(milliseconds);
  }

  @Override
  public synchronized long getFrontCameraTimeStamp() {
    return vision.getTimeStamp();
  }

  @Override
  public byte[] getFrontCameraImage() {
    return vision.getImageData();
  }

  @Override
  public long getGroundCameraTimeStamp() {
    throw new Error("Not implemented!");
  }

  @Override
  public byte[] getGroundCameraImage() {
    throw new Error("Not implemented!");
  }

  @Override
  public void addSensorDataObserver(SensorDataObserver observer) {
    throw new Error("Not implemented!");
  }
}
