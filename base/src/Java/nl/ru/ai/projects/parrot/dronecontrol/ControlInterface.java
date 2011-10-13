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

public interface ControlInterface {
  public enum FlyingState {LANDED, FLYING, EMERGENCY, DISCONNECTED, BATTERY_LOW};
  
  /**
   * Retrieves the current status of the drone. The flying state indicates the current 
   * state of the drone, if it is flying, landed or in a certain error state.
   *  
   * @return
   *   The flying state of the drone
   */
  public FlyingState getFlyingState();
  
  /**
   * Lets the drone take off if the flying state is not EMERGENCY or DISCONNECTED
   */
  public void takeoff();
  
  /**
   * Lets the drone land if the flying state is flying
   */
  public void land();
  
  /**
   * Sets the emergency flag of the drone leading to an immediate stop of all motors.
   */
  public void setEmergencyFlag();
  
  /**
   * Clears the emergency flag of the drone if the flying state is EMERGENCY
   */
  public void clearEmergencyFlag();
  
  /**
   * Lets the drone fly forward or backward if the drone flying state if FLYING
   * 
   * @param speed
   *   This parameter indicates how fast the drone steers forward or backward. The 
   *   parameter is clamped to the interval [-1, 1] where negative values lead
   *   to the drone moving backward with the given speed, and positive values lead
   *   to the drone moving forward.   
   */
  public void flyForward(double speed);
  
  /**
   * Lets the drone fly left or right if the drone flying state if FLYING
   * 
   * @param speed
   *   Gives the speed with that the drone moves left or right. Negative values let the
   *   drone move left and positive values let the drone move right. The value is clamped
   *   to the interval [-1, 1];
   */
  public void flySideways(double speed);

  /**
   * Lets the drone fly up or down if the drone flying state if FLYING
   * 
   * @param height
   *   Gives the that the drone wants to maintain during its flight (in meters)
   */
  public void setHeight(double height);

  /**
   * Lets the drone spin left or right if the drone flying state if FLYING
   * 
   * @param speed
   *   Gives the speed with that the drone turns left or right. Negative values let the
   *   drone turn left and positive values let the drone turn right. The value is clamped
   *   to the interval [-1, 1];
   */
  public void spin(double speed);
  
  /**
   * Resets all previous movement commands and lets the drone hover using the 
   * bottom image to stabilize itself.
   */
  public void hover();
}
