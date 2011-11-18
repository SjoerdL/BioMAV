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
package nl.ru.ai.projects.parrot.transitions;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

/**
 * @author sjoerdlagarde
 */
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
