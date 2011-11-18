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
package nl.ru.ai.projects.parrot.fsm;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;

/**
 * Defines a behavior to be executed by the Drone
 * @author sjoerdlagarde
 */
public abstract class Behavior {

	protected ParrotDroneInterface drone;
	protected PolePositionPollInterface poleInterface;
	protected int duration; 		// The duration of the behavior in milliseconds
	protected double spinSpeed;
	
    public Behavior(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
    	duration = 250;
    	drone = pdi;
    	poleInterface = pppi;
    	spinSpeed = 0.1;
    }
    
    /**
     * Returns whether or not the behavior should be fired based on the input
     * @return True when the behavior should be fired, false otherwise
     */
    public abstract boolean fire();

    /**
     * Runs the behavior
     * @throws InterruptedException
     */
    public abstract void run() throws InterruptedException;
    
    /**
     * Resets the behavior to its initial state
     */
    public void reset() {
    	;
    }
    
    /**
     * Returns the duration of a behavior (i.e. how long the behavior is executed each time it fires)
     * @return The duration of the behavior in milliseconds
     */
    public int getDuration() {
    	return duration;
    }

    /**
     * Returns the spinspeed of a behavior (i.e. how long the behavior is executed each time it fires)
     * @return The duration of the behavior in milliseconds
     */
    public double getSpinSpeed(){
    	return spinSpeed;
    }
    
    /**
     * Sets the duration of a behavior (i.e. how long the behavior is executed each time it fires)
     * @param duration The duration of the behavior in milliseconds
     */
    public void setDuration(int duration) {
    	this.duration = duration;
    }
    
    /**
     * Sets the spinspeed of a behavior (i.e. how long the behavior is executed each time it fires)
     * @param duration The duration of the behavior in milliseconds
     */
    public void setSpinSpeed(int speed){
    	this.spinSpeed = ((double)speed) / 100.0;
    }
}
