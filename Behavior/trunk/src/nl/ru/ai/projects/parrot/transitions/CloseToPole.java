package nl.ru.ai.projects.parrot.transitions;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class CloseToPole extends Transition {

	//threshold in cm
	
	public CloseToPole(State connectedTo, ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(connectedTo, pdi, pppi);
	}

	@Override
	public boolean fire() {
		Double[] poleDistances = polesInterface.getPoleDistances();
		for ( int i=0; i<poleDistances.length; i++ ) {
			if ( poleDistances[i] != null && poleDistances[i] < getDuration()/100.0 )
				return true;
		}
		
		return false;
	}

}
