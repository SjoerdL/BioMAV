package nl.ru.ai.projects.parrot.biomav.editor.transitions;

import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class Delay extends Transition {
  private boolean doReinit = true;
  private long lastTimeStamp = System.currentTimeMillis();
  
  public Delay(BehaviorArray ba, State connectedTo) {
    super(connectedTo, ba.pdi, ba.pppi);
    setDuration(1000);
  }

  @Override
  public boolean fire() {
    if (doReinit) {
      lastTimeStamp = System.currentTimeMillis();
      doReinit = false;
    } else {
      if (System.currentTimeMillis() - lastTimeStamp >= getDuration()) {
        doReinit = true;
        return true;
      }
    }
    return false;
  }
}
