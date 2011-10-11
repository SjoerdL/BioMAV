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

public interface SensoryDataInterface {
  /**
   * Retrieves the time stamp of the last sensory data snapshot
   * that has been taken.
   * @return
   *   Timestamp in milliseconds (local system time) 
   */
  public long getLastSensoryDataTimeStamp();
  
  /**
   * (Gyro information)
   * 
   * Returns how many radians the drone has turned since the last call
   * to getCurrentRotationVector.  
   * @return
   *   A 3-component double array representing a vector. The X, Y, 
   *   and Z component give the accumulated angle (radians) that 
   *   the drone has turned around the X, Y, and Z axis since the
   *   last call of this function. 
   */
  public double[] getCurrentRotationVector();
  

  /**
   * Returns the ground speed of the drone - use with care since
   * the ground speed data of the real drone seems to be especially
   * noisy and dependent on the texture of the ground.  
   * @return
   *   A 3-component double array representing a vector. The X, Y, 
   *   and Z component give the movement speed in m/s. Thereby X 
   *   represents forward movement (positive = forward) and
   *   Y movement sideways (positive = right). Z is always zero.
   */
  public double[] getGroundSpeed();

  /**
   * Add a new sensor data observer to the drone that is called when
   * new sensor data has been received.
   * 
   * @param observer
   */
  public void addSensorDataObserver(SensorDataObserver observer);
  
  /**
   * Does a sleep that is synchronized with the drone timer (especially useful
   * for the simulator)
   * 
   * @param milliseconds
   */
  public void droneSleep(long milliseconds) throws InterruptedException;
}
