package nl.ru.ai.projects.parrot.tools;

public class TwitterTweets {

	
	public static String initializeDrone[] = { "Whoah! I'm awake again!", "Mighty parrots of the wind, I AM ALIVE!", "Hi there: http://goo.gl/WGvy8", "Rise and shine, time to fly!" };
	public static String hover[] = { "Yep, hanging in the air right now!", "Ha! I am defeating gravity!", "MAYDAY MAYDAY! GRAVITONS ARE TAKING OVER!", "Thinking about her... http://goo.gl/qvXrN" };
	public static String camera[] = { "Check this out!", "All chickens on a stick, what a view!", "", "No way I'm flying over there", "CLICK, picture taken.", "I wonder if it is safe to fly over here..." };

	public static String random[] = { "I feel like I'm flying circles here!", "Space... The next frontier!", "Pff, who needs wings when you got props like these?!", "Hmmz, should I try facebook?! #fb", "Problem? Let me try my bird-eye perspective!", 
		"Bleep.. bleep.. bleep..", "Did I mention I'm a bit afraid of heights?! WHAAAHHHHH #scared", "Roger that!", "Golf Yankee Romeo Oscar" };
	
	public static String getRandomTweet(String[] tweets) {
		int randomValue = (int) (Math.random()*tweets.length);
		return tweets[randomValue];
	}
}
