package nl.ru.ai.projects.parrot.ea.fitness;

import java.util.Random;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.ea.evolve.EAConfiguration;
import nl.ru.ai.projects.parrot.tools.BehaviorModuleSL;
import nl.ru.ai.projects.parrot.tools.TerminatorTimer;

import org.jgap.FitnessFunction;
import org.jgap.IChromosome;

public abstract class EAFitness extends FitnessFunction {

	private static final long serialVersionUID = 1L;
	protected EAConfiguration config;
	private BehaviorModuleSL behaviorModule; 
	private Random rand;
	
	public EAFitness(EAConfiguration config) {
		this.config = config;
		rand = new Random(System.currentTimeMillis());
		
		Simulator sim = Simulator.getInstance();
		behaviorModule = new BehaviorModuleSL(sim.getDrone(0), sim.getDrone(0)); 

        // Add terminal conditions and execute the FSM
        TerminatorTimer timerTerminator = new TerminatorTimer();
        timerTerminator.setThreshold(config.FSM_TIMER_THRESHOLD);
        behaviorModule.addTerminalCondition(timerTerminator);
	}
	
	/**
	 * Calculates the fitness based on the flown coordinates
	 * @return The fitness value, where a lower value means a fitter individual
	 */	
	protected abstract double evaluate(IChromosome aSubject);
	
	protected void setupEA(IChromosome subject) throws InterruptedException {
		int durations[] = new int[10];
		for ( int i=0; i<durations.length; i++ )
			durations[i] = 100;
		for ( int i=0; i<subject.size(); i++ ) 
			durations[i] = (Integer)subject.getGene(i).getAllele();

		// Box-Muller transform
		double u1 = rand.nextDouble();
		double u2 = rand.nextDouble();
		double R = Math.sqrt((-2)*Math.log(u1));
		double th = 2*Math.PI*u2;
		double z0 = R*Math.cos(th)*0.3;
		double z1 = R*Math.sin(th)*0.3;		
		
		SimulatedDrone drone = Simulator.getInstance().getDrone(0);
    	drone.runScript("drone:setPositionAndResetOrientation("+z0+", "+z1+", 0);");

		behaviorModule.resetFSM();
		behaviorModule.poleFoundLeft.setDuration(durations[0]);
		behaviorModule.poleFoundRight.setDuration(durations[1]);
		behaviorModule.outOfSightLeft.setDuration(durations[2]);
		behaviorModule.outOfSightRight.setDuration(durations[3]);
		behaviorModule.poleLostLeft.setDuration(durations[4]);
		behaviorModule.poleFoundRight.setDuration(durations[5]);
		behaviorModule.closeToPoleLeft.setDuration(durations[6]);
		behaviorModule.closeToPoleRight.setDuration(durations[7]);
		behaviorModule.executeFSM();
		
		System.out.flush();
	}
}
