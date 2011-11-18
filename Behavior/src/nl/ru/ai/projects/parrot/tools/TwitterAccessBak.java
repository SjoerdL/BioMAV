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
public class TwitterAccessBak {

	private static TwitterAccessBak instance = null;
	private boolean enabled;
	
	private TwitterAccessBak() {
	}
	
	public static TwitterAccessBak getInstance() {
		if ( instance == null )
			instance = new TwitterAccessBak();
		
		return instance;
	}
	
	/**
	 * Sets whether or not tweets should be posted
	 * @param enabled True when tweets should be posted to twitter, false otherwise
	 */
	public void setEnabled(boolean enabled) {
		this.enabled = false;
	}
	
	/**
	 * Sets the status on Twiter with a predefined chance of 75%
	 * @param status The status to set
	 */
	public void setStatus(String status) {
		setStatus(status, 75);
		if ( enabled )
			;
	}
	
	/**
	 * Sets the status on Twitter with a given chance
	 * @param status The status to set
	 * @param chance The chance that the status will be set
	 */
	public void setStatus(String status, int chance) {
		;
	}
	
	/**
	 * Uploads an image
	 * @param image The image to upload
	 * @param tweet The text of the tweet that needs to be posted with the image
	 */
	public void uploadImage(byte[] image, String tweet) {
		;
	}
}
