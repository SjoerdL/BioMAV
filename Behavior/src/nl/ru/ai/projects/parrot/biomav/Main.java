package nl.ru.ai.projects.parrot.biomav;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.ea.evolve.EASetup;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.tools.BehaviorModule;
import nl.ru.ai.projects.parrot.tools.BehaviorModule1;
import nl.ru.ai.projects.parrot.tools.BehaviorModuleLB;
import nl.ru.ai.projects.parrot.tools.BehaviorModuleSL;
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.Writer;


/**
 *
 * @author sjoerdlagarde
 */
public class Main {
	
    /**
     * @param args the command line arguments
     * @throws InterruptedException 
     */
    public static void main(String[] args) throws InterruptedException {
    	TwitterAccess.getInstance().setEnabled(false); 	// True to have the drone tweet
    	State.setLogDronePosition(false); 				// True to log positions of drone to txt-file
    	State.setOutputCurrentState(false);
    	
    	Simulator sim = Simulator.getInstance();
    	sim.activateRendering(true);
    	sim.setCoupleTime(true);
    	
    	
    	//BehaviorModule module = new BehaviorModule1(sim.getDrone(0), sim.getDrone(0));
    	//BehaviorModule module = new BehaviorModule2(sim.getDrone(0), sim.getDrone(0));
    	//BehaviorModule module = new BehaviorModuleLB(sim.getDrone(0), sim.getDrone(0));
    	BehaviorModule module = new BehaviorModuleSL(sim.getDrone(0), sim.getDrone(0));
    	module.executeFSM();
    	System.exit(0);
    	
    	
    	/*
    	EASetup setup = new EASetup();
    	setup.run();
    	
    	Integer[] timings = setup.getTimings();
    	Writer out = Writer.getInstance();
    	out.setFilename("outputTimings.txt");
    	String timing = setup.getFitnessBestChromosome()+"\n";
    	for ( int i=0; i<timings.length; i++ )
    		timing+=timings[i]+"; ";   	
    	out.write(timing);
    	out.close();
    	
    	System.exit(0);
    	*/
    }

}
