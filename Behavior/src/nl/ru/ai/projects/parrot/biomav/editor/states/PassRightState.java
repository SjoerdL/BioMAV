package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class PassRightState extends State {
  public PassRightState(BehaviorArray behaviors) {
    super("Pass right");
    
    addBehavior(behaviors.passRightBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
