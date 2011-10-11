package nl.ru.ai.projects.parrot.transitions;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class TurnComplete extends Transition {

	private boolean firstRotationMeasure;
	private double rotationSum;
	
	public TurnComplete(State connectedTo, ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(connectedTo, pdi, pppi);
		firstRotationMeasure = true;
		rotationSum = 0;
	}
	
	public void reset() {
		firstRotationMeasure = true;
		rotationSum = 0;
	}
	
	@Override
	public boolean fire() {
		double[] rotationVector = droneInterface.getCurrentRotationVector();
		if ( firstRotationMeasure ) {
			firstRotationMeasure = false;
			rotationSum = 0;
		} else
			rotationSum += rotationVector[2];

		if ( Math.abs(rotationSum*180/Math.PI) > getDuration() ) {
			firstRotationMeasure = true;
			return true;
		} 
		
		return false;
	}

}
