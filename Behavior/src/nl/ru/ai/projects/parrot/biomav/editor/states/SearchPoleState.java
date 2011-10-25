package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class SearchPoleState extends State {
  public SearchPoleState(BehaviorArray behaviors) {
    super("Search pole");
    
    addBehavior(behaviors.searchPoleBehavior); 
    setExitBehavior(behaviors.hoverBehavior);
  }
}
