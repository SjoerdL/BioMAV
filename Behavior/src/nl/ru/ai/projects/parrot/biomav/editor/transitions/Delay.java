package nl.ru.ai.projects.parrot.biomav.editor.transitions;

import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class Delay extends Transition {
  public Delay(BehaviorArray ba, State connectedTo) {
    super(connectedTo, ba.pdi, ba.pppi);
  }

  @Override
  public boolean fire() {
    try {
      droneInterface.droneSleep(1000);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
    return true;
  }
}
