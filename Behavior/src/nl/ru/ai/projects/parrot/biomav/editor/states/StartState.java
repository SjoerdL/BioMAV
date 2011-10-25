package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class StartState extends State {
  public StartState(BehaviorArray behaviors) {
    super("Start");
    
    addBehavior(behaviors.initializeDroneBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
