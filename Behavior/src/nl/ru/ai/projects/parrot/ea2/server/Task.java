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

public class Task {
  public static final long TASK_TIMEOUT = 10 * 60 * 1000; // Milliseconds
  public static final String RESULT_FINISHED_STRING = "DONE";
  
  private TaskParameters parameters;
  private long computationStartTime = -1;
  
  private String resultString = "";
  private boolean resultFinished = false;
  
  public Task(TaskParameters parameters) {
    this.parameters = parameters;
  }
  
  public TaskParameters getTaskParameters() {
    return parameters;
  }
  
  public synchronized void markComputationStart() {
    if (!isComputing() && !hasResult()) {
      resultString = "";
      computationStartTime = System.currentTimeMillis();
    }
  }
  
  public synchronized boolean isComputing() {
    return !hasResult() && (System.currentTimeMillis() - computationStartTime < TASK_TIMEOUT);
  }
  
  public synchronized boolean hasResult() {
    return resultFinished;
  }

  public synchronized void pushResultString(String s) {
    if (RESULT_FINISHED_STRING.equals(s)) {
      resultFinished = true;
      notifyAll();
    } else {
      resultString += s + "\n";
    }
  }
  
  public synchronized String getResult() {
    if (hasResult()) {
      return resultString;
    }
    return null;
  }
}
