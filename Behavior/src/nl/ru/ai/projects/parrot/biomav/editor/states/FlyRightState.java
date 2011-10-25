package nl.ru.ai.projects.parrot.biomav.editor.states;

import nl.ru.ai.projects.parrot.fsm.State;

public class FlyRightState extends State {
  public FlyRightState(BehaviorArray behaviors) {
    super("Fly right");
    
    addBehavior(behaviors.flyToPoleRightBehavior);           
    setExitBehavior(behaviors.hoverBehavior);        
  }
}
