package atcontrol.connection;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import atcontrol.DroneCommander;

public class DroneCommandConnection
{
	public DroneCommander commander;
	public InetAddress droneAddress;
	public int commandPort;
	
	DatagramSocket socket;
	public int maxTimeout = 1000; // max time the socket will wait for a respond from the drone in ms.
	
	public DroneCommandConnection(DroneCommander commander, InetAddress droneAddress, int commandPort)
	{
		this.commander = commander;
		this.droneAddress = droneAddress;
		this.commandPort = commandPort;
		
		initialiseSocket();
	}
	
	/**
	 * Sends an AT command to the drone.
	 * @param command The command that is to be send.
	 */
	public void sendCommand(String command)
	{
		try
		{
			System.out.println("AT command: " + command);
			byte[] buffer = (command + "\r").getBytes();
			DatagramPacket packet = new DatagramPacket(buffer, buffer.length, droneAddress, 5556);
			socket.send(packet);
		}
		catch(Exception e)
		{
			System.err.println("Sending command " + command + " failed.");
			e.printStackTrace();
		}
	}
	
	/**
	 * Sends a watchdog command to the drone. 
	 * Essentially the same as 'sendCommand()', just without the output
	 * @param command
	 */
	public void sendWatchdogCommand(String command)
	{
		try
		{
			byte[] buffer = (command + "\r").getBytes();
			DatagramPacket packet = new DatagramPacket(buffer, buffer.length, droneAddress, 5556);
			socket.send(packet);
		}
		catch(Exception e)
		{
			System.err.println("Sending command " + command + " failed.");
			e.printStackTrace();
		}
	}
	
	private void initialiseSocket()
	{
		try
		{
			socket = new DatagramSocket();
			socket.setSoTimeout(maxTimeout);
		} catch (SocketException e)
		{
			e.printStackTrace();
		}
	}
}
