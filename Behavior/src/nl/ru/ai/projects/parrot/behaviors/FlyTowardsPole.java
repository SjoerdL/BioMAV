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
package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.biomav.BioMAVConfig;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;

/**
 * @author sjoerdlagarde
 */
public class FlyTowardsPole extends Behavior {

	private double threshold;
	
	public FlyTowardsPole(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
		threshold = 0.15;
	}

	@Override
	public boolean fire() {
		return true;
	}

	@Override
	public void run() throws InterruptedException {
		drone.spin(0.0);
		drone.flyForward(0.20*BioMAVConfig.forwardScaling);
		
		Double[] poles = poleInterface.getPolePositions();
		if ( poles!=null && poles.length>0 ) {
			Double angle = null; 
			for ( int i=0; i<poles.length; i++ ) {
				if ( poles[i] != null )
					angle = poles[i];
			}

			if ( angle == null ) { 	
				Sleeper.getInstance(drone).sleep(250);
			} else {
				if ( angle < -threshold ) {  		// Pole to the left
					drone.spin(-0.07*BioMAVConfig.spinScaling);
					Sleeper.getInstance(drone).sleep(100);
				} else if ( angle > threshold) { 	// Pole to the right
					drone.spin(0.07*BioMAVConfig.spinScaling);
					Sleeper.getInstance(drone).sleep(100);
				} else {
					Sleeper.getInstance(drone).sleep(250);
				}
			}
		} else { 			// No pole information available, so spin a bit
			drone.hover();
			Sleeper.getInstance(drone).sleep(1000);
			drone.spin(0.1*BioMAVConfig.spinScaling);
			Sleeper.getInstance(drone).sleep(500);
		}
	}

}
