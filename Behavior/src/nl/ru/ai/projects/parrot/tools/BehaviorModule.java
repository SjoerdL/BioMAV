package nl.ru.ai.projects.parrot.tools;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.FSMController;

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
