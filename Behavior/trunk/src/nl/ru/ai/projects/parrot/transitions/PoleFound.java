package nl.ru.ai.projects.parrot.transitions;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class PoleFound extends Transition {

	private double epsilon;
	
	public PoleFound(State connectedTo, ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(connectedTo, pdi, pppi);
		epsilon = 0.15;
	}
	
	@Override
	public boolean fire() {
		Double[] poles = polesInterface.getPolePositions();
		epsilon = (double)getDuration()/100;
		for ( int i=0; i<poles.length; i++ ) {
			if ( poles[i] != null ) {
				return ( Math.abs(poles[i]) < epsilon );
			}
		}	
		return false;
	}

}
