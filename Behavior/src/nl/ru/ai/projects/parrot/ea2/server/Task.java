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
