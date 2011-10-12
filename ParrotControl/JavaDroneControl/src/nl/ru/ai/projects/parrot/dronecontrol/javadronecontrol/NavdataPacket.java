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

import java.net.DatagramPacket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * This class wraps a navdata packet as received via the UDP to the drone and extracts the standard
 * state fields nad the demo-data set from it. The class furthermore implements packet validation
 * via checksum checking.
 * 
 * To use this class initialize it using the default constructor. After initialization you can
 * parse/wrap a {@link DatagramPacket} using the function {@link #parseNavdata(DatagramPacket)}. The parsed 
 * data then is available via the public fields of this object. If the parsing goes wrong, the
 * method throws a {@link NavdataPacketFormatException}.
 * 
 * @author Paul Konstantin Gerke
 *
 */
public class NavdataPacket {
  /**
   * Calculates the check sum across the specifed amount of bytes in the given byte buffer
   *   
   * @param bytes
   *   ByteBuffer array containing the data that the checksum should be computed on. The checksum
   *   computation starts at the current position of the buffer cursor.
   * @param len
   *   Amount of bytes that should be included for the checksum computation
   * @return
   */
  public static int computeChecksum(ByteBuffer bytes, int len) {
    int result = 0;
    for (int i = 0; i < len; i++) {
      byte b = bytes.get();
      result += b < 0 ? 0x0100 + b : b;
    }
    return result;
  }
  
  public static final int HID_CHECKSUM = 0xFFFF;
  
  /* Constants copied from official Parrot SDK */
  
  public static final int ARDRONE_FLY_MASK            = 1 << 0;  /*!< FLY MASK : (0) ardrone is landed, (1) ardrone is flying */
  public static final int ARDRONE_VIDEO_MASK          = 1 << 1;  /*!< VIDEO MASK : (0) video disable, (1) video enable */
  public static final int ARDRONE_VISION_MASK         = 1 << 2;  /*!< VISION MASK : (0) vision disable, (1) vision enable */
  public static final int ARDRONE_CONTROL_MASK        = 1 << 3;  /*!< CONTROL ALGO : (0) euler angles control, (1) angular speed control */
  public static final int ARDRONE_ALTITUDE_MASK       = 1 << 4;  /*!< ALTITUDE CONTROL ALGO : (0) altitude control inactive (1) altitude control active */
  public static final int ARDRONE_USER_FEEDBACK_START = 1 << 5;  /*!< USER feedback : Start button state */
  public static final int ARDRONE_COMMAND_MASK        = 1 << 6;  /*!< Control command ACK : (0) None, (1) one received */
  public static final int ARDRONE_FW_FILE_MASK        = 1 << 7;  /* Firmware file is good (1) */
  public static final int ARDRONE_FW_VER_MASK         = 1 << 8;  /* Firmware update is newer (1) */
//    ARDRONE_FW_UPD_MASK         = 1 << 9;  /* Firmware update is ongoing (1) */
  public static final int ARDRONE_NAVDATA_DEMO_MASK   = 1 << 10; /*!< Navdata demo : (0) All navdata, (1) only navdata demo */
  public static final int ARDRONE_NAVDATA_BOOTSTRAP   = 1 << 11; /*!< Navdata bootstrap : (0) options sent in all or demo mode, (1) no navdata options sent */
  public static final int ARDRONE_MOTORS_MASK         = 1 << 12; /*!< Motors status : (0) Ok, (1) Motors problem */
  public static final int ARDRONE_COM_LOST_MASK       = 1 << 13; /*!< Communication Lost : (1) com problem, (0) Com is ok */
  public static final int ARDRONE_VBAT_LOW            = 1 << 15; /*!< VBat low : (1) too low, (0) Ok */
  public static final int ARDRONE_USER_EL             = 1 << 16; /*!< User Emergency Landing : (1) User EL is ON, (0) User EL is OFF*/
  public static final int ARDRONE_TIMER_ELAPSED       = 1 << 17; /*!< Timer elapsed : (1) elapsed, (0) not elapsed */
  public static final int ARDRONE_ANGLES_OUT_OF_RANGE = 1 << 19; /*!< Angles : (0) Ok, (1) out of range */
  public static final int ARDRONE_ULTRASOUND_MASK     = 1 << 21; /*!< Ultrasonic sensor : (0) Ok, (1) deaf */
  public static final int ARDRONE_CUTOUT_MASK         = 1 << 22; /*!< Cutout system detection : (0) Not detected, (1) detected */
  public static final int ARDRONE_PIC_VERSION_MASK    = 1 << 23; /*!< PIC Version number OK : (0) a bad version number, (1) version number is OK */
  public static final int ARDRONE_ATCODEC_THREAD_ON   = 1 << 24; /*!< ATCodec thread ON : (0) thread OFF (1) thread ON */
  public static final int ARDRONE_NAVDATA_THREAD_ON   = 1 << 25; /*!< Navdata thread ON : (0) thread OFF (1) thread ON */
  public static final int ARDRONE_VIDEO_THREAD_ON     = 1 << 26; /*!< Video thread ON : (0) thread OFF (1) thread ON */
  public static final int ARDRONE_ACQ_THREAD_ON       = 1 << 27; /*!< Acquisition thread ON : (0) thread OFF (1) thread ON */
  public static final int ARDRONE_CTRL_WATCHDOG_MASK  = 1 << 28; /*!< CTRL watchdog : (1) delay in control execution (> 5ms), (0) control is well scheduled */
  public static final int ARDRONE_ADC_WATCHDOG_MASK   = 1 << 29; /*!< ADC Watchdog : (1) delay in uart2 dsr (> 5ms), (0) uart2 is good */
  public static final int ARDRONE_COM_WATCHDOG_MASK   = 1 << 30; /*!< Communication Watchdog : (1) com problem, (0) Com is ok */
  public static final int ARDRONE_EMERGENCY_MASK      = 1 << 31;  /*!< Emergency landing : (0) no emergency; (1) emergency */

  /**
   * Exception thrown if the received navdata packet contained invalid data.
   * 
   * @author Paul Konstantin Gerke
   *
   */
  public static class NavdataPacketFormatException extends Exception {
    private static final long serialVersionUID = -9002844056303380102L;

    /**
     * Creates a NavdataPacketFormatException with the specified reason.
     * 
     * @param why
     */
    public NavdataPacketFormatException(String why) {
      super(why);
    }
  }
  
  /**
   * General representation of a header (including the following data)
   * of a navdata packet sent by the AR.Drone
   * 
   * @author Paul Konstantin Gerke
   *
   */
  private class DataHeader {
    public int headerID;
    public byte[] data;
    
    /**
     * Initializes this DataHeader using data from a ByteBuffer
     * 
     * @param src
     *   ByteBuffer that the herader information should be read from, starting
     *   at the current ByteBuffer position.
     * 
     * @throws NavdataPacketFormatException
     *   Thrown if the format of read header is invalid
     */
    public DataHeader(ByteBuffer src) throws NavdataPacketFormatException {
      if (src.remaining() < 4) {
        throw new NavdataPacketFormatException("Header of option too small");
      }
      headerID = src.getShort();
      if (headerID < 0) {
        headerID += 0x00010000;
      }
      int size = src.getShort() - 4;
      if (src.remaining() < size) {
        throw new NavdataPacketFormatException("Data field of option to long");
      }
      data = new byte[size];
      src.get(data);
    }
  }
  
  /**
   * Data structure object exposing the demo data set to Java (see AR.Drone SDK 1.8 
   * for details)
   * 
   * @author Paul Konstantin Gerke
   * 
   */
  public class DemoDataStruct {
    public static final int HID = 0;

    public int controlState, batteryPercentage;
    
    /**
     * float32_t   theta;      /*!< UAV's pitch in milli-degrees 
     * float32_t   phi;        /*!< UAV's roll  in milli-degrees 
     * float32_t   psi;        /*!< UAV's yaw   in milli-degrees 
     */
    public float[] euler;
    
    /**!< UAV's altitude in centimeters */
    public int altitude;

    /**!< UAV's estimated linear velocity */
    public float[] velocity;
    
    /**
     * Initializes the demo data using the raw data from the demo header.
     * 
     * @param ba
     *   Byte information read from the demo-section of the navdata.
     */
    public DemoDataStruct(byte[] ba) {
      ByteBuffer bb = ByteBuffer.wrap(ba);
      bb.order(ByteOrder.LITTLE_ENDIAN);
      
      controlState = bb.getInt();
      batteryPercentage = bb.getInt();
      
      euler = new float[3];
      euler[0] = bb.getFloat();
      euler[1] = bb.getFloat();
      euler[2] = bb.getFloat();
      
      altitude = bb.getInt();
      
      velocity = new float[3];
      velocity[0] = bb.getFloat();
      velocity[1] = bb.getFloat();
      velocity[2] = bb.getFloat();
    }
  }
  
  public int droneState = -1;
  public int sequenceNumber = -1;
  public int visionFlag = -1;
  
  public DemoDataStruct demodata = null;
  
  /**
   * Parses a DatagramPacket sent from the drone via the Navdata stream and stores the navdata
   * contents in the public fields of this object. 
   * 
   * @param packet
   *   DatagramPacket received via the AR.Drone navdata stream
   * @throws NavdataPacketFormatException
   *   Thrown if the specified datagram packet does not contain navdata or if the received navdata packet
   *   is broken.
   */
  public void parseNavdata(DatagramPacket packet) throws NavdataPacketFormatException {
    ByteBuffer sourceByteBuffer = ByteBuffer.wrap(packet.getData(), packet.getOffset(), packet.getLength());
    sourceByteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    
    {
      int header = sourceByteBuffer.getInt();
      if (header != 0x55667788) {
        throw new NavdataPacketFormatException("Magic number mismatch");
      }
    }
    
    // Read standard fields
    droneState = sourceByteBuffer.getInt();
    sequenceNumber = sourceByteBuffer.getInt();
    visionFlag = sourceByteBuffer.getInt();
    
    List<DataHeader> headers = new ArrayList<DataHeader>();
    // Extract headers and data
    while (sourceByteBuffer.remaining() > 0) {
      headers.add(new DataHeader(sourceByteBuffer));
    }
    
    if (headers.size() <= 0) {
      throw new NavdataPacketFormatException("To few options: at least checksum must be contained in navdata");
    }
    
    {
      DataHeader checksumHeader = headers.get(headers.size() - 1);
      if (checksumHeader.headerID != HID_CHECKSUM) {
        throw new NavdataPacketFormatException("Last package must be checksum");
      }
      sourceByteBuffer.rewind();
      int checksum = computeChecksum(sourceByteBuffer, packet.getLength() - 8);
      {
        // Compare checksums
        ByteBuffer checksumBuffer = ByteBuffer.wrap(checksumHeader.data);
        checksumBuffer.order(ByteOrder.LITTLE_ENDIAN);
        if (checksumBuffer.getInt() != checksum) {
          throw new NavdataPacketFormatException("Checksum error!");
        }
      }
      
      // Remove checksum header after test
      headers.remove(checksumHeader);
    }
    
    for (DataHeader header : headers) {
      switch (header.headerID) {
        case DemoDataStruct.HID:
          demodata = new DemoDataStruct(header.data);
          break;
      }
    }
  }
}
