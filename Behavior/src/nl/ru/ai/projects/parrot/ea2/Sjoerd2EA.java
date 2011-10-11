package nl.ru.ai.projects.parrot.ea2;

import java.util.Map;

import nl.ru.ai.projects.parrot.behaviors.FlyTowardsPole;
import nl.ru.ai.projects.parrot.behaviors.Hover;
import nl.ru.ai.projects.parrot.behaviors.InitializeDrone;
import nl.ru.ai.projects.parrot.behaviors.PassLeft;
import nl.ru.ai.projects.parrot.behaviors.PassRight;
import nl.ru.ai.projects.parrot.behaviors.SearchPole;
import nl.ru.ai.projects.parrot.behaviors.TurnLeft;
import nl.ru.ai.projects.parrot.behaviors.TurnRight;
import nl.ru.ai.projects.parrot.behaviors.TwitterBehavior;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.fsm.FSMController;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;
import nl.ru.ai.projects.parrot.transitions.CloseToPole;
import nl.ru.ai.projects.parrot.transitions.OutOfSight;
import nl.ru.ai.projects.parrot.transitions.PoleFound;

import org.jgap.Chromosome;
import org.jgap.Configuration;
import org.jgap.Gene;
import org.jgap.IChromosome;
import org.jgap.InvalidConfigurationException;
import org.jgap.impl.IntegerGene;

public class Sjoerd2EA implements DroneGAFactory {

	@Override
	public FSMController createFSM(ParrotDroneInterface pdi, PolePositionPollInterface pppi, Map<String, String> configurationData) {
		FSMController fsm = new FSMController();
		
		// Define states
	    State searchPoleLeft = new State("Search left pole");
	    State searchPoleRight = new State("Search right pole");
	    State leftPass = new State("Pass left");
	    State turnRight = new State("Turn right");
	    State rightPass = new State("Pass right");
	    State turnLeft = new State("Turn left");
	    State flyToPoleLeft = new State("Fly to pole left");
	    State flyToPoleRight = new State("Fly to pole right");
	    
        // Add behaviors to states
        Behavior initializeDroneBehavior = new InitializeDrone(pdi, pppi);
        initializeDroneBehavior.setDuration(2500);
        Behavior hoverBehavior = new Hover(pdi, pppi);
        hoverBehavior.setDuration(1500);
        Behavior twitterBehavior = new TwitterBehavior(pdi, pppi);
        Behavior searchPoleBehavior = new SearchPole(pdi, pppi);
        searchPoleBehavior.setDuration(200);
        Behavior passLeftBehavior = new PassLeft(pdi, pppi);
        passLeftBehavior.setDuration(250);
        Behavior passRightBehavior = new PassRight(pdi, pppi);
        passRightBehavior.setDuration(250);
        Behavior turnLeftBehavior = new TurnLeft(pdi, pppi);
        turnLeftBehavior.setDuration(250);
        Behavior turnRightBehavior = new TurnRight(pdi, pppi);
        turnRightBehavior.setDuration(250);
        Behavior flyToPoleLeftBehavior = new FlyTowardsPole(pdi, pppi);
        flyToPoleLeftBehavior.setDuration(250);
        Behavior flyToPoleRightBehavior = new FlyTowardsPole(pdi, pppi);
        flyToPoleRightBehavior.setDuration(250);
 
        searchPoleRight.setEntryBehavior(initializeDroneBehavior); 	// State       
        searchPoleRight.addBehavior(searchPoleBehavior); 
        searchPoleRight.setExitBehavior(hoverBehavior);
        searchPoleLeft.addBehavior(searchPoleBehavior);	 			// State
        searchPoleLeft.setExitBehavior(hoverBehavior);        
        leftPass.addBehavior(passLeftBehavior);  					// State
        leftPass.addBehavior(twitterBehavior);
        leftPass.setExitBehavior(hoverBehavior);        
        turnRight.addBehavior(turnRightBehavior);  					// State
        turnRight.addBehavior(twitterBehavior); 	
        turnRight.setExitBehavior(hoverBehavior);
        rightPass.addBehavior(passRightBehavior); 	 				// State
        rightPass.addBehavior(twitterBehavior); 
        rightPass.setExitBehavior(hoverBehavior);
        turnLeft.addBehavior(turnLeftBehavior);  					// State
        turnLeft.addBehavior(twitterBehavior);
        turnLeft.setExitBehavior(hoverBehavior);
        flyToPoleLeft.addBehavior(flyToPoleLeftBehavior);	 		// State
        flyToPoleLeft.addBehavior(twitterBehavior);
        flyToPoleLeft.setExitBehavior(hoverBehavior);
        flyToPoleRight.addBehavior(flyToPoleRightBehavior);	 		// State
        flyToPoleRight.addBehavior(twitterBehavior);
        flyToPoleRight.setExitBehavior(hoverBehavior);
        
        // Add transitions between states
        Transition poleFoundLeft = new PoleFound(flyToPoleLeft, pdi, pppi);
        poleFoundLeft.setDuration(20);
        searchPoleLeft.addTransition(poleFoundLeft);
        
        Transition poleFoundRight = new PoleFound(flyToPoleRight, pdi, pppi);
        poleFoundRight.setDuration(20);
        searchPoleRight.addTransition(poleFoundRight);
        
        turnLeft.addTransition(poleFoundRight);        
        turnRight.addTransition(poleFoundLeft); 
        
        Transition outOfSightRight = new OutOfSight(turnRight, pdi, pppi);
        outOfSightRight.setDuration(5);
        leftPass.addTransition(outOfSightRight);
        
        Transition outOfSightLeft = new OutOfSight(turnLeft, pdi, pppi);
        outOfSightLeft.setDuration(5);
        rightPass.addTransition(outOfSightLeft);
        
        Transition poleLostLeft = new OutOfSight(searchPoleLeft, pdi, pppi);
        poleLostLeft.setDuration(5);
        flyToPoleLeft.addTransition(poleLostLeft);
        
        Transition poleLostRight = new OutOfSight(searchPoleRight, pdi, pppi);
        poleLostRight.setDuration(5);
        flyToPoleRight.addTransition(poleLostRight);        
        
        Transition closeToPoleLeft = new CloseToPole(rightPass, pdi, pppi);
        closeToPoleLeft.setDuration(211);
        flyToPoleLeft.addTransition(closeToPoleLeft);
        
        Transition closeToPoleRight = new CloseToPole(leftPass, pdi, pppi);
        closeToPoleRight.setDuration(211);
        flyToPoleRight.addTransition(closeToPoleRight); 

        fsm.setInitialState(searchPoleRight);				
		
		poleFoundLeft.setDuration(Integer.parseInt(configurationData.get("0")));
		poleFoundRight.setDuration(Integer.parseInt(configurationData.get("1")));
		outOfSightLeft.setDuration(Integer.parseInt(configurationData.get("2")));
		outOfSightRight.setDuration(Integer.parseInt(configurationData.get("3")));
		poleLostLeft.setDuration(Integer.parseInt(configurationData.get("4")));
		poleFoundRight.setDuration(Integer.parseInt(configurationData.get("5")));
		closeToPoleLeft.setDuration(Integer.parseInt(configurationData.get("6")));
		closeToPoleRight.setDuration(Integer.parseInt(configurationData.get("7")));
        
		return fsm;
	}

	@Override
	public IChromosome createGenome(Configuration config) throws InvalidConfigurationException {
	    Gene[] sampleGenes = new Gene[8];
	    
		sampleGenes[0] = new IntegerGene(config, 0, 50);			// Pole found left (angle)
		sampleGenes[1] = new IntegerGene(config, 0, 50);			// Pole found right (angle)
		sampleGenes[2] = new IntegerGene(config, 0, 30); 			// Out of sight left
		sampleGenes[3] = new IntegerGene(config, 0, 30); 			// Out of sight right
		sampleGenes[4] = new IntegerGene(config, 0, 30); 			// Pole lost left
		sampleGenes[5] = new IntegerGene(config, 0, 30); 			// Pole lost right
		sampleGenes[6] = new IntegerGene(config, 50, 350); 			// Close to pole left
		sampleGenes[7] = new IntegerGene(config, 50, 350);			// Close to pole right
		
	    Chromosome result = new Chromosome(config, sampleGenes);
	    
	    return result;
	}

}
