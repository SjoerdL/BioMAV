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
