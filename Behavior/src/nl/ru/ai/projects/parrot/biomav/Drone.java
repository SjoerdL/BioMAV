/*
    This file is part of the BioMAV project.

    The BioMAV project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The BioMAV project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The BioMAV project. If not, see <http://www.gnu.org/licenses/>.
*/
package nl.ru.ai.projects.parrot.biomav;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

/**
 * @author sjoerdlagarde
 */
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
