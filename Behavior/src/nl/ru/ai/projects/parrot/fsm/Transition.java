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
 * Defines a (uni-)directional transitions to a given state
 * @author sjoerdlagarde
 */
public abstract class Transition {

    private State connectedTo;
    private Behavior transitionBehavior;
    protected ParrotDroneInterface droneInterface;
    protected PolePositionPollInterface polesInterface;
    protected int duration;
    protected double threshold;
    
    public Transition(State toState, ParrotDroneInterface droneInterface, PolePositionPollInterface polesInterface) {
        connectedTo = toState;
        transitionBehavior = null;
        duration = 0;
        threshold = 0;
        
        this.droneInterface = droneInterface;
        this.polesInterface = polesInterface;
    }

    /**
     * Returns the state to which the transition leads
     * @return The state to which the transition leads
     */
    public State getConnectedState() {
        return connectedTo;
    }

    /**
     * Sets the behavior that will be executed when the transition is made
     * @param behavior The behavior to execute when the transition is made
     */
    public void setTransitionBehavior(Behavior behavior) {
        this.transitionBehavior = behavior;
    }

    /**
     * Checks whether or not a transition is appropriate
     * @return True when a transition needs to be executed, false otherwise
     */
    public abstract boolean fire();

    /**
     * Resets the transition to its original state
     */
    public void reset() {
    	;
    }

    /**
     * Runs the behavior associated with the transition.
     * If no behavior is defined, no behavior will be executed
     * @throws InterruptedException 
     */
    public void run() throws InterruptedException {
    	if ( State.outputCurrentState )
    		System.out.println("Transition: "+this.getClass().getSimpleName());
        if ( transitionBehavior != null )
            transitionBehavior.run();
    }
    
    /**
     * Returns the duration that is internally used by the transition (e.g. as a timer or threshold)
     * @return The duration
     */
    public int getDuration() {
    	return duration;
    }
    
    /**
     * Sets the duration that will be used internally by the transition (e.g. as a timer or threshold )
     * @param duration The duration
     */
    public void setDuration(int duration) {
    	this.duration = duration;
    }

}
