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
 * This interface defines functions to query the images of 
 * the cameras that are attached to the drone.
 * </p>
 * 
 * @author Paul Konstantin Gerke
 *
 */
public interface VideoPollInterface {
  public static final int FRONT_VIDEO_FRAME_WIDTH = 320;
  public static final int FRONT_VIDEO_FRAME_HEIGHT = 240;
  public static final int GROUND_VIDEO_FRAME_WIDTH = 176;
  public static final int GROUND_VIDEO_FRAME_HEIGHT = 144;
  
  /**
   * Retrieves the time stamp of the last received front camera image.
   * @return 
   *   Timestamp in milliseconds (local system time) 
   */
  public long getFrontCameraTimeStamp();
  /**
   * Returns the pixel data of the last received front camera image.
   * @return
   *   Front camera RGB-Values. The array has a size of 
   *   3 * FRONT_VIDEO_FRAME_WIDTH * FRONT_VIDEO_FRAME_HEIGHT and stores
   *   pixel data in rows  
   */
  public byte[] getFrontCameraImage();

  /**
   * Retrieves the time stamp of the last received ground camera image.
   * @return
   *   Timestamp in milliseconds (local system time) 
   */
  public long getGroundCameraTimeStamp();
  /**
   * Returns the pixel data of the last received ground camera image.
   * @return
   *   Ground camera RGB-Values. The array has a size of 
   *   3 * GROUND_VIDEO_FRAME_WIDTH * GROUND_VIDEO_FRAME_HEIGHT and stores
   *   pixel data in rows  
   */
  public byte[] getGroundCameraImage();
}
