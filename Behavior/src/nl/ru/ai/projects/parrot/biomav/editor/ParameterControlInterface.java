package nl.ru.ai.projects.parrot.biomav.editor;

public interface ParameterControlInterface {
  public enum ParameterTypes {OPTIONS};
  
  public String[] getParameterNames();
  public ParameterTypes[] getParameterTypes();
  public Object[] getParameterOptions(int i);
  public Object getParameterValue(int i);
  public void setParameterValue(int i, Object value);
}
