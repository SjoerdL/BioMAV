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

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.FSMController;

/**
 * @author sjoerdlagarde
 */
public abstract class BehaviorModule {

	protected FSMController fsm;
	protected ParrotDroneInterface pdi;
	protected PolePositionPollInterface pppi;
	
	public BehaviorModule(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		this.pdi = pdi;
		this.pppi = pppi;
		fsm = new FSMController();
	}
	
	/**
	 * Executes the Finite State Machine
	 * @throws InterruptedException
	 */
	public void executeFSM() throws InterruptedException {
		fsm.execute();
	}
	
	/**
	 * Resets the Finite State Machine
	 */
	public void resetFSM() {
		fsm.reset();
	}
	
	/**
	 * Adds a terminal condition to the Finite State Machine
	 * @param condition The condition to add
	 */
	public void addTerminalCondition(TerminalCondition condition) {
		fsm.addTerminalCondition(condition);
	}
	
	/**
	 * Returns the Finite State Machine controller
	 * @return The FSM Controller
	 */
	public FSMController getFSMController() {
		return fsm;
	}	
	
}
