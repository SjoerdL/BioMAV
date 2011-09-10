package nl.ru.ai.projects.parrot.dronecontrol;

public interface SensorDataObserver {
  /**
   * Called by {@link SensoryDataInterface} to notify the observer that
   * a new packet of sensor data has been received. This function is called 
   * asynchronously by the simulation thread and should exit as fast as possible. 
   * 
   * @param sender
   */
  public void newSensorData(SensoryDataInterface sender);
}
