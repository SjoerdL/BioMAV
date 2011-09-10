package atcontrol.connection;

import java.net.InetAddress;
import atcontrol.DroneCommander;

public class DroneVideoReceiver extends Thread
{
	public DroneCommander commander;
	public InetAddress droneAddress;
	public int videoPort;
	
	public DroneVideoReceiver(DroneCommander commander, InetAddress droneAddress, int videoPort)
	{
		this.commander = commander;
		this.droneAddress = droneAddress;
		this.videoPort = videoPort;
	}
	
	public void run()
	{
		
	}
}
