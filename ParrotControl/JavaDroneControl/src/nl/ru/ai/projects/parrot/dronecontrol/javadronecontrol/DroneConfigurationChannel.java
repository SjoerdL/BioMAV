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
package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol;

/**
 * <p>
 * This class allows to send a variety of drone configuration commands.
 * Drone configurations are static parameters like turn speed, maximum
 * height or the navdata packet type that is sent. Currently, however,
 * only the navdata packet type can be set using this class. More 
 * commands still need to be implemented.
 * </p>
 * 
 * <p>
 * To send the commands the class uses an intitialized instance of the
 * {@link ATControlCommandInterface}
 * </p>
 * 
 * TODO:
 *   - ATM this is open loop control - should be closed loop control
 *   - Add more configurations that can be sent
 * 
 * @author Paul Konstantin Gerke
 *
 */
public class DroneConfigurationChannel {
  private ATControlCommandInterface controlInterface;
  
  /**
   * Creates a new drone configuration channel that uses the given 
   * {@link ATControlCommandInterface} to communicate with the drone. 
   * 
   * @param controlInterface
   *   {@link ATControlCommandInterface} used to communicate with the
   *   drone.
   */
  public DroneConfigurationChannel(ATControlCommandInterface controlInterface) {
    this.controlInterface = controlInterface;
  }
  
  /**
   * Sets the navdata packet type sent by the drone.
   * 
   * @param demo
   *   True means that the drone sends the smaller demo navdata packages,
   *   false means that the drone sends the full debug packages. 
   */
  public void setNavdataType(boolean demo) {
    if (demo) {
      controlInterface.sendSetOption("general:navdata_demo", "TRUE");
    } else {
      controlInterface.sendSetOption("general:navdata_demo", "FALSE");
    }
  }
}
