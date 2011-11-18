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
