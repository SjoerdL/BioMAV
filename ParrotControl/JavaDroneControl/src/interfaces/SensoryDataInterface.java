package interfaces;

public interface SensoryDataInterface
{
	/**
	 * Retrieves the time stamp of the last sensory data snapshot that has been
	 * taken.
	 * 
	 * @return Timestamp in milliseconds (local system time)
	 */
	public long getLastSensoryDataTimeStamp();

	/**
	 * (Gyro information)
	 * 
	 * Returns how many radians the drone has turned since the last call to
	 * getCurrentRotationVector.
	 * 
	 * @return A 3-component double array representing a vector. The X, Y, and Z
	 *         component give the accumulated angle (radians) that the drone has
	 *         turned around the X, Y, and Z axis since the last call of this
	 *         function.
	 */
	public double[] getCurrentRotationVector();
}
