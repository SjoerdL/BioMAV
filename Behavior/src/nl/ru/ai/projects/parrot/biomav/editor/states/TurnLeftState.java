package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class TurnLeftState extends State {
  public TurnLeftState(BehaviorArray behaviors) {
    super("Turn left");
    
    addBehavior(behaviors.turnLeftBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
