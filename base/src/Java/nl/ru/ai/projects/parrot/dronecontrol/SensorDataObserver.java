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
package nl.ru.ai.projects.parrot.dronecontrol;

/**
 * This interface can be used for synchronous sensor data retrieval with the
 * {@link SensoryDataInterface}. The defined function is called whenever new
 * data is received by the sensor interface, and no new data is received for the
 * duration of the the call to newSensorData.
 * 
 * @author Paul Konstantin Gerke
 *
 */
public interface SensorDataObserver {
  /**
   * Called by {@link SensoryDataInterface} to notify the observer that
   * a new packet of sensor data has been received. This function is called 
   * asynchronously by the simulation thread and should exit as fast as possible. 
   * 
   * @param sender
   *   SensoryDataInterface which called the callback and just received new data.
   */
  public void newSensorData(SensoryDataInterface sender);
}
