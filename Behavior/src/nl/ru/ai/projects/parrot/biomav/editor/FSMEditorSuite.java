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

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import nl.ru.ai.projects.parrot.biomav.editor.EditorFSMFactory.InvalidGraphStructureException;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.fsm.FSMController;

public class FSMEditorSuite extends JFrame {
  private static final long serialVersionUID = -3875638887609936627L;
  
  private BehaviorEditor editor = new BehaviorEditor();
  
  private JButton startButton = new JButton("Start");
  private JButton stopButton = new JButton("Stop");
  
  private EditorFSMFactory fsmFactory = new EditorFSMFactory(Simulator.getInstance());
  private FSMExecutor executor = null;
  
  public FSMEditorSuite() {
    super("BioMAV - Finite State Machine Editor");
    
    setLayout(new BorderLayout(10, 10));
    add(editor, BorderLayout.CENTER);
    
    {
      JPanel finiteStateMachineControl = new JPanel();
      JPanel subPanel = new JPanel();
      
      finiteStateMachineControl.setLayout(new BorderLayout());
      finiteStateMachineControl.add(new JLabel("Finit State Machine Control"), BorderLayout.NORTH);
      finiteStateMachineControl.add(subPanel, BorderLayout.WEST);
      
      subPanel.setLayout(new GridLayout(1, 0));
      subPanel.add(startButton);
      subPanel.add(stopButton);
      stopButton.setEnabled(false);
      
      add(finiteStateMachineControl, BorderLayout.SOUTH);
    }
    
    startButton.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        FSMController fsmController;
        try {
          fsmController = fsmFactory.createFiniteStateMachine(editor.getGraph());
          executor = new FSMExecutor(fsmController);
          executor.start();
          startButton.setEnabled(false);
          stopButton.setEnabled(true);
        } catch (InvalidGraphStructureException e1) {
          JOptionPane.showMessageDialog(FSMEditorSuite.this, "Error: " + e1.getMessage());
        }
      }
    });
    stopButton.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        executor.stop();
        executor = null;
        startButton.setEnabled(true);
        stopButton.setEnabled(false);
      }
    });
    
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
