package nl.ru.ai.projects.parrot.dronecontrol.groundstation;

public interface DroneInputDevice {
  public float getPitch();
  public float getYaw();
  public float getRoll();
  public float getHeight();
  
  public boolean getStartMode();
  
  public boolean getEmergencyMode();
}
