package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class PassLeftState extends State {
  public PassLeftState(BehaviorArray behaviors) {
    super("Pass left");
    
    addBehavior(behaviors.passLeftBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
