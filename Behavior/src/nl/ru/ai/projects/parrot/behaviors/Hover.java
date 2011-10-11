package nl.ru.ai.projects.parrot.behaviors;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.tools.Sleeper;
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.TwitterTweets;

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
