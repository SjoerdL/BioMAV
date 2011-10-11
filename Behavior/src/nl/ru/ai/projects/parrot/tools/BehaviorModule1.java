package nl.ru.ai.projects.parrot.tools;

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
import nl.ru.ai.projects.parrot.transitions.OutOfSight;
import nl.ru.ai.projects.parrot.transitions.PoleFound;
import nl.ru.ai.projects.parrot.transitions.TurnComplete;

public class BehaviorModule1 extends BehaviorModule {
	
	public Behavior initializeDroneBehavior;
	public Behavior hoverBehavior;
	public Transition poleFound;
	public Transition outOfSightRight;
	public Transition turnCompleteRight;
	public Transition outOfSightLeft;	
	public Transition turnCompleteLeft;	
	
	public BehaviorModule1(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
		super(pdi, pppi);
		
    	// Define states
        State searchPole = new State("Search pole");
        State leftPass = new State("Pass left");
        State turnRight = new State("Turn right");
        State rightPass = new State("Pass right");
        State turnLeft = new State("Turn left");

        // Add behaviors to states
        initializeDroneBehavior = new InitializeDrone(pdi, pppi);
        initializeDroneBehavior.setDuration(2500);
        hoverBehavior = new Hover(pdi, pppi);
        hoverBehavior.setDuration(2000);
        Behavior twitterBehavior = new TwitterBehavior(pdi, pppi);
        Behavior searchPoleBehavior = new SearchPole(pdi, pppi);
        searchPoleBehavior.setDuration(200);
        Behavior passLeftBehavior = new PassLeft(pdi, pppi);
        searchPoleBehavior.setDuration(250);
        Behavior passRightBehavior = new PassRight(pdi, pppi);
        passRightBehavior.setDuration(250);
        Behavior turnLeftBehavior = new TurnLeft(pdi, pppi);
        turnLeftBehavior.setDuration(250);
        Behavior turnRightBehavior = new TurnRight(pdi, pppi);
        turnRightBehavior.setDuration(250);
 
        searchPole.setEntryBehavior(initializeDroneBehavior); 	// Put the drone in the air        
        searchPole.addBehavior(searchPoleBehavior); 		// Initial state
        searchPole.setExitBehavior(hoverBehavior);
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
        turnLeft.setExitBehavior(hoverBehavior);
        
        // Add transitions between states
        poleFound = new PoleFound(leftPass, pdi, pppi);
        poleFound.setDuration(15);
        searchPole.addTransition(poleFound);
        outOfSightRight = new OutOfSight(turnRight, pdi, pppi);
        outOfSightRight.setDuration(8);
        leftPass.addTransition(outOfSightRight);
        turnCompleteRight = new TurnComplete(rightPass, pdi, pppi);
        turnCompleteRight.setDuration(195);
        turnRight.addTransition(turnCompleteRight);
        outOfSightLeft = new OutOfSight(turnLeft, pdi, pppi);
        outOfSightLeft.setDuration(8);
        rightPass.addTransition(outOfSightLeft);
        turnCompleteLeft = new TurnComplete(leftPass, pdi, pppi);
        turnCompleteLeft.setDuration(195);
        turnLeft.addTransition(turnCompleteLeft);

        fsm.setInitialState(searchPole);
	}
}
