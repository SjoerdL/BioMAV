package nl.ru.ai.projects.parrot.tools;


/**
 *
 * @author sjoerdlagarde
 */
public class TerminatorNumberOfIterations extends TerminalCondition {

    private int counter = 0;
    private int maximumNumberOfIterations = 0;

    /**
     * Sets the number of iterations after which termination should occur
     * @param iterations The number of iterations
     */
    public void setNumberOfIterations(int iterations) {
    	maximumNumberOfIterations = iterations;
    }
    
	public void reset() {
		counter = 0;
	}    
    
    @Override
    public boolean terminate() {
    	counter++;
        return ( counter > maximumNumberOfIterations );
    }

}
