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
 * This interface is used by the NavdataChannel object to notify objects
 * that a new navdata packet has been received.
 * 
 * @author Paul Konstantin Gerke
 *
 */
public interface NavdataChannelObserver {
  /**
   * Called by a {@link NavdataChannel} object to notify the receiving
   * object that new navdata is available. During the call of this function
   * it is guaranteed that no new navdata packets will be processed, 
   * effectively resulting in a lock on the just received navdata.
   * 
   * @param channel
   *   Channel that notified the object of new navdata.
   */
  public void newNavdata(NavdataChannel channel);
}
