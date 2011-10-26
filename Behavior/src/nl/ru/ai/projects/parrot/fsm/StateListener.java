package nl.ru.ai.projects.parrot.fsm;

public interface StateListener {
  public void stateEntered(State state);
  public void stateLeft(State state);
}
