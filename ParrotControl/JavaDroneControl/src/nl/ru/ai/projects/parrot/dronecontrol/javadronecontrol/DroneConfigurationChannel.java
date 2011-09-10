package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol;

/**
 * TODO:
 *   - ATM this is open loop control - should be closed loop control
 * 
 * @author paul
 *
 */
public class DroneConfigurationChannel {
  private ATControlCommandInterface controlInterface;
  
  public DroneConfigurationChannel(ATControlCommandInterface controlInterface) {
    this.controlInterface = controlInterface;
  }
  
  public void setNavdataType(boolean demo) {
    if (demo) {
      controlInterface.sendSetOption("general:navdata_demo", "TRUE");
    } else {
      controlInterface.sendSetOption("general:navdata_demo", "FALSE");
    }
  }
}
