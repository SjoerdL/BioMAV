package atcontrol.connection;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.util.Iterator;
import java.util.Set;
import atcontrol.DroneCommander;

public class DroneNavDataReceiver extends Thread
{
	private static final int BUFSIZE = 4096;
	
	public DroneCommander commander;
	public InetAddress droneAddress;
	public int navDataPort;
	
	public DatagramChannel navdataChannel;
	private Selector selector;
	
	public boolean done = false;
	
	public DroneNavDataReceiver(DroneCommander commander, InetAddress droneAddress, int navDataPort)
	{
		this.commander = commander;
		this.droneAddress = droneAddress;
		this.navDataPort = navDataPort;
		
		try
		{
			navdataChannel = DatagramChannel.open();
			navdataChannel.configureBlocking(false);
			navdataChannel.socket().bind(new InetSocketAddress(navDataPort));
			navdataChannel.connect(new InetSocketAddress(droneAddress, navDataPort));
	
			selector = Selector.open();
			navdataChannel.register(selector, SelectionKey.OP_READ | SelectionKey.OP_WRITE);
		}
		catch(Exception e)
		{
			System.err.println("Exception opening the channel to receive navdata.");
			e.printStackTrace();
			System.exit(-1);
		}
		
		startup();
	}
	
	private void startup()
	{
		commander.exitBootstrapMode();
		
		DroneNavData nd = getDataOnce();
		
		System.out.println("checking if control is received..");
		
		while(nd != null && !nd.controlReceived)
		{
			commander.exitBootstrapMode();
			nd = getDataOnce();
		}
		
		System.out.println("control received. sending acknowledge command");
		
		commander.sendAcknowledgeCommand();
	}
	
	@SuppressWarnings("rawtypes")
	private DroneNavData getDataOnce()
	{
		DroneNavData nd = null;
		
		try
		{
			ByteBuffer inbuf = ByteBuffer.allocate(BUFSIZE);

			// System.out.println("One pass");
			selector.select();

			Set readyKeys = selector.selectedKeys();
			Iterator iterator = readyKeys.iterator();
			while (iterator.hasNext())
			{
				SelectionKey key = (SelectionKey) iterator.next();
				iterator.remove();
				if (key.isWritable())
				{
					byte[] trigger_bytes = { 0x01, 0x00, 0x00, 0x00 };
					ByteBuffer trigger_buf = ByteBuffer.allocate(trigger_bytes.length);
					trigger_buf.put(trigger_bytes);
					trigger_buf.flip();
					navdataChannel.write(trigger_buf);
					navdataChannel.register(selector, SelectionKey.OP_READ);
				}
				else if (key.isReadable())
				{
					inbuf.clear();
					int len = navdataChannel.read(inbuf);
					byte[] packet = new byte[len];
					inbuf.flip();
					inbuf.get(packet, 0, len);

					nd = DroneNavData.createFromData(packet);

				//	commander.navDataReceived(nd);
				}
			}
		} catch (Exception e)
		{
			e.printStackTrace();
			// drone.changeToErrorState(e);
		}
		
		return nd;
	}
	
	private void disconnect()
	{
		try
		{
			selector.close();
		} catch (IOException iox)
		{
			// Ignore
		}

		try
		{
			navdataChannel.disconnect();
		} catch (IOException iox)
		{
			// Ignore
		}
	}

	@SuppressWarnings("rawtypes")
	@Override
	public void run()
	{
		try
		{
			ByteBuffer inbuf = ByteBuffer.allocate(BUFSIZE);
			done = false;
			while (!done)
			{
		//		System.out.println("One pass");
				selector.select();
				if (done)
				{
					disconnect();
					break;
				}
				Set readyKeys = selector.selectedKeys();
				Iterator iterator = readyKeys.iterator();
				while (iterator.hasNext())
				{
					SelectionKey key = (SelectionKey) iterator.next();
					iterator.remove();
					if (key.isWritable())
					{
						byte[] trigger_bytes = { 0x01, 0x00, 0x00, 0x00 };
						ByteBuffer trigger_buf = ByteBuffer.allocate(trigger_bytes.length);
						trigger_buf.put(trigger_bytes);
						trigger_buf.flip();
						navdataChannel.write(trigger_buf);
						navdataChannel.register(selector, SelectionKey.OP_READ);
					}
					else if (key.isReadable())
					{
						inbuf.clear();
						int len = navdataChannel.read(inbuf);
						byte[] packet = new byte[len];
						inbuf.flip();
						inbuf.get(packet, 0, len);

						DroneNavData nd = DroneNavData.createFromData(packet);

						commander.navDataReceived(nd);
					}
				}
			}
		} catch (Exception e)
		{
			e.printStackTrace();
			//drone.changeToErrorState(e);
		}

	}
}
