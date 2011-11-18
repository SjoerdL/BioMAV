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
package nl.ru.ai.projects.parrot.tools;

/**
 * @author sjoerdlagarde
 */
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
