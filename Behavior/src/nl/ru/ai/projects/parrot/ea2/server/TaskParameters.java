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
