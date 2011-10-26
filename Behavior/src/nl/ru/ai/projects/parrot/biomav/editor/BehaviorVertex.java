package nl.ru.ai.projects.parrot.biomav.editor;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.security.InvalidParameterException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.biomav.editor.states.FlyLeftState;
import nl.ru.ai.projects.parrot.biomav.editor.states.FlyRightState;
import nl.ru.ai.projects.parrot.biomav.editor.states.PassLeftState;
import nl.ru.ai.projects.parrot.biomav.editor.states.PassRightState;
import nl.ru.ai.projects.parrot.biomav.editor.states.SearchPoleState;
import nl.ru.ai.projects.parrot.biomav.editor.states.StartState;
import nl.ru.ai.projects.parrot.biomav.editor.states.TurnLeftState;
import nl.ru.ai.projects.parrot.biomav.editor.states.TurnRightState;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.StateListener;

public class BehaviorVertex implements ParameterControlInterface, StateListener {
  @SuppressWarnings("rawtypes")
  public static final Map<String, Class> FSM_STATES = new HashMap<String, Class>();
  static {
    FSM_STATES.put("Start", StartState.class);
    FSM_STATES.put("Turn Left", TurnLeftState.class);
    FSM_STATES.put("Turn Right", TurnRightState.class);
    FSM_STATES.put("Fly Left", FlyLeftState.class);
    FSM_STATES.put("Fly Right", FlyRightState.class);
    FSM_STATES.put("Pass left", PassLeftState.class);
    FSM_STATES.put("Pass right", PassRightState.class);
    FSM_STATES.put("Search pole", SearchPoleState.class);
  }
  public static final String FSM_INIT_STATE = "Start";
  
  private String selectedBehaviorState = FSM_INIT_STATE;
  private volatile boolean stateActive = false;
  
  private Set<BehaviorVertexListener> listeners = new HashSet<BehaviorVertexListener>();
  
  public String getSelectedBehavior() {
    return selectedBehaviorState;
  }

  public void setSelectedBehavior(String state) {
    if (!FSM_STATES.containsKey(state)) {
      throw new InvalidParameterException("setSelectedBehavior: " + state);
    }
    stateActive = false;
    selectedBehaviorState = state;
  }
  
  public boolean isActive() {
    return stateActive;
  }
  
  @SuppressWarnings({ "rawtypes", "unchecked" })
  public State initState(BehaviorArray behaviors) {
    Class stateClass = FSM_STATES.get(selectedBehaviorState);
    try {
      Constructor c = stateClass.getConstructor(BehaviorArray.class);
      State result = (State) c.newInstance(behaviors);
      result.addStateListener(this);
      return result;
    } catch (NoSuchMethodException e) {
      e.printStackTrace();
    } catch (SecurityException e) {
      e.printStackTrace();
    } catch (InstantiationException e) {
      e.printStackTrace();
    } catch (IllegalAccessException e) {
      e.printStackTrace();
    } catch (IllegalArgumentException e) {
      e.printStackTrace();
    } catch (InvocationTargetException e) {
      e.printStackTrace();
    }
    return null;
  }

  @Override
  public ParameterTypes[] getParameterTypes() {
    ParameterTypes[] res = {ParameterTypes.OPTIONS}; 
    return res;
  }

  @Override
  public Object[] getParameterOptions(int i) {
    return FSM_STATES.keySet().toArray();
  }

  @Override
  public Object getParameterValue(int i) {
    return getSelectedBehavior();
  }

  @Override
  public void setParameterValue(int i, Object value) {
    setSelectedBehavior(value.toString());
  }

  @Override
  public String[] getParameterNames() {
    String[] res = {"Behavior"}; 
    return res;
  }

  @Override
  public void stateEntered(State state) {
    stateActive = true;

    BehaviorVertexListener[] ls;
    synchronized (listeners) {
      ls = listeners.toArray(new BehaviorVertexListener[0]);
    }
    for (BehaviorVertexListener l : ls) {
      l.behaviorVertexActivityChanged(this);
    }
  }

  @Override
  public void stateLeft(State state) {
    stateActive = false;

    BehaviorVertexListener[] ls;
    synchronized (listeners) {
      ls = listeners.toArray(new BehaviorVertexListener[0]);
    }
    for (BehaviorVertexListener l : ls) {
      l.behaviorVertexActivityChanged(this);
    }
}
  
  public void addBehaviorVertexListener(BehaviorVertexListener l) {
    synchronized (listeners) {
      if (!listeners.contains(l)) {
        listeners.add(l);
      }
    }
  }
}
