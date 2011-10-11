package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.TwitterTweets;

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
