package nl.ru.ai.projects.parrot.ea2.server;

import java.util.HashMap;
import java.util.Map;

public class TaskParameters {
  private Map<String, String> map = new HashMap<String, String>();
  
  public TaskParameters() {
  }
  
  public TaskParameters(Map<String, String> parameterMap) {
    this.map.putAll(parameterMap);
  }
  
  public TaskParameters(String parameterData) {
    String[] lines = parameterData.split("\n");
    for (String line : lines) {
    	System.out.println(line);
      String[] splits = line.split(":", 2);
      map.put(splits[0], splits[1]);
    }
  }
  
  public Map<String, String> getMap() {
    return map;
  }
  
  @Override
  public String toString() {
    String result = "";
    
    for (Map.Entry<String, String> entry : map.entrySet()) {
      result += entry.getKey() + " : " + entry.getValue() + "\n";
    }
    
    return result;
  }
}
