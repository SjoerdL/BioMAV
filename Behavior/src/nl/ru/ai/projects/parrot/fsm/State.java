package nl.ru.ai.projects.parrot.fsm;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.tools.PositionLogger;
import nl.ru.ai.projects.parrot.tools.Writer;

/**
 * Defines a state in a Finite State Machine
 * 
 * @author sjoerdlagarde
 */
public class State {

  private String name;

  private ArrayList<Behavior> behaviors;

  private ArrayList<Transition> transitions;

  public static HashSet<State> resetStates = new HashSet<State>(20);

  private Behavior entryBehavior;

  private Behavior exitBehavior;

  private boolean isActive;

  private boolean isFinalState;

  private static boolean logDronePosition;

  public static boolean outputCurrentState;

  private static int writeCounter;

  // State listeners to allow observation of the states of a FSM
  private Set<StateListener> stateListeners = new HashSet<StateListener>();

  public State(String name) {
    this.name = name;

    behaviors = new ArrayList<Behavior>();
    transitions = new ArrayList<Transition>();

    entryBehavior = null;
    exitBehavior = null;

    isActive = false;
    isFinalState = false;

    writeCounter = 0;
  }

  /**
   * The name of the state
   * 
   * @return The name of the state
   */
  public String getName() {
    return name;
  }

  /**
   * Sets whether or not to log the position of the drone to a text file
   * 
   * @param value
   *          True when the position of the drone should be logged, false
   *          otherwise
   */
  public static void setLogDronePosition(boolean value) {
    State.logDronePosition = value;
  }

  /**
   * Sets whether or not to output the name of the state when that state becomes
   * active
   * 
   * @param value
   *          True when to output the current state, false otherwise
   */
  public static void setOutputCurrentState(boolean value) {
    State.outputCurrentState = value;
  }

  /**
   * Adds a new behavior to the state
   * 
   * @param behavior
   *          The behavior to add
   */
  public void addBehavior(Behavior behavior) {
    behaviors.add(behavior);
  }

  /**
   * Sets the behavior to be executed on entry of the state
   * 
   * @param behavior
   *          The entry behavior
   */
  public void setEntryBehavior(Behavior behavior) {
    this.entryBehavior = behavior;
  }

  /**
   * Sets the behavior to be executed on exit of the state
   * 
   * @param behavior
   *          The exit behavior
   */
  public void setExitBehavior(Behavior behavior) {
    this.exitBehavior = behavior;
  }

  /**
   * Sets whether or not the state is a final state (i.e. no transitions will
   * occur)
   * 
   * @param isFinal
   *          True when the state is a final state, false otherwise (default is
   *          false)
   */
  public void setFinalState(boolean isFinal) {
    this.isFinalState = isFinal;
  }

  /**
   * Return whether or not the state is a final state
   * 
   * @return True when the state is a final state, false otherwise
   */
  public boolean isFinalState() {
    return isFinalState;
  }

  /**
   * Adds a transition to the list of transitions
   * 
   * @param transition
   *          The transition to add
   */
  public void addTransition(Transition transition) {
    transitions.add(transition);
  }

  /**
   * Resets the state by resettings its associated behaviors and transitions
   */
  public void reset() {
    isActive = false;
    writeCounter = 0;
    for (int i = 0; i < behaviors.size(); i++)
      // Reset behaviors
      behaviors.get(i).reset();
    for (int j = 0; j < transitions.size(); j++) { // Reset transitions
      Transition transition = transitions.get(j);
      transition.reset();
      if (resetStates.add(transition.getConnectedState())) // Avoid cyclic
                                                           // structures
        transition.getConnectedState().reset();
    }
  }

  /**
   * Clears the hashset that contains the states that are already reset
   */
  public void clearResetStates() {
    resetStates.clear();
  }

  /**
   * Runs the behaviors in a state Note that each behavior needs to decide for
   * itself whether or not to execute
   * 
   * @throws InterruptedException
   */
  public State run() throws InterruptedException {

    if (State.logDronePosition) {
      double[] position = Simulator.getInstance().getDrone(0)
          .getDronePosition();
      PositionLogger.addPosition(position);

      // Replace decimal signs as to make text file easy accessible to Excel
      String pos0 = Double.toString(position[0]).replace('.', ',');
      String pos1 = Double.toString(position[1]).replace('.', ',');
      Writer.getInstance().write(pos0 + ";" + pos1);
      // Matlab
      // String pos0 = Double.toString(position[0]);
      // String pos1 = Double.toString(position[1]);
      // Writer.getInstance().write(pos0 + "," + pos1 + ";");
      writeCounter++;
      if (writeCounter == 10) {
        Writer.getInstance().flush();
        writeCounter = 0;
      }
    }

    if (!isActive) { // Perform entry-action
      // Notify listeners about state entry
      for (StateListener sl : stateListeners) {
        sl.stateEntered(this);
      }

      if (outputCurrentState)
        System.out.println("Entered State: " + name);
      isActive = true;
      if (entryBehavior != null)
        entryBehavior.run();
    }

    // Transition to other state?
    for (int i = 0; i < transitions.size(); i++) {
      Transition currentTransition = transitions.get(i);
      if (currentTransition.fire()) {
        // Notify listeners about state exit
        for (StateListener sl : stateListeners) {
          sl.stateLeft(this);
        }

        isActive = false;
        if (exitBehavior != null)
          exitBehavior.run();

        currentTransition.run(); // Run transition behavior
        return currentTransition.getConnectedState();
      }
    }

    // Run behaviors
    for (int i = 0; i < behaviors.size(); i++) {
      Behavior currentBehavior = behaviors.get(i);
      if (currentBehavior.fire())
        currentBehavior.run();
    }

    return this;
  }

  /**
   * Adds a new state listener that is notified when anything regarding 
   * this state changes.
   *  
   * @param sl
   *   The state listener that should be added
   */
  public void addStateListener(StateListener sl) {
    if (!stateListeners.contains(sl)) {
      stateListeners.add(sl);
    }
  }

  /**
   * Removes a state listener from the listener set.
   * 
   * @param sl
   *   State listener to remove.
   * @return
   *   Returns true if the listener could be successfully removed
   */
  public boolean removeStateListener(StateListener sl) {
    return stateListeners.remove(sl);
  }
}
