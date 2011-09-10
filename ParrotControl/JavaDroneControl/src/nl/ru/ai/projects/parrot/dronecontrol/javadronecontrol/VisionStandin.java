package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol;

import java.io.IOException;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;

import com.codeminders.ardrone.VideoReader;
import com.codeminders.ardrone.VideoReceivedInterface;

public class VisionStandin implements VideoReceivedInterface {
  private VideoReader videoReader = null;
  private Thread readerThread = null;
  
  private ByteBuffer videoBuffer = ByteBuffer.allocate(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH * VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT * 3 + 4); // + 4 = extra capacity for easy int to 3 byte conversion
  private ByteBuffer tempBuffer = ByteBuffer.allocate(videoBuffer.capacity()); 
  private long timestamp = -1;

  {
    videoBuffer.order(ByteOrder.BIG_ENDIAN);
    tempBuffer.order(ByteOrder.BIG_ENDIAN);
  }
  
  private long lastQueriedTimestamp = -1;
  private byte[] lastQueriedImage = null;
  
  public synchronized void connect(InetAddress target, DroneGroundStation groundStation) throws IOException {
    disconnect();
    
    videoReader = new VideoReader(this, target, 5555);
    readerThread = new Thread(videoReader);
    readerThread.setDaemon(true);
    readerThread.start();
  }

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
  
  public long getTimeStamp() {
    return timestamp;
  }
  
  public synchronized byte[] getImageData() {
    if (lastQueriedTimestamp != timestamp) {
      videoBuffer.rewind();
      lastQueriedImage = videoBuffer.duplicate().array();
      lastQueriedTimestamp = timestamp;
    }
    return lastQueriedImage;
  }
}
