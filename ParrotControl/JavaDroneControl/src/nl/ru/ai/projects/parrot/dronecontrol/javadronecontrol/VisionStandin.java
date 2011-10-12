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

import java.io.IOException;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;

import com.codeminders.ardrone.VideoReader;
import com.codeminders.ardrone.VideoReceivedInterface;

/**
 * Vision stand in module which decodes the image stream from the drone using bits and pieces from the
 * javadrone project (http://code.google.com/p/javadrone/). In the long run the decoding should be done
 * by a much more efficient C implementation using a JNI to transfer the data to Java.
 * 
 * The methods getImageData() and getTimestamp() are the most interesting methods when using this
 * library, exposing an interface for querying drone-images.  
 * 
 * @author Paul Konstantin Gerke
 *
 */
public class VisionStandin implements VideoReceivedInterface {
  private static final int VIDEO_PORT = 5555; // Not completely nice - could be read from information requestable via AT-commands, however should work for the default settings
  
  private VideoReader videoReader = null;
  private Thread readerThread = null;
  
  // Temp buffer holds the raw image data as decoded by the javadrone-code, tempBuffer contains the resaturated and
  // byte-aligned pixel data
  private ByteBuffer videoBuffer = ByteBuffer.allocate(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH * VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT * 3 + 4); // + 4 = extra capacity for easy int to 3 byte conversion
  private ByteBuffer tempBuffer = ByteBuffer.allocate(videoBuffer.capacity());
  
  // Time stamp of the last received image
  private long timestamp = -1;

  {
    videoBuffer.order(ByteOrder.BIG_ENDIAN);
    tempBuffer.order(ByteOrder.BIG_ENDIAN);
  }
  
  // Timestamp and data of the last image that was returned using getImageData 
  // (these are for performance optimization so that the ByteBuffer does not
  // need to be converted to a byte-array for every frame - speeds up frame skips)
  private long lastQueriedTimestamp = -1;
  private byte[] lastQueriedImage = null;
  
  /**
   * Connects the vision stream to the drone specified by the given InetAddress
   * 
   * @param target
   *   InetAddress of the drone that should be connected to
   * @param groundStation
   *   GroundStation object that contols this VisionStanding object
   * @throws IOException
   *   Thrown by the connect functions if anything should go wrong
   */
  public synchronized void connect(InetAddress target, DroneGroundStation groundStation) throws IOException {
    disconnect();
    
    videoReader = new VideoReader(this, target, VIDEO_PORT);
    readerThread = new Thread(videoReader);
    readerThread.setDaemon(true);
    readerThread.start();
  }

  /**
   * Disconnects the vision stream from the drone and stops all receiving threads
   */
  public synchronized void disconnect() {
    if (videoReader != null) {
      videoReader.stop();
      readerThread.interrupt();
      try {
        readerThread.join();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      videoReader = null;
      
      timestamp = -1;
      lastQueriedTimestamp = -1;
      lastQueriedImage = null;
    }
  }
  
  @Override
  public void videoFrameReceived(int _x, int _y, int width, int height, int[] data) {
    int i = width * height - 1;
    tempBuffer.rewind();
    
    // Correction of RGB data stream (needs to be done to achieve saturation)
    // however this still is weird
    for (int y = height - 1; y >= 0; y--) {
      for (int x = width - 1; x >= 0; x--) {
        int targetPosition = (width * y + x) * 3;

        tempBuffer.position(targetPosition);
        tempBuffer.put((byte) ((data[i] >> 15) & 0xFF));
        tempBuffer.put((byte) ((data[i] >> 7)  & 0xFF));
        tempBuffer.put((byte) ((data[i] << 1)  & 0xFF));
        
        i--;
      }
    }
    synchronized (this) {
      timestamp = System.currentTimeMillis();
      videoBuffer.rewind();
      tempBuffer.rewind();
      videoBuffer.put(tempBuffer);
    }
  }
  
  /**
   * Returns the timestamp of the last received frame
   * @return
   */
  public synchronized long getTimeStamp() {
    return timestamp;
  }
  
  /**
   * Returns the image data of the last received frame.
   * @return
   *   A VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH * VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT * 3 sized byte
   *   array which contains pixel data in RGB-form.
   */
  public synchronized byte[] getImageData() {
    if (lastQueriedTimestamp != timestamp) {
      videoBuffer.rewind();
      lastQueriedImage = videoBuffer.duplicate().array();
      lastQueriedTimestamp = timestamp;
    }
    return lastQueriedImage;
  }
}
