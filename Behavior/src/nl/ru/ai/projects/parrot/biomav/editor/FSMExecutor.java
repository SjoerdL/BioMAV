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
package nl.ru.ai.projects.parrot.biomav.editor;

import nl.ru.ai.projects.parrot.fsm.FSMController;

public class FSMExecutor {
  private FSMController controller;
  
  private Thread execThread = null;
  
  public FSMExecutor(FSMController fsmController) {
    controller = fsmController;
  }
  
  public void start() {
    if (execThread == null) {
      execThread = new Thread() {
        @Override
        public void run() {
          try {
            controller.execute();
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
        }
      };
      
      execThread.setDaemon(true);
      execThread.start();
    }
  }
  
  public void stop() {
    if (execThread != null) {
      execThread.interrupt();
      try {
        execThread.join();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      execThread = null;
    }
  }
}
