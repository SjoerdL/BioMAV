package nl.ru.ai.projects.parrot.tools;

/**
 * Defines a condition that will stop the FSM when met
 * @author sjoerdlagarde
 */
public abstract class TerminalCondition {

    public TerminalCondition() {

    }

    /**
     * Resets the terminal condition to its original state
     */
    public void reset() {
    	;
    }
    
    public abstract boolean terminate();

}
