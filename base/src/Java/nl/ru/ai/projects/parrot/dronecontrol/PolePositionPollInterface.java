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
 * <p>
 * This interface describes the capabilities that a pole detector should have. The set of
 * function available defines which data is extracted from the camera images.
 * </p>
 * 
 * <p>
 * At the moment the heading relative to the forward-vector of a detected pole is 
 * reported and the approximated distance to the pole in meters.
 * </p>
 * 
 * @author Paul Konstantin Gerke
 * @author Sjoerd Lagarde
 * @author Jurriaan Langevoort
 *
 */
public interface PolePositionPollInterface {
  /**
   * Retrieves the time stamp of the last retrieved sample of pole positions and
   * distances. 
   * 
   * @return
   *   Time stamp in milli seconds
   */
  public long getPolePositionTimeStamp();
  
  /**
   * Retrieves the last sample of pole positions visible to the camera. 
   * 
   * @return
   *   <p>
   *   The array of angles (in rad) to all poles in the scene. The amount of 
   *   returned doubles always equals the amount of poles there 
   *   are in the scene. However, a double can be null, which means that it is not 
   *   visible. If a double is not null it is within the intervals of
   *   [-fov / 2, fov / 2] (fov = field of view of the front camera). A negative
   *   angle indicates that the pole is to the left of the center axis of the drone,
   *   a positive angle indicates that the given pole is to the right.
   *   </p>
   *   
   *   <p>
   *   Array of pole distances. Indices in the array correspond to indices of the array returned
   *   by {@link #getPoleDistances()}.
   *   </p> 
   */
/* 
   * Notes:
   *   This function gives you a giant load of information. It is capable of 
   *   IDENTIFYING poles that get outside the field of view for a moment (the simulator 
   *   always knows where a every pole is at every time). I do not think that the vision group  
   *   will be capable of reproducing this behavior (they probably do not know all the time
   *   where every pole is at a given time).
   *     What you, however, probably can make use of is using the identity of CONTINOUSLY
   *   VISIBLE poles, which corresponds to a tracking a pole in the field of view. However,
   *   you will have to filter the information on your own (hence, throw away some information).     
   *    
   */
  public Double[] getPolePositions();

  /**
   * Gets the pole distances in meters. Elements are null if the given pole is not visible.
   * 
   * @return
   *   Array of pole distances. Indices in the array correspond to indices of the array returned
   *   by {@link #getPolePositions()}. 
   */
  public Double[] getPoleDistances();
}
