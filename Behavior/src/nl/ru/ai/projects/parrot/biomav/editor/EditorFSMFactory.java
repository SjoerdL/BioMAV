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

import java.util.ArrayList;
import java.util.List;

import edu.uci.ics.jung.graph.DirectedGraph;
import edu.uci.ics.jung.graph.util.Pair;
import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.fsm.FSMController;
import nl.ru.ai.projects.parrot.fsm.State;

public class EditorFSMFactory {
  public static class InvalidGraphStructureException extends Exception {
    private static final long serialVersionUID = 2656345213473859743L;
    
    public InvalidGraphStructureException(String msg) {
      super(msg);
    }
  }
  
  private BehaviorArray behaviorArray;
  
  public EditorFSMFactory(Simulator simulator) {
    behaviorArray = new BehaviorArray(simulator.getDrone(0), simulator.getDrone(0));
  }
  
  public FSMController createFiniteStateMachine(DirectedGraph<BehaviorVertex, TransitionEdge> graph) throws InvalidGraphStructureException {
    List<BehaviorVertex> vertices = new ArrayList<BehaviorVertex>();
    vertices.addAll(graph.getVertices());
    State[] states = new State[vertices.size()];
    
    int startStateIndex = -1; 
    for (int i = 0; i < vertices.size(); i++) {
      // Find start state
      if (vertices.get(i).getSelectedBehavior().equals(BehaviorVertex.FSM_INIT_STATE)) {
        if (startStateIndex >= 0) {
          throw new InvalidGraphStructureException("Multiple start states found!");
        }
        startStateIndex = i;
      }
      
      states[i] = vertices.get(i).initState(behaviorArray);
    }
    // Control graph on errors
    if (startStateIndex < 0) {
      throw new InvalidGraphStructureException("No start state found!");
    }
    
    for (TransitionEdge edge : graph.getEdges()) {
      Pair<BehaviorVertex> edgeEndpoints = graph.getEndpoints(edge);
      
      State fromState = states[vertices.indexOf(edgeEndpoints.getFirst())];
      State toState = states[vertices.indexOf(edgeEndpoints.getSecond())];
      
      edge.initTransition(behaviorArray, fromState, toState);
    }
    
    FSMController fsmController = new FSMController();
    fsmController.setInitialState(states[startStateIndex]);
    
    return fsmController; 
  }
}
