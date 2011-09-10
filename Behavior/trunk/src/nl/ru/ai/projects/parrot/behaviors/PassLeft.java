package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.biomav.BioMAVConfig;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;

public class PassLeft extends Behavior {
	
	public PassLeft(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
	}

	@Override
	public boolean fire() {
		return true; 		// Behavior will always be executed
	}

	@Override
	public void run() throws InterruptedException {
		drone.flyForward(0.10*BioMAVConfig.forwardScaling);		
		drone.flySideways(-0.15*BioMAVConfig.sidewaysScaling);
		Sleeper.getInstance(drone).sleep(duration);
	}

}
