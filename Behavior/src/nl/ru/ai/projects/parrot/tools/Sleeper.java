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

import nl.ru.ai.projects.parrot.dronecontrol.SensoryDataInterface;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

/**
 * @author sjoerdlagarde
 */
public class Sleeper {

	private static Sleeper instance = null;
	
	private SensoryDataInterface sensoryDataInterface = null;
	
	private Sleeper(SensoryDataInterface sensoryDataInterface) {
	  this.sensoryDataInterface = sensoryDataInterface;
	}
	
	public static synchronized Sleeper getInstance(SensoryDataInterface sensoryDataInterface) {
		if ( instance == null )
			instance = new Sleeper(sensoryDataInterface);
		
		return instance;
	}
	
	/**
	 * Tries to put the current thread into sleep (catches and ignores exceptions)
	 * @param millis The time in milliseconds for the thread to sleep
	 */
	public void sleep(long millis) throws InterruptedException {
		//drone.getDrone().droneSleep(millis);
		//drone.getSimulator().simSleep(millis);
	  
	  if (sensoryDataInterface != null) {
	    sensoryDataInterface.droneSleep(millis);
	  } else {
  		long start = Simulator.getInstance().getCurrentTimeStamp();
  		while ( Simulator.getInstance().getCurrentTimeStamp()-start < millis );
	  }
	}
}
