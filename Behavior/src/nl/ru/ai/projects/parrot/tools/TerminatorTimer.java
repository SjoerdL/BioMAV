package nl.ru.ai.projects.parrot.tools;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

/**
 * 
 * @author sjoerdlagarde
 */
public class TerminatorTimer extends TerminalCondition {

	private long time;
	private int threshold;
	
	public TerminatorTimer() {
		time = Simulator.getInstance().getCurrentTimeStamp();
		threshold = 30000;  	// Threshold is set to 30 seconds 
	}

	public void reset() {
		time = Simulator.getInstance().getCurrentTimeStamp();
	}
	
	/**
	 * Sets the threshold in milliseconds after which termination should occur
	 * @param threshold The threshold in milliseconds
	 */
	public void setThreshold(int threshold) {
		this.threshold = threshold;
	}
	
	@Override
	public boolean terminate() {
		return (Simulator.getInstance().getCurrentTimeStamp()-time > threshold);
	}
}
