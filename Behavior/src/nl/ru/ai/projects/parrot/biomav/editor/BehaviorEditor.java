package nl.ru.ai.projects.parrot.biomav.editor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.InputEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.util.HashSet;
import java.util.Set;

import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.apache.commons.collections15.Factory;
import org.apache.commons.collections15.Predicate;
import org.apache.commons.collections15.Transformer;

import edu.uci.ics.jung.algorithms.layout.FRLayout2;
import edu.uci.ics.jung.graph.DelegateForest;
import edu.uci.ics.jung.graph.DirectedOrderedSparseMultigraph;
import edu.uci.ics.jung.graph.DirectedSparseGraph;
import edu.uci.ics.jung.graph.SparseMultigraph;
import edu.uci.ics.jung.visualization.VisualizationViewer;
import edu.uci.ics.jung.visualization.control.AbstractGraphMousePlugin;
import edu.uci.ics.jung.visualization.control.DefaultModalGraphMouse;
import edu.uci.ics.jung.visualization.control.EditingGraphMousePlugin;
import edu.uci.ics.jung.visualization.control.GraphMouseListener;
import edu.uci.ics.jung.visualization.control.ModalGraphMouse;
import edu.uci.ics.jung.visualization.control.PickingGraphMousePlugin;
import edu.uci.ics.jung.visualization.control.PluggableGraphMouse;
import edu.uci.ics.jung.visualization.control.ViewTranslatingGraphMousePlugin;
import edu.uci.ics.jung.visualization.renderers.DefaultVertexLabelRenderer;
import edu.uci.ics.jung.visualization.renderers.GradientVertexRenderer;
import edu.uci.ics.jung.visualization.renderers.VertexLabelAsShapeRenderer;
import edu.uci.ics.jung.visualization.util.PredicatedParallelEdgeIndexFunction;

/**
 * Editor component that allows editing a finite state machine. The
 * component is a JUNG-based graph editor that can directly manipulate
 * behavior graphs of the finite state machine that controls the drone.  
 * 
 * @author Paul Konstantin Gerke
 *
 */
public class BehaviorEditor extends JPanel {
  private static final long serialVersionUID = 690046335487721939L;

  private DirectedOrderedSparseMultigraph<BehaviorVertex, TransitionEdge> graph = new DirectedOrderedSparseMultigraph<BehaviorVertex, TransitionEdge>();
  
  private VisualizationViewer<BehaviorVertex, TransitionEdge> graphViewer = new VisualizationViewer<BehaviorVertex, TransitionEdge>(new FRLayout2<BehaviorVertex, TransitionEdge>(graph));

  private PluggableGraphMouse graphMouse = new PluggableGraphMouse();
  private EditingGraphMousePlugin<BehaviorVertex, TransitionEdge> graphMouseEditPlugin = new EditingGraphMousePlugin<BehaviorVertex, TransitionEdge>(InputEvent.CTRL_MASK, new Factory<BehaviorVertex>() {
      @Override
      public BehaviorVertex create() {
        return new BehaviorVertex();
      }
    }, new Factory<TransitionEdge>() {
      @Override
      public TransitionEdge create() {
        return new TransitionEdge();
      }
    });
  private ViewTranslatingGraphMousePlugin graphMouseTranslatePlugin = new ViewTranslatingGraphMousePlugin(MouseEvent.BUTTON3_MASK);
  private PickingGraphMousePlugin<BehaviorVertex, TransitionEdge> graphMousePickPlugin = new PickingGraphMousePlugin<BehaviorVertex, TransitionEdge>();

  private JPanel pcInterfacePanel = new JPanel();
  private JPanel pcInterfacePanelContents = new JPanel();
  private ParameterControlInterface selectedPCInterface = null;
  
  private void setNewSelectedParameterControlInterface(ParameterControlInterface pcInterface) {
    pcInterfacePanelContents.removeAll();
    selectedPCInterface = null;
    
    if (pcInterface != null) {
      selectedPCInterface = pcInterface;
      final ParameterControlInterface fpcInterface = pcInterface;
      
      String[] parameterNames = pcInterface.getParameterNames();
      ParameterControlInterface.ParameterTypes[] parameterTypes = pcInterface.getParameterTypes();
      
      for (int i = 0; i < parameterTypes.length; i++) {
        JLabel label = new JLabel(parameterNames[i]);
        pcInterfacePanelContents.add(label);
        
        final int index = i;
        switch (parameterTypes[i]) {
          case OPTIONS:
            final JComboBox<Object> optionComboBox = new JComboBox<Object>(pcInterface.getParameterOptions(i));
            optionComboBox.setEditable(false);
            optionComboBox.setSelectedItem(pcInterface.getParameterValue(i));
            
            optionComboBox.addActionListener(new ActionListener() {
              @Override
              public void actionPerformed(ActionEvent e) {
                fpcInterface.setParameterValue(index, optionComboBox.getSelectedItem());
                repaint();
              }
            });
            
            pcInterfacePanelContents.add(optionComboBox);
            
            break;
        }
      }
    }

    pcInterfacePanel.validate();
    pcInterfacePanel.repaint();
  }
  
  public BehaviorEditor() {
    ///////// Graph stuff
    graphViewer.addKeyListener(new KeyListener() {
      @Override
      public void keyTyped(KeyEvent e) {
      }
      
      @Override
      public void keyReleased(KeyEvent e) {
      }
      
      @Override
      public void keyPressed(KeyEvent e) {
        if (e.getKeyCode() == KeyEvent.VK_DELETE) {
          for (TransitionEdge edge : graphViewer.getPickedEdgeState().getPicked()) {
            graph.removeEdge(edge);
          }
          for (BehaviorVertex vertex : graphViewer.getPickedVertexState().getPicked()) {
            graph.removeVertex(vertex);
          }
          e.consume();
          repaint();
        }
      }
    });
    
    graphViewer.setBackground(Color.GRAY);
    
    graphViewer.setGraphMouse(graphMouse);
    graphMouse.add(graphMouseEditPlugin); 
    graphMouse.add(graphMouseTranslatePlugin);
    graphMouse.add(graphMousePickPlugin);

    VertexLabelAsShapeRenderer<BehaviorVertex, TransitionEdge> renderer = new VertexLabelAsShapeRenderer<BehaviorVertex, TransitionEdge>(graphViewer.getRenderContext());
    graphViewer.getRenderContext().setVertexLabelTransformer(new Transformer<BehaviorVertex, String>() {
      @Override
      public String transform(BehaviorVertex v) {
        return v.getSelectedBehavior();
      }
    });
    graphViewer.getRenderContext().setEdgeLabelTransformer(new Transformer<TransitionEdge, String>() {
      @Override
      public String transform(TransitionEdge e) {
        return "Edge: " + e.hashCode();
      }
    });
    graphViewer.getRenderContext().setVertexShapeTransformer(renderer);
    graphViewer.getRenderer().setVertexLabelRenderer(renderer);
    
    graphViewer.getPickedVertexState().addItemListener(new ItemListener() {
      @Override
      public void itemStateChanged(ItemEvent e) {
        BehaviorVertex vertex = (BehaviorVertex) e.getItem();
        if (e.getStateChange() == ItemEvent.SELECTED) {
          setNewSelectedParameterControlInterface(vertex);
        }
        if (e.getStateChange() == ItemEvent.DESELECTED) {
          if (vertex == selectedPCInterface) {
            setNewSelectedParameterControlInterface(null);
          }
        }
      }
    });
    
    this.addComponentListener(new ComponentListener() {
      @Override
      public void componentShown(ComponentEvent e) {
      }
      
      @Override
      public void componentResized(ComponentEvent e) {
        graphViewer.getGraphLayout().setSize(graphViewer.getSize());
      }
      
      @Override
      public void componentMoved(ComponentEvent e) {
      }
      
      @Override
      public void componentHidden(ComponentEvent e) {
      }
    });
    
    ///////////// Side panel (parameter control)
    pcInterfacePanel.setPreferredSize(new Dimension(300, 0));
    pcInterfacePanel.setLayout(new BorderLayout());
    
    pcInterfacePanelContents.setLayout(new GridLayout(0, 2, 0, 5));
    pcInterfacePanel.add(pcInterfacePanelContents, BorderLayout.NORTH);
    
    ///////////// [this]
    
    setLayout(new BorderLayout(10, 10));
    
    add(graphViewer, BorderLayout.CENTER);
    add(pcInterfacePanel, BorderLayout.EAST);
  }

  public static void main(String[] args) {
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        JFrame mainFrame = new JFrame("Behavior Editor Test");
        
        BehaviorEditor be = new BehaviorEditor();
        mainFrame.add(be);
        
        mainFrame.pack();
        mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        mainFrame.setVisible(true);
      }
    });
  }
}
