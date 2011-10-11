package nl.ru.ai.projects.parrot.biomav;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

public class Drone {

	private static Drone instance = null;
	
	private Simulator simulator;
	private SimulatedDrone drone;
  
	public static Drone getInstance() {
		if ( instance == null )
			instance = new Drone();
		return instance;
	}
  
	private Drone() {
		simulator = Simulator.getInstance();
		drone = simulator.getDrone(0);
	}
  
	/**
	 * Returns the current drone
	 * @return The drone
	 */
	public SimulatedDrone getDrone() {
		return drone;
	}
  
	/**
	 * Returns the simulator environment
	 * @return The simulator
	 */
	public Simulator getSimulator() {
		return simulator;
	}
}
