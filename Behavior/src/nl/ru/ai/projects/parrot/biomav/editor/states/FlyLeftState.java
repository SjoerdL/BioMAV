package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class FlyLeftState extends State {
  public FlyLeftState(BehaviorArray behaviors) {
    super("Fly left");
    
    addBehavior(behaviors.flyToPoleLeftBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
