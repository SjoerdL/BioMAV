package nl.ru.ai.projects.parrot.dronecontrol.simulator.test;

import java.util.Scanner;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

public class Test02 {
  public static void main(String[] args) {
    final Simulator simulator = Simulator.getInstance();

    simulator.activateRendering(true);
    simulator.setCoupleTime(false);

    final SimulatedDrone drone = simulator.getDrone(0);

    // Create data print thread
    Thread dataThread = new Thread() {
      @Override
      public void run() {
        while (!Thread.currentThread().isInterrupted()) {
          double[] pos = drone.getDronePosition();
//          System.out.println(simulator.getCurrentTimeStamp() + " " + pos[0] + " " + pos[1] + " " + pos[2]);
          
          try {
            simulator.simSleep(100);
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
        }
      }
    };
    dataThread.setDaemon(true);
    dataThread.start();    
    
    drone.clearEmergencyFlag();
    drone.takeoff();
    drone.flyForward(1.0);
    try {
      drone.droneSleep(1000);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
    drone.hover();
    try {
      drone.droneSleep(10000);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
    
    drone.land();
    
    Scanner scanner = new Scanner(System.in);
    scanner.nextLine();
    
    System.exit(0);
  }
}
