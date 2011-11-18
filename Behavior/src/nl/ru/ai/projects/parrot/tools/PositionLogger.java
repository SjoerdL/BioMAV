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
package nl.ru.ai.projects.parrot.tools;

import java.util.ArrayList;

/**
 * @author sjoerdlagarde
 */
public class PositionLogger {

	private static ArrayList<double[]> positions = new ArrayList<double[]>(1000);
	
	/**
	 * Adds a position to the list of positions
	 * @param position The position to add
	 */
	public static void addPosition(double[] position) {
		positions.add(position);
	}
	
	/**
	 * Returns the positions collected so far
	 * @return A list of arrays representing the x/y values respectively
	 */
	public static ArrayList<double[]> getPositions() {
		return positions;
	}
	
	/**
	 * Resets the list of positions
	 */
	public static void resetPositions() {
		positions.clear();
	}
	
}
