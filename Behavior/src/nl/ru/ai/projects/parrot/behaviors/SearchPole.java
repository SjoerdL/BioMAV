package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.biomav.BioMAVConfig;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;

public class SearchPole extends Behavior {

	private long lastSearch;
	private int mode;
	private int spinCounter;
	private int multiplier;
	
	public SearchPole(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
		reset();
	}

	@Override
	public boolean fire() {
		return true;
	}
	
	public void reset() {
		lastSearch = 0;
		mode = 0;
		spinCounter = 0;
		multiplier = 1;
	}

	@Override
	public void run() throws InterruptedException {
		if ( drone.getLastSensoryDataTimeStamp()-lastSearch > 30000 ) 
			reset();
		
		switch ( mode ) {
		case 0:  		// left
			drone.spin(0.10*BioMAVConfig.spinScaling);
			spinCounter++;
			break;
		case 1: 		// right
			drone.spin(-0.10*BioMAVConfig.spinScaling);
			spinCounter++;
			break;
		}
		
		if ( spinCounter > 4*multiplier ) {
			mode = ( mode == 0 ) ? 1 : 0;
			spinCounter = 0;
			multiplier+=2;
		}
		
		Sleeper.getInstance(drone).sleep(getDuration());
		lastSearch = drone.getLastSensoryDataTimeStamp();
	}
}
