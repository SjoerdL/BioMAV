package nl.ru.ai.projects.parrot.tools;

import java.util.ArrayList;

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
