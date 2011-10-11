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

public class LaurieEA implements DroneGAFactory {

	public Behavior initializeDroneBehavior;
	public Behavior hoverBehavior;
	public Behavior searchPoleBehavior;
	public Behavior passLeftBehavior;
	public Behavior passRightBehavior;
	public Behavior turnLeftBehavior;
	public Behavior turnRightBehavior;
	public Behavior flyToPoleLeftBehavior;
	public Behavior flyToPoleRightBehavior;
	public Transition poleFound;
	public Transition poleFoundRight;
	public Transition outOfSightRight;
	//public Transition turnCompleteRight;
	public Transition outOfSightLeft;	
	//public Transition turnCompleteLeft;
	public Transition poleLostLeft;
	public Transition poleLostRight;
	public Transition closeToPoleLeft;
	public Transition closeToPoleRight;
	@Override
	public FSMController createFSM(ParrotDroneInterface pdi,
			PolePositionPollInterface pppi,
			Map<String, String> configurationData) {
		FSMController fsm = new FSMController();
		
		System.out.println("Laurie's shizzle");
		// Define states
	    State searchPole = new State("Search pole");
	    State searchPoleRight = new State("Search pole right");
	    State leftPass = new State("Pass left");
	    State turnRight = new State("Turn right");
	    State rightPass = new State("Pass right");
	    State turnLeft = new State("Turn left");
	    State flyToPoleLeft = new State("Fly to pole left");
	    State flyToPoleRight = new State("Fly to pole right");
	    
	 // Add behaviors to states
        initializeDroneBehavior = new InitializeDrone(pdi, pppi);
        initializeDroneBehavior.setDuration(2500);
        hoverBehavior = new Hover(pdi, pppi);
        hoverBehavior.setDuration(200);
        Behavior twitterBehavior = new TwitterBehavior(pdi, pppi);
        searchPoleBehavior = new SearchPole(pdi, pppi);
        searchPoleBehavior.setDuration(200);
        passLeftBehavior = new PassLeft(pdi, pppi);
        searchPoleBehavior.setDuration(250);
        passRightBehavior = new PassRight(pdi, pppi);
        passRightBehavior.setDuration(250);
        turnLeftBehavior = new TurnLeft(pdi, pppi);
        turnLeftBehavior.setDuration(250);
        turnRightBehavior = new TurnRight(pdi, pppi);
        turnRightBehavior.setDuration(250);
        flyToPoleLeftBehavior = new FlyTowardsPole(pdi, pppi);
        flyToPoleLeftBehavior.setDuration(250);
        flyToPoleRightBehavior = new FlyTowardsPole(pdi, pppi);
        flyToPoleRightBehavior.setDuration(250);
 
        searchPole.setEntryBehavior(initializeDroneBehavior); 	// Put the drone in the air        
        searchPole.addBehavior(searchPoleBehavior); 		// Initial state
        searchPole.setExitBehavior(hoverBehavior);
        searchPoleRight.addBehavior(searchPoleBehavior);	// State 0.5
        searchPoleRight.setExitBehavior(hoverBehavior);        
        leftPass.addBehavior(passLeftBehavior); 			// State 1
        leftPass.addBehavior(twitterBehavior);
        leftPass.setExitBehavior(hoverBehavior);        
        turnRight.addBehavior(turnRightBehavior); 			// State 2
        turnRight.addBehavior(twitterBehavior); 	
        turnRight.setExitBehavior(hoverBehavior);
        rightPass.addBehavior(passRightBehavior); 			// State 3
        rightPass.addBehavior(twitterBehavior); 
        rightPass.setExitBehavior(hoverBehavior);
        turnLeft.addBehavior(turnLeftBehavior); 			// State 4
        turnLeft.addBehavior(twitterBehavior);
        turnLeft.setExitBehavior(hoverBehavior);
        flyToPoleLeft.addBehavior(flyToPoleLeftBehavior);		// State 5
        flyToPoleLeft.addBehavior(twitterBehavior);
        flyToPoleLeft.setExitBehavior(hoverBehavior);
        flyToPoleLeft.addBehavior(flyToPoleRightBehavior);		// State 6
        flyToPoleLeft.addBehavior(twitterBehavior);
        flyToPoleLeft.setExitBehavior(hoverBehavior);
        
        // Add transitions between states
        poleFound = new PoleFound(flyToPoleLeft, pdi, pppi);
        poleFound.setDuration(15);
        searchPole.addTransition(poleFound);
        turnLeft.addTransition(poleFound);                
        poleFoundRight = new PoleFound(flyToPoleRight, pdi, pppi);
        poleFoundRight.setDuration(15);
        turnRight.addTransition(poleFoundRight); 
        searchPoleRight.addTransition(poleFoundRight);
        outOfSightRight = new OutOfSight(turnRight, pdi, pppi);
        outOfSightRight.setDuration(8);
        leftPass.addTransition(outOfSightRight);
        //turnCompleteRight = new TurnComplete(rightPass, pdi, pppi);
        //turnCompleteRight.setDuration(195);
        //turnRight.addTransition(turnCompleteRight);
        outOfSightLeft = new OutOfSight(turnLeft, pdi, pppi);
        outOfSightLeft.setDuration(8);
        rightPass.addTransition(outOfSightLeft);
        //turnCompleteLeft = new TurnComplete(leftPass, pdi, pppi);
        //turnCompleteLeft.setDuration(195);
        //turnLeft.addTransition(turnCompleteLeft);
        poleLostLeft = new OutOfSight(searchPole, pdi, pppi);
        poleLostLeft.setDuration(2000);
        flyToPoleLeft.addTransition(poleLostLeft);
        poleLostRight = new OutOfSight(searchPoleRight, pdi, pppi);
        poleLostRight.setDuration(2000);
        flyToPoleRight.addTransition(poleLostRight);
        
        
        closeToPoleLeft = new CloseToPole(leftPass, pdi, pppi);
        closeToPoleLeft.setDuration(195);
        flyToPoleLeft.addTransition(closeToPoleLeft);
        closeToPoleRight = new CloseToPole(rightPass, pdi, pppi);
        closeToPoleRight.setDuration(195);
        flyToPoleRight.addTransition(closeToPoleRight);

        fsm.setInitialState(searchPole);
        
        initializeDroneBehavior.setDuration(2500);
		hoverBehavior.setDuration(2000);		
		closeToPoleLeft.setDuration(Integer.parseInt(configurationData.get("0")));
		closeToPoleRight.setDuration(Integer.parseInt(configurationData.get("0")));
		outOfSightRight.setDuration(Integer.parseInt(configurationData.get("1")) * 10);
		outOfSightLeft.setDuration(Integer.parseInt(configurationData.get("1")) * 10);
		poleLostLeft.setDuration(Integer.parseInt(configurationData.get("1")));
		poleLostRight.setDuration(Integer.parseInt(configurationData.get("1")) * -1);
		poleFound.setDuration(Integer.parseInt(configurationData.get("2")));
		poleFoundRight.setDuration(Integer.parseInt(configurationData.get("2")));	
		flyToPoleLeftBehavior.setDuration(Integer.parseInt(configurationData.get("3")));
		flyToPoleRightBehavior.setDuration(Integer.parseInt(configurationData.get("3")));
		flyToPoleLeftBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("4")));
		flyToPoleRightBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("4")));
		passLeftBehavior.setDuration(Integer.parseInt(configurationData.get("5")));
		passRightBehavior.setDuration(Integer.parseInt(configurationData.get("5")));
		passLeftBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("6")));
		passRightBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("6")));
		searchPoleBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("7")));
		turnLeftBehavior.setDuration(Integer.parseInt(configurationData.get("8")));
		turnRightBehavior.setDuration(Integer.parseInt(configurationData.get("8")));
		turnLeftBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("9")));
		turnRightBehavior.setSpinSpeed(Integer.parseInt(configurationData.get("9")));
		return fsm;
	}

	@Override
	public IChromosome createGenome(Configuration config)
			throws InvalidConfigurationException {
		Gene[] sampleGenes = new Gene[10];
		///*
		sampleGenes[0] = new IntegerGene(config, 0, 300);			// CloseToPole
		sampleGenes[1] = new IntegerGene(config, 0, 200);			// Out of Sight
		sampleGenes[2] = new IntegerGene(config, 5, 90); 			// Pole Found
		sampleGenes[3] = new IntegerGene(config, 0, 100);			//FlyToWardsPoleForwardSpeed
		sampleGenes[4] = new IntegerGene(config, 0, 20);			// FlyToWardsPoleSpinSpeed
		sampleGenes[5] = new IntegerGene(config, 0, 100);		 	// PassForwardSpeed
		sampleGenes[6] = new IntegerGene(config, 0, 20);		 	// PassSpinSpeed
		sampleGenes[7] = new IntegerGene(config, 0, 20);			// SearchPoleSpinSpeed
		sampleGenes[8] = new IntegerGene(config, 0, 100);		 	// TurnForwardSpeed
		sampleGenes[9] = new IntegerGene(config, 0, 20);		 	// TurnSpinSpeed
		
		//*/
		/*
		sampleGenes[0] = new IntegerGene(config, 100, 101);			// CloseToPole
		sampleGenes[1] = new IntegerGene(config, 501, 502);			// Out of Sight
		sampleGenes[2] = new IntegerGene(config, 15, 16); 			// Pole Found
		sampleGenes[3] = new IntegerGene(config, 10, 11);			//FlyTowardsPoleForwardSpeed
		sampleGenes[4] = new IntegerGene(config, 4, 5);			// FlyToWardsPoleSpinSpeed
		sampleGenes[5] = new IntegerGene(config, 20, 21);		 	// PassForwardSpeed
		sampleGenes[6] = new IntegerGene(config, 7, 8);		 	// PassSpinSpeed
		sampleGenes[7] = new IntegerGene(config, 10, 11);			// SearchPoleSpinSpeed
		sampleGenes[8] = new IntegerGene(config, 9, 10);		 	// TurnForwardSpeed
		sampleGenes[9] = new IntegerGene(config, 8, 9);		 	// TurnSpinSpeed
		// */

	    Chromosome result = new Chromosome(config, sampleGenes);
	    
	    return result;
	}

}
