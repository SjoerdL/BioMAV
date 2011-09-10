/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.ru.ai.projects.parrot.dronecontrol.manualcontrol;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

/**
 * 
 * @author Vincent
 */
public class Model {

  protected SimulatedDrone drone;

  public Model(Control control) throws InterruptedException {
    Simulator simulator = Simulator.getInstance();
//    simulator.setCoupleTime(false);
    simulator.activateRendering(true);
    
    drone = simulator.getDrone(0);
    DroneControl droneControl = new DroneControl(control, drone);
    droneControl.run();
  }

  private class DroneControl implements Runnable {
    protected SimulatedDrone drone;

    Control control;

    DroneControl(Control control, SimulatedDrone drone) {
      this.drone = drone;
      this.control = control;
    }

    public void run() {
      while (true) {
        try {
          Thread.sleep(10);
        } catch (InterruptedException e) {
          Thread.currentThread().interrupt();
        }
        
        drone.hover();

        if (control.getUp()) {
          drone.flyForward(1.0);
        }
        if (control.getDown()) {
          drone.flyForward(-1.0);
        }
        if (control.getLeft()) {
          drone.spin(-1.0);
        }
        if (control.getRigth()) {
          drone.spin(1.0);
        }
        if (control.getXButton()) {
          drone.land();
        }
        if (control.getZButton()) {
          drone.clearEmergencyFlag();
          drone.takeoff();
        }
        if (control.getAButton()) {
          drone.flySideways(-1.0);
        }
        if (control.getDButton()) {
          drone.flySideways(1.0);
        }

      }

    }
  }
}
