package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.HashSet;
import java.util.Set;

import nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol.NavdataPacket.DemoDataStruct;
import nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol.NavdataPacket.NavdataPacketFormatException;

public class NavdataChannel implements Runnable {
  public static final int NAVDATA_PORT = 5554;
  
  private DroneConfigurationChannel droneConfigChannel;
  
  private Thread navdataThread = null;
  private DatagramSocket navdataSocket = null;
  private InetAddress remoteAddress = null;
  
  private Set<NavdataChannelObserver> observers = new HashSet<NavdataChannelObserver>();
  
  private int lastSequenceNumber = -1;
  private long navdataStartTime = -1;
  private long lastTimeStamp = -1;
  private int droneStatus = 0;
  private DemoDataStruct demoData = null;
  
  public NavdataChannel(DroneConfigurationChannel droneConfigChannel) {
    this.droneConfigChannel = droneConfigChannel;
  }
  
  public synchronized void connect(InetAddress remoteAddress) throws SocketException, UnknownHostException {
    disconnect();
    
    navdataSocket = new DatagramSocket(NAVDATA_PORT);
    navdataSocket.setSoTimeout(500);
    this.remoteAddress = remoteAddress;
    
    navdataThread = new Thread(this);
    navdataStartTime = System.currentTimeMillis();
    navdataThread.start();
  }

  public void disconnect() {
    if (navdataThread != null) {
      navdataThread.interrupt();
      try {
        navdataThread.join();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      navdataThread = null;
    }
    
    synchronized (this) {
      if (navdataSocket != null) {
        navdataSocket.close();
        navdataSocket = null;
      }
  
      lastTimeStamp = -1;
      lastSequenceNumber = -1;
      navdataStartTime = -1;
      demoData = null;
      remoteAddress = null;
    }
  }

  private boolean processPacket(DatagramPacket packet) {
    NavdataChannelObserver[] obs = {};
    
    NavdataPacket navdataPacket = new NavdataPacket();
    
    synchronized (this) {
      try {
        navdataPacket.parseNavdata(packet);
        if (navdataPacket.sequenceNumber < lastSequenceNumber) {
          return false;
        }
        lastSequenceNumber = navdataPacket.sequenceNumber;
  
        lastTimeStamp = System.currentTimeMillis() - navdataStartTime;
        droneStatus = navdataPacket.droneState;      
        demoData = navdataPacket.demodata;
        
        synchronized (observers) {
          obs = observers.toArray(new NavdataChannelObserver[0]);
        }
      } catch (NavdataPacketFormatException e) {
        e.printStackTrace();
        return false;
      }
    }

    for (NavdataChannelObserver observer : obs) {
      observer.newNavdata(this);
    }

    return true;
  }
  
  public void addNavdataChannelObserver(NavdataChannelObserver observer) {
    synchronized (observers) {
      if (!observers.contains(observer)) {
        observers.add(observer);
      }
    }
  }
  
  public void removeNavdataChannelObserver(NavdataChannelObserver observer) {
    synchronized (observers) {
      observers.remove(observer);
    }
  }
  
  @Override
  public void run() {
    boolean initialized = false;
    
    byte[] initDatagramData = {1, 0, 0, 0};
    DatagramPacket initPacket = new DatagramPacket(initDatagramData, initDatagramData.length, remoteAddress, NAVDATA_PORT);
    
    byte[] receivedDatagramData = new byte[4096];
    DatagramPacket receivePacket = new DatagramPacket(receivedDatagramData, receivedDatagramData.length);
    
    while (!Thread.currentThread().isInterrupted()) {
      if (!initialized) {
        synchronized (this) {
          lastSequenceNumber = -1;
        }
        
        // Send initialization package
        try {
          navdataSocket.send(initPacket);
          droneConfigChannel.setNavdataType(true);

          navdataSocket.receive(receivePacket);
          
          if (processPacket(receivePacket)) {
            droneConfigChannel.setNavdataType(true);
          
            initialized = true;
          }
        } catch (IOException e) {
          System.err.println("Could not initialize navdata stream, retrying!");
          e.printStackTrace();
        }
      } else {
        try {
          navdataSocket.receive(receivePacket);
          
          if (processPacket(receivePacket)) {
            // Successful
          }
        } catch (IOException e) {
          initialized = false;
          try {
            Thread.sleep(10);
          } catch (InterruptedException e1) {
            Thread.currentThread().interrupt();
          }
        }
      }
    }
  }
  
  public synchronized long getLastNavdataTimestamp() {
    return lastTimeStamp;
  }
  
  
  public synchronized int getLastDroneStatus() {
    return droneStatus;
  }

  public synchronized DemoDataStruct getLastDemoData() {
    return demoData;
  }
}
