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

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.TwitterTweets;

/**
 * @author sjoerdlagarde
 */
public class InitializeDrone extends Behavior {
	
	public InitializeDrone(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
	}

	@Override
	public boolean fire() {
		return false; 		// Only used as initial state, drone cannot take-off twice
	}

	@Override
	public void run() throws InterruptedException {
		TwitterAccess.getInstance().setStatus(TwitterTweets.getRandomTweet(TwitterTweets.initializeDrone), 95);

		drone.takeoff();
		Sleeper.getInstance(drone).sleep(getDuration());
		drone.hover();
		Sleeper.getInstance(drone).sleep(1000);
	}
}
