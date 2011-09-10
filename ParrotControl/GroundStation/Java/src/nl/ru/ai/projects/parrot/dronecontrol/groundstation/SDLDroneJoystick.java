package nl.ru.ai.projects.parrot.dronecontrol.groundstation;

public class SDLDroneJoystick implements ExtDroneInputDevice {
  static {
    System.loadLibrary("GroundStation");
  }
  
  private static native boolean initJoystick0(int i);
  private static native boolean getOptionButton0(int i);
  private static native boolean getStartMode0();
  private static native boolean getEmergencyMode0();
  private static native float getPitch0();
  private static native float getYaw0();
  private static native float getRoll0();
  private static native float getHeight0();
  private static native void shutdownJoystick0();

  private static SDLDroneJoystick instance = null;
  
  public synchronized static SDLDroneJoystick getInstance() {
    if (instance == null) {
      if (initJoystick0(0)) {
        instance = new SDLDroneJoystick();
      }
    }
    return instance;
  }
  
  private boolean destroyed = false; 

  private SDLDroneJoystick() {
    Runtime.getRuntime().addShutdownHook(new Thread() {
      @Override
      public void run() {
        shutdown();
      }
    });
  }
  
  private synchronized void shutdown() {
    if (destroyed) {
      return;
    }
    destroyed = true;
    
    System.out.println("Invoking joystick destruction");
    shutdownJoystick0();
    System.out.println("Joystick destroyed");
  }
  
  @Override
  public synchronized boolean getOptionButton(int i) {
    if (destroyed) {
      return false;
    }
    
    return getOptionButton0(i);
  }

  @Override
  public synchronized float getPitch() {
    if (destroyed) {
      return 0;
    }
    
    return getPitch0();
  }

  @Override
  public synchronized float getYaw() {
    if (destroyed) {
      return 0;
    }

    return getYaw0();
  }

  @Override
  public float getRoll() {
    if (destroyed) {
      return 0;
    }

    return getRoll0();
  }

  @Override
  public synchronized float getHeight() {
    if (destroyed) {
      return 0;
    }
    
    return getHeight0();
  }

  @Override
  public synchronized boolean getStartMode() {
    if (destroyed) {
      return false;
    }
    
    return getStartMode0();
  }

  @Override
  public synchronized boolean getEmergencyMode() {
    if (destroyed) {
      return false;
    }

    return getEmergencyMode0();
  }
}
