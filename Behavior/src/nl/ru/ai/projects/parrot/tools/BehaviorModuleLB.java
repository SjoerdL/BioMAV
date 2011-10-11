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
import nl.ru.ai.projects.parrot.transitions.TurnComplete;

public class BehaviorModuleLB extends BehaviorModule {
	
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
	
	public BehaviorModuleLB(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
		
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
        poleFound.setDuration(35);
        searchPole.addTransition(poleFound);
        turnLeft.addTransition(poleFound);                
        poleFoundRight = new PoleFound(flyToPoleRight, pdi, pppi);
        poleFoundRight.setDuration(35);
        turnRight.addTransition(poleFoundRight); 
        searchPoleRight.addTransition(poleFoundRight);
        outOfSightRight = new OutOfSight(turnRight, pdi, pppi);
        outOfSightRight.setDuration(90);
        leftPass.addTransition(outOfSightRight);
        //turnCompleteRight = new TurnComplete(rightPass, pdi, pppi);
        //turnCompleteRight.setDuration(195);
        //turnRight.addTransition(turnCompleteRight);
        outOfSightLeft = new OutOfSight(turnLeft, pdi, pppi);
        outOfSightLeft.setDuration(90);
        rightPass.addTransition(outOfSightLeft);
        //turnCompleteLeft = new TurnComplete(leftPass, pdi, pppi);
        //turnCompleteLeft.setDuration(195);
        //turnLeft.addTransition(turnCompleteLeft);
        poleLostLeft = new OutOfSight(searchPole, pdi, pppi);
        poleLostLeft.setDuration(90);
        flyToPoleLeft.addTransition(poleLostLeft);
        poleLostRight = new OutOfSight(searchPoleRight, pdi, pppi);
        poleLostRight.setDuration(90);
        flyToPoleRight.addTransition(poleLostRight);
        
        
        closeToPoleLeft = new CloseToPole(leftPass, pdi, pppi);
        closeToPoleLeft.setDuration(211);
        flyToPoleLeft.addTransition(closeToPoleLeft);
        closeToPoleRight = new CloseToPole(rightPass, pdi, pppi);
        closeToPoleRight.setDuration(211);
        flyToPoleRight.addTransition(closeToPoleRight);

        fsm.setInitialState(searchPole);
	}
}
