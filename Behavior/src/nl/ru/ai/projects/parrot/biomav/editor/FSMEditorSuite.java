package nl.ru.ai.projects.parrot.biomav.editor;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

public class FSMEditorSuite extends JFrame {
  private static final long serialVersionUID = -3875638887609936627L;
  
  private BehaviorEditor editor = new BehaviorEditor();
  
  public FSMEditorSuite() {
    super("BioMAV - Finite State Machine Editor");
    
    add(editor);
    
    pack();
  }

  public static void main(String[] args) {
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        // HACK: Cheaty shit! - Fix bug!
        try {
          Thread.sleep(1000);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
        
        Simulator.waitReady();
        
        Simulator sim = Simulator.getInstance();
        sim.activateRendering(true);
        
        FSMEditorSuite editorSuite = new FSMEditorSuite();
        editorSuite.setVisible(true);
        editorSuite.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      }
    });
    
    Simulator.runSimulator();
  }
}
