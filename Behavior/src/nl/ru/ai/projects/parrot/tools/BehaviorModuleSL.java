package nl.ru.ai.projects.parrot.tools;

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
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;
import nl.ru.ai.projects.parrot.transitions.CloseToPole;
import nl.ru.ai.projects.parrot.transitions.OutOfSight;
import nl.ru.ai.projects.parrot.transitions.PoleFound;

public class BehaviorModuleSL extends BehaviorModule {

	public Transition poleFoundLeft;
	public Transition poleFoundRight;
	public Transition outOfSightRight;
	public Transition outOfSightLeft;
	public Transition poleLostLeft;
	public Transition poleLostRight;
	public Transition closeToPoleLeft;
	public Transition closeToPoleRight;
	
	public BehaviorModuleSL(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
		
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
        poleFoundLeft = new PoleFound(flyToPoleLeft, pdi, pppi);
        poleFoundLeft.setDuration(20);
        searchPoleLeft.addTransition(poleFoundLeft);
        
        poleFoundRight = new PoleFound(flyToPoleRight, pdi, pppi);
        poleFoundRight.setDuration(20);
        searchPoleRight.addTransition(poleFoundRight);
        
        turnLeft.addTransition(poleFoundRight);        
        turnRight.addTransition(poleFoundLeft); 
        
        outOfSightRight = new OutOfSight(turnRight, pdi, pppi);
        outOfSightRight.setDuration(5);
        leftPass.addTransition(outOfSightRight);
        
        outOfSightLeft = new OutOfSight(turnLeft, pdi, pppi);
        outOfSightLeft.setDuration(5);
        rightPass.addTransition(outOfSightLeft);
        
        poleLostLeft = new OutOfSight(searchPoleLeft, pdi, pppi);
        poleLostLeft.setDuration(5);
        flyToPoleLeft.addTransition(poleLostLeft);
        
        poleLostRight = new OutOfSight(searchPoleRight, pdi, pppi);
        poleLostRight.setDuration(5);
        flyToPoleRight.addTransition(poleLostRight);        
        
        closeToPoleLeft = new CloseToPole(rightPass, pdi, pppi);
        closeToPoleLeft.setDuration(211);
        flyToPoleLeft.addTransition(closeToPoleLeft);
        
        closeToPoleRight = new CloseToPole(leftPass, pdi, pppi);
        closeToPoleRight.setDuration(211);
        flyToPoleRight.addTransition(closeToPoleRight); 

        fsm.setInitialState(searchPoleRight);		
	}

}
