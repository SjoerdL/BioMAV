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
package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.biomav.BioMAVConfig;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;

/**
 * @author sjoerdlagarde
 */
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
