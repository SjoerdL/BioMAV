package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.TwitterTweets;

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
