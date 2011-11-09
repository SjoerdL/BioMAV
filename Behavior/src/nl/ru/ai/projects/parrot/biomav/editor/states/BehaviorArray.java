package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.behaviors.FlyTowardsPole;
import nl.ru.ai.projects.parrot.behaviors.Hover;
import nl.ru.ai.projects.parrot.behaviors.InitializeDrone;
import nl.ru.ai.projects.parrot.behaviors.PassLeft;
import nl.ru.ai.projects.parrot.behaviors.PassRight;
import nl.ru.ai.projects.parrot.behaviors.SearchPole;
import nl.ru.ai.projects.parrot.behaviors.TurnLeft;
import nl.ru.ai.projects.parrot.behaviors.TurnRight;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;

public class BehaviorArray {
  public Behavior initializeDroneBehavior, hoverBehavior, searchPoleBehavior, 
    passLeftBehavior, passRightBehavior, turnLeftBehavior, turnRightBehavior, 
    flyToPoleLeftBehavior, flyToPoleRightBehavior;
  
  public ParrotDroneInterface pdi;
  public PolePositionPollInterface pppi;
  
  public BehaviorArray(ParrotDroneInterface pdi, PolePositionPollInterface pppi) {
    this.pdi = pdi;
    this.pppi = pppi;
    
    initializeDroneBehavior = new InitializeDrone(pdi, pppi);
    initializeDroneBehavior.setDuration(2500);
    hoverBehavior = new Hover(pdi, pppi);
    hoverBehavior.setDuration(1500);
    searchPoleBehavior = new SearchPole(pdi, pppi);
    searchPoleBehavior.setDuration(200);
    passLeftBehavior = new PassLeft(pdi, pppi);
    passLeftBehavior.setDuration(250);
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
  }
}
