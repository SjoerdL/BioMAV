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
