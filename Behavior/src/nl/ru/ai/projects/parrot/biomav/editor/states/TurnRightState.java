package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class TurnRightState extends State {
  public TurnRightState(BehaviorArray behaviors) {
    super("Turn right");
    
    addBehavior(behaviors.turnRightBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
