package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.biomav.BioMAVConfig;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;

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
