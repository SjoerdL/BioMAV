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
public class Hover extends Behavior {
	
	public Hover(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
	}

	@Override
	public boolean fire() {
		return true;
	}

	@Override
	public void run() throws InterruptedException {
		if ( Math.random() <= 0.4 ) 
			TwitterAccess.getInstance().setStatus(TwitterTweets.getRandomTweet(TwitterTweets.hover));
		else if ( TwitterAccess.getInstance().isEnabled() )
			TwitterAccess.getInstance().uploadImage(drone.getFrontCameraImage(), TwitterTweets.getRandomTweet(TwitterTweets.camera));
		
		drone.hover();
		Sleeper.getInstance(drone).sleep(duration); 		// Give the drone some time to fully hover
	}

}
