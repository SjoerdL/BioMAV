/*
    This file is part of the BioMAV project.

    The BioMAV project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The BioMAV project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The BioMAV project. If not, see <http://www.gnu.org/licenses/>.
*/
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
