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

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.security.InvalidParameterException;
import java.util.HashMap;
import java.util.Map;

import nl.ru.ai.projects.parrot.biomav.editor.states.BehaviorArray;
import nl.ru.ai.projects.parrot.biomav.editor.transitions.Delay;
import nl.ru.ai.projects.parrot.biomav.editor.transitions.OutOfSight;
import nl.ru.ai.projects.parrot.biomav.editor.transitions.PoleFound;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;

public class TransitionEdge implements ParameterControlInterface {
  @SuppressWarnings("rawtypes")
  public static final Map<String, Class> FSM_TRANSITIONS = new HashMap<String, Class>();
  static {
    FSM_TRANSITIONS.put("Pole found", PoleFound.class);
    FSM_TRANSITIONS.put("Out of sight", OutOfSight.class);
    FSM_TRANSITIONS.put("Close to pole", OutOfSight.class);
    FSM_TRANSITIONS.put("Delay", Delay.class);
  }
  
  private String selectedTransition = FSM_TRANSITIONS.keySet().toArray()[0].toString(); // Select something
  
  public String getTransition() {
    return selectedTransition;
  }
  
  public void setTransition(String t) {
    if (!FSM_TRANSITIONS.containsKey(t)) {
      throw new InvalidParameterException("setTransition: " + t);
    }
    selectedTransition = t;
  }
  
  @SuppressWarnings({ "unchecked", "rawtypes" })
  public Transition initTransition(BehaviorArray ba, State from, State to) {
    Class transitionClass = FSM_TRANSITIONS.get(selectedTransition);
    try {
      Constructor c = transitionClass.getConstructor(BehaviorArray.class, State.class);
      Transition result = (Transition) c.newInstance(ba, to);
      from.addTransition(result);
      
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
  public String[] getParameterNames() {
    String[] result = {"Transition"};
    return result;
  }

  @Override
  public ParameterTypes[] getParameterTypes() {
    ParameterTypes[] result = {ParameterTypes.OPTIONS}; 
    return result;
  }

  @Override
  public Object[] getParameterOptions(int i) {
    return FSM_TRANSITIONS.keySet().toArray();
  }

  @Override
  public Object getParameterValue(int i) {
    return getTransition();
  }

  @Override
  public void setParameterValue(int i, Object value) {
    setTransition(value.toString());
  }
}
