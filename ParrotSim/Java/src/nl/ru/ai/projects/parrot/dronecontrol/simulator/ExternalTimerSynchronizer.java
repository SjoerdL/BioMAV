package nl.ru.ai.projects.parrot.dronecontrol.simulator;

import java.util.HashSet;
import java.util.Set;
import java.util.TreeSet;

public class ExternalTimerSynchronizer {
  private class Delay implements Comparable<Delay> {
    private long endTime;
    
    public Delay(long endTime) {
      this.endTime = endTime;
    }
    
    public synchronized boolean testResumeThread(long time) {
      if (endTime <= time) {
        this.notify();
        return true;
      }
      return false;
    }

    public synchronized void resumeThread() {
      this.notify();
    }

    @Override
    public int compareTo(Delay other) {
      return new Long(endTime).compareTo(other.endTime);
    }
    
    @Override
    public String toString() {
      return "(Delay: " + endTime + ")";
    }
  }
  
  private Long firstTime = new Long(-1);
  private long lastTime = -1;
  private Set<Delay> delayStack = new TreeSet<Delay>();
  private Set<Delay> usedDelays = new HashSet<Delay>();
  
  public ExternalTimerSynchronizer() {
    Runtime.getRuntime().addShutdownHook(new Thread() {
      @Override
      public void run() {
        synchronized (delayStack) {
          for (Delay delay : delayStack.toArray(new Delay[0])) {
            delay.resumeThread();
            delayStack.remove(delay);
          }
        }
      }
    });
  }
  
  public void externalTimerUpdate(long time) {
    synchronized (firstTime) {
      if (firstTime < 0) {
        firstTime = time;
      }
      
      synchronized (delayStack) {
        for (Delay delay : delayStack) {
          if (delay.testResumeThread(time - firstTime)) {
            usedDelays.add(delay);
          } else {
            break;
          }
        }
        
        for (Delay usedDelay : usedDelays) {
          delayStack.remove(usedDelay);
        }
        usedDelays.clear();
      }
      
      lastTime = time - firstTime;
    }
  }
  
  public void sleep(long milliseconds) throws InterruptedException {
    Delay delay = new Delay(Math.max(0, lastTime) + milliseconds);
    synchronized (delay) {
      synchronized (delayStack) {
        delayStack.add(delay);
      }
      delay.wait();
    }
  }
}
