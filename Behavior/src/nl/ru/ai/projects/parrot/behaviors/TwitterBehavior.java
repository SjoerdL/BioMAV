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
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.TwitterTweets;

/**
 * @author sjoerdlagarde
 */
public class TwitterBehavior extends Behavior {

	private boolean fired;
	
	public TwitterBehavior(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
		fired = false;
		duration = 0;
	}
	
	@Override
	public boolean fire() {
		if ( !fired ) {
			fired = true;
			return true;
		}

		return false;
	}

	@Override
	public void run() {
		TwitterAccess.getInstance().setStatus(TwitterTweets.getRandomTweet(TwitterTweets.random), 15);
	}
	

}
