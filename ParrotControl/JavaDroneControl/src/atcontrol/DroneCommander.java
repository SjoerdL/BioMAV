package atcontrol;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.StringTokenizer;
import interfaces.ControlInterface;
import interfaces.SensoryDataInterface;
import atcontrol.commands.ChangeHeightCommand;
import atcontrol.commands.ClearEmergencyCommand;
import atcontrol.commands.ExitBootstrapCommand;
import atcontrol.commands.HoverCommand;
import atcontrol.commands.LandCommand;
import atcontrol.commands.MaxAltitudeCommand;
import atcontrol.commands.MoveForwardCommand;
import atcontrol.commands.MoveSidewaysCommand;
import atcontrol.commands.RotateCommand;
import atcontrol.commands.SetEmergencyCommand;
import atcontrol.commands.TakeOffCommand;
import atcontrol.commands.WatchdogCommand;
import atcontrol.connection.DroneCommandConnection;
import atcontrol.connection.DroneNavData;
import atcontrol.connection.DroneNavDataReceiver;
import atcontrol.connection.DroneVideoReceiver;

public class DroneCommander implements ControlInterface, SensoryDataInterface, Runnable
{
	DroneCommanderFrame commanderFrame;
	DroneCommandConnection commandConnection;
	DroneNavDataReceiver navDataReceiver;
	DroneVideoReceiver videoReceiver;
	
	Thread watchdogThread; // watchdog thread to keep the dog awake (or asleep, for that matter)
	long watchdogWait = 200;
	
	// settings
	public int maxAltitude = 2000; // max altitude of the drone in mm
	public double speed = 0.15; // the speed at which the drone will move. interval [0..1]
	
	// connection settings
	public int commandPort = 5556;
	public int videoPort = 5555;
	public int navDataPort = 5554;
	public String droneIP = "192.168.1.1";
	public InetAddress droneInetAddress;
	
	// internal data
	private FlyingState flyingState = FlyingState.LANDED; // indicating flying state of the drone.
	public int commandNr = 1; // command counter
	
	public String lastSendCmdPt1 = "";
	public String lastSendCmdPt2 = "";
	
	public DroneNavData latestNavData;
	private long latestNavDataTimestamp;
	private DroneNavData lastRequestedNavData;
	
	public DroneCommander()
	{
		droneInetAddress = getInetAddress(droneIP);
		
		commanderFrame = new DroneCommanderFrame(this);
		commandConnection = new DroneCommandConnection(this, droneInetAddress, commandPort);
		
		setMaxAltitude(maxAltitude);
		
		navDataReceiver = new DroneNavDataReceiver(this, droneInetAddress, navDataPort);
		
		watchdogThread = new Thread(this);
		watchdogThread.start();
		
		navDataReceiver.start();
		
		videoReceiver = new DroneVideoReceiver(this, droneInetAddress, videoPort);
		videoReceiver.start();
	}	
	
	/**
	 * Set's the maximum altitude of the drone.
	 */
	private void setMaxAltitude(int maxAltitude)
	{
		commandConnection.sendCommand(MaxAltitudeCommand.createCommand(commandNr++, maxAltitude)); 
	}
	
	/**
	 * Let's the drone exit bootstrap mode so that it actually keeps sending useful 
	 * navData.
	 */
	public void exitBootstrapMode()
	{
		System.out.println("Exiting bootstrap mode and entering demo mode");
		
		commandConnection.sendCommand(ExitBootstrapCommand.createCommand(commandNr++));
	}
	
	public void sendAcknowledgeCommand()
	{
		System.out.println("Sending aknowledge command.");
		String acknowledgeStr = "AT*CTRL=0";
		
		commandConnection.sendCommand(acknowledgeStr);
		
	}
	
	@Override
	public void takeoff()
	{
		System.out.println("Drone is taking off.");
		
		// while navData doesnt say drone has taken off
		// {
		commandConnection.sendCommand(TakeOffCommand.createCommand(commandNr++));
		// }
		
		flyingState = FlyingState.FLYING;
	}
	
	@Override
	public void land()
	{
		System.out.println("Drone is landing.");
		
		// while navData doesnt say drone has landed
		// {
			commandConnection.sendCommand(LandCommand.createCommand(commandNr++));
		// }
		
		flyingState = FlyingState.LANDED;
	}

	@Override
	public void hover()
	{
		System.out.println("Drone is hovering.");
		
		commandConnection.sendCommand(HoverCommand.createCommand(commandNr++));
		
		flyingState = FlyingState.FLYING;
	}
	
	@Override
	public FlyingState getFlyingState()
	{
		return flyingState;
	}

	@Override
	public void setEmergencyFlag()
	{
		System.err.println("Drone going in emergency state!");

		commandConnection.sendCommand(SetEmergencyCommand.createCommand(commandNr++));

		flyingState = FlyingState.EMERGENCY;
	}

	@Override
	public void clearEmergencyFlag()
	{
	//	if(flyingState == FlyingState.EMERGENCY)
	//	{
		System.out.println("Drone leaving emergency state!");

		commandConnection.sendCommand(ClearEmergencyCommand.createCommand(commandNr++));

		flyingState = FlyingState.LANDED;
	//	}
	}

	@Override
	public void flyForward(double speed)
	{
		if(flyingState == FlyingState.FLYING)
		{
			if(speed < 0)
			{
				System.out.println("Drone is moving backward.");
			}
			else
			{
				System.out.println("Drone is moving forward.");
			}
			
			commandConnection.sendCommand(MoveForwardCommand.createCommand(commandNr++, convertSpeed(speed)));
		}
	}

	@Override
	public void flySideways(double speed)
	{
		if(flyingState == FlyingState.FLYING)
		{
			if(speed < 0)
			{
				System.out.println("Drone is moving sideways to the left.");
			}
			else
			{
				System.out.println("Drone is moving sideways to the right.");
			}
			
			commandConnection.sendCommand(MoveSidewaysCommand.createCommand(commandNr++, convertSpeed(speed)));
		}
	}

	@Override
	public void setHeight(double height)
	{
		// laten zitten. Paul heeft een wrapper
	}

	@Override
	public void spin(double speed)
	{
		if(flyingState == FlyingState.FLYING)
		{
			if(speed < 0)
			{
				System.out.println("Drone is turning to the left.");
			}
			else
			{
				System.out.println("Drone is turning to the right.");
			}
			
			commandConnection.sendCommand(RotateCommand.createCommand(commandNr++, convertSpeed(speed)));
		}						
	}
	
	/**
	 * If speed is positive, the height of the drone will increase. If speed is negative,
	 * the height of the drone will decrease;
	 * @param speed
	 */
	public void changeHeight(double speed)
	{
		if(flyingState == FlyingState.FLYING)
		{
			if(speed < 0)
			{
				System.out.println("Drone is going downwards");
			}
			else
			{
				System.out.println("Drone is going upwards");
			}
			
			commandConnection.sendCommand(ChangeHeightCommand.createCommand(commandNr++, convertSpeed(speed)));
		}
	}
	
	public void navDataReceived(DroneNavData nd)
	{
		latestNavData = nd;
		latestNavDataTimestamp = System.currentTimeMillis();
		
	//	DroneNavData.printState(nd);
		
		commanderFrame.updateLabels();
	}
	
	/**
	 * Thread to keep the watchdog busy so that the drone is fully controllable.
	 * It does this by reiterating the last command that was send.
	 */
	@Override
	public void run()
	{		
		while (true)
		{
			commandConnection.sendWatchdogCommand(WatchdogCommand.createCommand(commandNr++));
			
			try // sleeping cause this command has only to be send once every 50 ms.
			{   // not taking chances on this bitch, we are doing it every 30 ms.
				Thread.sleep(watchdogWait);
			} catch (InterruptedException iex)
			{
			}
		}
	}
	
	/**
	 * Converts a double to its integer-representation.
	 * @param s The double that is to be converted.
	 * @return The returned integer-representation of s.
	 */
	private int convertSpeed(double s)
	{
		if(s < -1)
			s = -1;
		else if(s > 1)
			s = 1;
		
		float speed2 = (float) s;
		
		FloatBuffer fb;
		IntBuffer ib;
		ByteBuffer bb = ByteBuffer.allocate(4);
		fb = bb.asFloatBuffer();
		ib = bb.asIntBuffer();
		fb.put(0, speed2);
		
		return ib.get(0);
	}
	
	private InetAddress getInetAddress(String ipString)
	{
		// parsing IP address
		StringTokenizer st = new StringTokenizer(ipString, ".");
		byte[] ip_bytes;
		
		ip_bytes = new byte[4];
		if (st.countTokens() == 4)
		{
			for (int i = 0; i < 4; i++)
			{
				ip_bytes[i] = (byte) Integer.parseInt(st.nextToken());
			}
		}
		else
		{
			System.out.println("Incorrect IP address format: " + droneIP);
			System.exit(-1);
		}

		System.out.println("IP: " + droneIP);
		
		InetAddress address = null;
		try
		{
			address = InetAddress.getByAddress(ip_bytes);
		} catch (UnknownHostException e)
		{
			System.err.println("Could not parse the given IP address.");
			e.printStackTrace();
			System.exit(-1);
		}
		
		return address;
	}

	@Override
	public long getLastSensoryDataTimeStamp()
	{
		return latestNavDataTimestamp;
	}

	@Override
	public double[] getCurrentRotationVector()
	{
		double[] data = new double[3];
		
		// get data 
		if(lastRequestedNavData == null)
		{
			return data;
		}
		else
		{			
			data[0] = lastRequestedNavData.getYaw()  - latestNavData.getYaw();
			data[1] = lastRequestedNavData.getPitch()  - latestNavData.getPitch();
			data[2] = lastRequestedNavData.getRoll()  - latestNavData.getRoll();
		}
		
		lastRequestedNavData = latestNavData;
		
		return data;
	}
}
