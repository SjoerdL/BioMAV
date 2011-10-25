package nl.ru.ai.projects.parrot.biomav.editor.transitions;

import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.fsm.State;

public class OutOfSight extends nl.ru.ai.projects.parrot.transitions.OutOfSight {
  public OutOfSight(BehaviorArray ba, State connectedTo) {
    super(connectedTo, ba.pdi, ba.pppi);
    setDuration(5);
  }
}
