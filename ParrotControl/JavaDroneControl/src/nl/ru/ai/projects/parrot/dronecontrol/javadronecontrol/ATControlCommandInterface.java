package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;

public class ATControlCommandInterface {
  public static final int PORT = 5556;
  public static final int COMMAND_STROBE_DELAY = 30; // milliseconds
  public static final int SHUTDOWN_COMMAND_REPEATITIONS = 10;
  public static final int COMMAND_REPEATITIONS = 5;
  public static final int COM_WATCHDOG_DELAY = 200;

  private interface Command {
    public String buildString();
  }
  
  private class FlatTrimCommand implements Command {
    @Override
    public String buildString() {
      return "AT*FTRIM=" + commandCounter++;
    }
  }
  
  private class WatchDogCommand implements Command {
    @Override
    public String buildString() {
      return "AT*COMWDG=" + commandCounter++;
    }
  }
  
  private class StrobeEmergencyCommand implements Command {
    @Override
    public String buildString() {
      return "AT*REF=" + commandCounter++ + "," + (0x11540000 | 0x00000100);
    }
  }
  
  private class TakeoffCommand implements Command {
    @Override
    public String buildString() {
      return "AT*REF=" + commandCounter++ + "," + (0x11540000 | 0x00000200);
    }
  }
  
  private class LandCommand implements Command {
    @Override
    public String buildString() {
      return "AT*REF=" + commandCounter++ + "," + (0x11540000 | 0x00000000);
    }
  }
  
  private class HoverCommand implements Command {
    @Override
    public String buildString() {
      return "AT*PCMD=" + commandCounter++ + ",0,0,0,0,0";
    }
  }
  
  private class FlyCommand implements Command {
    @Override
    public String buildString() {
      return "AT*PCMD=" + commandCounter++ + ",1," + convertFloatToInt((float) setRoll) + "," + convertFloatToInt((float) setPitch) + "," + convertFloatToInt((float) upSpeed) + "," + convertFloatToInt((float) yawSpeed);
    }
  }
  
  private class SetOptionCommand implements Command {
    public String optionName, value;
    
    public SetOptionCommand(String optionName, String value) {
      this.optionName = optionName;
      this.value = value;
    }
    
    @Override
    public String buildString() {
      return "AT*CONFIG=" + commandCounter++ + ",\"" + optionName + "\",\"" + value + "\"";
    }

    @Override
    public int hashCode() {
      final int prime = 31;
      int result = 1;
      result = prime * result + getOuterType().hashCode();
      result = prime * result
          + ((optionName == null) ? 0 : optionName.hashCode());
      result = prime * result + ((value == null) ? 0 : value.hashCode());
      return result;
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj)
        return true;
      if (obj == null)
        return false;
      if (getClass() != obj.getClass())
        return false;
      SetOptionCommand other = (SetOptionCommand) obj;
      if (!getOuterType().equals(other.getOuterType()))
        return false;
      if (optionName == null) {
        if (other.optionName != null)
          return false;
      } else if (!optionName.equals(other.optionName))
        return false;
      if (value == null) {
        if (other.value != null)
          return false;
      } else if (!value.equals(other.value))
        return false;
      return true;
    }

    private ATControlCommandInterface getOuterType() {
      return ATControlCommandInterface.this;
    }
  }
  
  private class PlannedCommand implements Command {
    private Command command;
    private int usageCount = 0;
    
    public PlannedCommand(Command command) {
      this.command = command;
    }

    public PlannedCommand(Command command, boolean single) {
      this.command = command;
      if (single) {
        usageCount = COMMAND_REPEATITIONS - 1;
      }
    }

    @Override
    public String buildString() {
      usageCount++;
      return command.buildString();
    }

    private int getUseCount() {
      return usageCount;
    }
    
    @Override
    public boolean equals(Object obj) {
      if (this == obj)
        return true;
      if (obj == null)
        return false;
      if ((obj instanceof Command) && (getClass() != obj.getClass())) {
        return command.equals(obj);
      }
      if (getClass() != obj.getClass())
        return false;
      PlannedCommand other = (PlannedCommand) obj;
      if (!getOuterType().equals(other.getOuterType()))
        return false;
      if (command == null) {
        if (other.command != null)
          return false;
      } else if (!command.equals(other.command))
        return false;
      return true;
    }

    private ATControlCommandInterface getOuterType() {
      return ATControlCommandInterface.this;
    }
  }
  
  private final Command WATCHDOG_COMMAND = new WatchDogCommand();
  private final Command FLAT_TRIM_COMMAND = new FlatTrimCommand();
  private final Command STROBE_EMERGENCY_COMMAND = new StrobeEmergencyCommand();
  private final Command TAKEOFF_COMMAND = new TakeoffCommand();
  private final Command LAND_COMMAND = new LandCommand();
  private final Command HOVER_COMMAND = new HoverCommand();
  private final Command FLY_COMMAND = new FlyCommand();
  
  
  private ByteBuffer conversionByteBuffer = ByteBuffer.allocate(4);
  private FloatBuffer conversionFloatBuffer = conversionByteBuffer.asFloatBuffer();
  private IntBuffer conversionIntBuffer = conversionByteBuffer.asIntBuffer();
  
  private DatagramSocket datagramConnection = null;
  private InetAddress targetAddress = null;
  
  private Thread updateThread = null;
  
  // Persistent command values for controlling the drone
  private int commandCounter = 0;
  private int emergencyStrobeSetCounter = 0, emergencyStrobeClearCounter = 0;
  
  private List<PlannedCommand> commandStack = new ArrayList<PlannedCommand>();
  private boolean flying = false, hovering = true;
  private double setPitch = 0, yawSpeed = 0, setRoll = 0, upSpeed = 0;
  
  private String commandString = "";
  
  public ATControlCommandInterface() throws SocketException {
    datagramConnection = new DatagramSocket(PORT);
  }
  
  public synchronized int convertFloatToInt(float f) {
    conversionFloatBuffer.put(0, f);
    return conversionIntBuffer.get(0);
  }
  
  public synchronized void connect(InetAddress targetAddress) {
    disconnect();
    
    commandCounter = 0;
    this.targetAddress = targetAddress;
    
    if (targetAddress != null) {
      final ATControlCommandInterface _this = this;
      
      updateThread = new Thread() {
        @Override
        public void run() {
          long lastWatchdog = System.currentTimeMillis() - COM_WATCHDOG_DELAY; 

          while (!Thread.interrupted()) {
            if (System.currentTimeMillis() - lastWatchdog > COM_WATCHDOG_DELAY) {
              synchronized (_this) {
                commandStack.add(new PlannedCommand(WATCHDOG_COMMAND, true));
                lastWatchdog = System.currentTimeMillis();
              }
            }
            
            try {
              sendUpdateCommand();
            } catch (IOException e) {
              e.printStackTrace();
            }
            try {
              Thread.sleep(Math.min(COMMAND_STROBE_DELAY, System.currentTimeMillis() - lastWatchdog));
            } catch (InterruptedException e) {
              Thread.currentThread().interrupt();
            }
          }
          
          // Let the drone... crash!
          if (Thread.interrupted()) {
            try {
              Thread.sleep(1);
            } catch (InterruptedException e) {
            }
            strobeEmergency();
            
            synchronized (_this) {
              for (int repeatitionCounter = 0; repeatitionCounter < SHUTDOWN_COMMAND_REPEATITIONS; repeatitionCounter++) {
                try {
                  sendUpdateCommand();
                } catch (IOException e) {
                  e.printStackTrace();
                }
                try {
                  Thread.sleep(COMMAND_STROBE_DELAY);
                } catch (InterruptedException e) {
                }
              }
              
              Thread.currentThread().interrupt();
            }
          }

          synchronized (_this) {
            _this.notifyAll();
          }
        }
      };
      updateThread.start();
    }
  }

  public synchronized void disconnect() {
    if (updateThread != null) {
      updateThread.interrupt();
      try {
        wait();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      updateThread = null;
    }
    
    this.targetAddress = null;
  }
  
  private synchronized void prepareCommandString(String command) {
    command = command.trim() + "\r";
    commandString += command;
  }

  private synchronized void sendCommandString() throws IOException {
    if (targetAddress != null) {
//    System.out.println("Sending:\n" + commandString.trim());
//    System.out.println("Sending " + commandString.length() + " bytes");
      DatagramPacket datagramPacket = new DatagramPacket(commandString.getBytes(), commandString.getBytes().length, targetAddress, PORT);
      datagramConnection.send(datagramPacket);
      commandString = "";
    } else {
      throw new IOException("ATControlCommandInterface not connected to any remote address");
    }
  }

  private synchronized void sendUpdateCommand() throws IOException {
    // Command stack
    for (PlannedCommand command : commandStack.toArray(new PlannedCommand[0])) {
      prepareCommandString(command.buildString());
      if (command.getUseCount() >= COMMAND_REPEATITIONS) {
        commandStack.remove(command);
      }
    } 

    if (emergencyStrobeSetCounter > 0) {
      emergencyStrobeSetCounter--;
      prepareCommandString(STROBE_EMERGENCY_COMMAND.buildString());
//      System.out.println("STROBIN EMERGENCY: SET");
    } else if (emergencyStrobeClearCounter > 0) {
      emergencyStrobeClearCounter--;
      prepareCommandString(LAND_COMMAND.buildString());
//      System.out.println("STROBIN EMERGENCY: CLEAR");
    } else {
      // Static commands
      if (!flying) {
        prepareCommandString(LAND_COMMAND.buildString());
      } else if (hovering) {
        prepareCommandString(HOVER_COMMAND.buildString());
      } else {
        prepareCommandString(FLY_COMMAND.buildString());
      }
    }

    sendCommandString();
  }
  
  public synchronized void takeoff() {
    flying = true;
    if (!commandStack.contains(TAKEOFF_COMMAND)) { 
      hover();
      commandStack.add(new PlannedCommand(FLAT_TRIM_COMMAND));
      commandStack.add(new PlannedCommand(TAKEOFF_COMMAND));
    }
  }

  public synchronized void land() {
    flying = false;
  }

  public synchronized void strobeEmergency() {
    if ((emergencyStrobeSetCounter <= 0) && (emergencyStrobeClearCounter <= 0)) {
      emergencyStrobeSetCounter = 20;
      emergencyStrobeClearCounter = 10;
    }
    land();
  }

  public synchronized void flyForward(double speed) {
    hovering = false;
    setPitch = -Math.max(-1, Math.min(1, speed)); // Negative is forward!
  }

  public synchronized void flySideways(double speed) {
    hovering = false;
    setRoll = Math.max(-1, Math.min(1, speed));
  }

  public synchronized void setUpSpeed(double speed) {
    hovering = false;
    upSpeed = Math.max(-1, Math.min(1, speed));
  }

  public synchronized void spin(double speed) {
    hovering = false;
    yawSpeed = Math.max(-1, Math.min(1, speed));
  }

  public synchronized void hover() {
    hovering = true;
    setPitch = 0;
    setRoll = 0;
    yawSpeed = 0;
    upSpeed = 0;
  }
  
  public synchronized void sendSetOption(String option, String value) {
    Command newCommand = new SetOptionCommand(option, value); 
    if (!commandStack.contains(newCommand)) {
      commandStack.add(new PlannedCommand(newCommand));
    }
  }
}
