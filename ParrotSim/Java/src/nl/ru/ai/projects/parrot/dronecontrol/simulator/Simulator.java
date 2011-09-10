package nl.ru.ai.projects.parrot.dronecontrol.simulator;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import java.nio.ByteBuffer;

public class Simulator {
  private static Simulator instance = null;
  
  private static native boolean initSimulator(Simulator simObj);
  private static native boolean runSimulator0(Simulator simObj);
  private static native boolean waitReady0();
  
  private native static void cleanupSimulator();
  
  private native static void setCoupleTime0(boolean b);
  private native static boolean getCoupleTime0();

  private native static long getLastRenderTimeStamp0();
  private native static long getCurrentTimeStamp0();
  
  private native static Object[] runLuaCode0(String code);
  
  private native static void activateVideoInterface0(boolean b);
  private native static void activateRendering0(boolean b);
  private native static boolean isRenderingActivated0();
  
  private native static ByteBuffer getImage0(int droneIndex, int imageIndex);

  private native static SimulatedDrone[] getDrones0();
  
  static {
    System.loadLibrary("ParrotSimulator");
  }
    
  public static void runSimulator() {
    synchronized (Simulator.class) {
      if (instance == null) {
        instance = new Simulator();
      } else {
        return;
      }
    }

    Runtime.getRuntime().addShutdownHook(new Thread() {
      @Override
      public void run() {
        instance.cleanup();
      }
    });
    runSimulator0(instance);
  }
  
  public static synchronized boolean waitReady() {
    return waitReady0();
  }
  
  public synchronized static Simulator getInstance() {
    if (instance == null) {
      Simulator tmpInstance = new Simulator();
      if (initSimulator(tmpInstance)) {
        instance = tmpInstance;
        Runtime.getRuntime().addShutdownHook(new Thread() {
          @Override
          public void run() {
            instance.cleanup();
          }
        });
      }
    } else {
      if (!waitReady()) {
        return null;
      }
    }
    return instance;
  }
  
  
  private boolean videoActivated = true;
  private boolean destroyed = false;
  private ExternalTimerSynchronizer externalTimerSynchronizer = new ExternalTimerSynchronizer();

  private List<SimulatedDrone> drones = new ArrayList<SimulatedDrone>();
  
  private Simulator() {
  }
  
  private synchronized void updateDroneArray() {
    if (destroyed) {
      return;
    }
    
    drones.clear();
    drones.addAll(Arrays.asList(getDrones0()));
  }
  
  private void cleanup() {
    synchronized (this) {
      if (destroyed) {
        return;
      }
      
      destroyed = true;
    }

    cleanupSimulator();
  }
  
  /**
   * Checks whether the cleanup function of this object was not yet called
   * 
   * @return
   *   True if the the cleanup function was not yet called and therefore the 
   *   library is still instantiated 
   */
  public synchronized boolean isInitialized() {
    return !destroyed;
  }
  
  /**
   * Gets the image imageIndex rendered by drone droneIndex.
   * 
   * @param droneIndex
   *   Index identifying the drone the image should be obtained from
   * @param imageIndex
   *   Index of the image that should be obtained from the given drone.
   * @return
   *   null if the image cannot be retrieved from the given drone.
   *   Otherwise the result is a byte array containing rowwise rgb-data 
   *   of the rendered image.
   */
  public synchronized byte[] getImage(int droneIndex, int imageIndex) {
    if (destroyed) {
      return null;
    }

    ByteBuffer imageBuffer = getImage0(droneIndex, imageIndex);
    if (imageBuffer == null) {
      return null;
    }
    byte[] result = new byte[imageBuffer.capacity()];
    imageBuffer.rewind();
    imageBuffer.get(result);
    return result;
  }
  

  /**
   * This function allows to run arbitrary lua-code in the simulator environment and return 
   * values from it using Java types
   * 
   * @param code
   *   Code to be run
   * @return
   *   results returned by teh code snippet
   */
  public synchronized Object[] runLuaCode(String code) {
    if (destroyed) {
      return null;
    }
    
    return runLuaCode0(code);
  }
  
  /**
   * Sets whether the simulation speed should be throttled so that
   * the simulation time never runs faster than the real time (system time).
   * 
   * @param b
   */
  public synchronized void setCoupleTime(boolean b) {
    if (destroyed) {
      return;
    }
    
    setCoupleTime0(b);
  }
  
  /**
   * Checks whether the simulation time is coupled to the system time
   * 
   * @return
   */
  public synchronized boolean getCoupleTime() {
    if (destroyed) {
      return false;
    }
    
    return getCoupleTime0();
  }
  
  /**
   * Returns the time stamp of the last simulation step the simulator
   * has done.
   * 
   * @return
   *   Time stamp in milliseconds
   */
  public synchronized long getCurrentTimeStamp() {
    if (destroyed) {
      return -1;
    }
    
    return getCurrentTimeStamp0();
  }
  
  /**
   * Returns the time stamp of the last rendering done by the simulator.
   * 
   * @return
   *   Time stamp in milliseconds
   */
  public synchronized long getLastRenderTimeStamp() {
    if (destroyed) {
      return -1;
    }
    
    return getLastRenderTimeStamp0();
  }
  
  /**
   * Returns how many drones are in the simulation
   * @return
   */
  public synchronized int getDroneCount() {
    if (destroyed) {
      return 0;
    }
    
    updateDroneArray();
    
    return drones.size();
  }
  
  /**
   * Returns the drone with index i. For i it must hold that: 
   * 0 <= i < getDroneCount()
   * @param i
   * @return
   */
  public synchronized SimulatedDrone getDrone(int i) {
    if (destroyed) {
      return null;
    }
    updateDroneArray();
    
    return drones.get(i);
  }
  
  /**
   * Enables or disables the rendering of camera images of the drone. Disabling
   * the rendering results in a performance increase for the simulation. However,
   * disabling this feature makes drone functions that return camera images from
   * the simulated drone useless (because the camera images are not rendered).
   * 
   * To enable the video interface the simulator library needs to be
   * compiled with OpenGL/SDL support. Furthermore, at the moment, the
   * functionality also depends on the activation of the rendedering of the 
   * simulation (-> activateRendering) [this will be changed in a future
   * version however so that the camera images can also be rendered without
   * showing the rendered simulation]. 
   * 
   * @param b
   */
  public synchronized void activateVideoInterface(boolean b) {
    if (destroyed) {
      return;
    }
    
    videoActivated = b;
    activateVideoInterface0(b);
  }
  
  /**
   * Checks whether the rendering of camera images of the drone is enabled. 
   * 
   * @return
   */
  public synchronized boolean isVideoActivated() {
    if (destroyed) {
      return false;
    }
    
    return videoActivated && isRenderingActivated();
  }

  /**
   * Enables or disables OpenGL-rendering through SDL. If disabled, no
   * visualization of the simulated drone will be rendered. If true and if
   * the simulator library has been compiled with SDL report, the default SDL 
   * window will be shown and a visualization of the drone will be shown.
   * 
   * However, deactivating the rendering of the simulation can yield a major
   * simulation speed boost.
   * 
   * @param b
   */
  public synchronized void activateRendering(boolean b) {
    if (destroyed) {
      return;
    }
    
    activateRendering0(b);
  }
 
  /**
   * Checks whether the render engine is running. If an error occurs 
   * during the SDL or OpenGL initialization the value will be reset to
   * false even if true after calling activateRendering(true).
   * 
   * This is because the render engine is initialized asynchronously in another
   * thread. There is no possibility yet to check whether the 
   * render engine has been correctly initialized other than checking if this
   * flag is set to false again.
   * 
   * @return
   */
  public synchronized boolean isRenderingActivated() {
    if (destroyed) {
      return false;
    }
    
    return isRenderingActivated0();
  }

  /**
   * Performs a sleep that is synchronized with the simulation timer. 
   * 
   * @param milliseconds
   *   Milliseconds to wait
   * @throws InterruptedException
   */
  public void simSleep(long milliseconds) throws InterruptedException {
    externalTimerSynchronizer.sleep(milliseconds);
  }
}
