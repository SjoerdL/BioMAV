package nl.ru.ai.projects.parrot.tools;

import nl.ru.ai.projects.parrot.dronecontrol.SensoryDataInterface;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;


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
