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

import java.util.ArrayList;

import nl.ru.ai.projects.parrot.tools.TerminalCondition;

/**
 * Controls and executes the Finite State Machine
 * @author sjoerdlagarde
 */
public class FSMController {

    private State initialState;
    private ArrayList<TerminalCondition> terminalConditions;

    public FSMController() {
        terminalConditions = new ArrayList<TerminalCondition>();
    }

    /**
     * Sets the initial state for the FSM
     * @param state The state to use as initial state
     */
    public void setInitialState(State state) {
        this.initialState = state;
    }

    /**
     * Adds a terminal condition to the list of terminal conditions
     * @param condition The condition to add
     */
    public void addTerminalCondition(TerminalCondition condition) {
        terminalConditions.add(condition);
    }

    /**
     * Executes the FSM
     * @throws InterruptedException 
     */
    public void execute() throws InterruptedException {
        State currentState = initialState;
        while ( !reachedTerminalCondition() && currentState!=null ) {
            currentState = currentState.run();
            if ( currentState.isFinalState() )
                break;
        }
    }
    
    /**
     * Resets the Finite State Machine Controller
     */
    public void reset() {
    	initialState.clearResetStates();
    	initialState.reset();
    	
    	for ( int i=0; i<terminalConditions.size(); i++ )
    		terminalConditions.get(i).reset();    	
    }

    /**
     * Checks whether or not a terminal condition has been reached
     * @return True when a terminal condition has been reached, false otherwise
     */
    private boolean reachedTerminalCondition() {
        for ( int i=0; i<terminalConditions.size(); i++ ) {
            if ( terminalConditions.get(i).terminate() ) 
            	return true;
        }

        return false;
    }

}
