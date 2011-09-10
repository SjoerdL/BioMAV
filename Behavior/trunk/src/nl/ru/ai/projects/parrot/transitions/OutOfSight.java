package nl.ru.ai.projects.parrot.transitions;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class OutOfSight extends Transition {
	
	private long outOfSightCounter;
	
	public OutOfSight(State connectedTo, ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(connectedTo, pdi, pppi);
		reset();
	}
	
	public void reset() {
		outOfSightCounter = 0;
	}
	
	@Override
	public boolean fire() {
		Double[] poles = polesInterface.getPolePositions();
		Double angle = null;
		for ( int i=0; i<poles.length; i++ ) {
			if ( poles[i] != null )
				angle = poles[i];
		}
		
		if ( angle == null )
			return verifyOutOfSight();
		else
			outOfSightCounter = ( outOfSightCounter <= 0 ) ? 0 : outOfSightCounter-1;
		
		return false;
	}
	
	/**
	 * Checks whether the pole is really out of sight or the drone just missed it by 
	 * accidental spinning for example
	 * @return True when the pole is really out of sight, false otherwise
	 */
	private boolean verifyOutOfSight() {
		outOfSightCounter++;
		if ( outOfSightCounter >= getDuration() ) {
			this.reset();
			return true;
		}
		
		return false;
	}
}
