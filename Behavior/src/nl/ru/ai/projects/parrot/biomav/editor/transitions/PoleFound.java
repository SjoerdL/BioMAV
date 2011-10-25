package nl.ru.ai.projects.parrot.biomav.editor.transitions;

import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.fsm.State;

public class PoleFound extends nl.ru.ai.projects.parrot.transitions.PoleFound {
  public PoleFound(BehaviorArray ba, State connectedTo) {
    super(connectedTo, ba.pdi, ba.pppi);
    setDuration(20);
  }
}
