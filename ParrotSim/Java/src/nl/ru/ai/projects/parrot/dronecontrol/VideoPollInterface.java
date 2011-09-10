package nl.ru.ai.projects.parrot.dronecontrol;

public interface VideoPollInterface {
  public static final int FRONT_VIDEO_FRAME_WIDTH = 320;
  public static final int FRONT_VIDEO_FRAME_HEIGHT = 240;
  public static final int GROUND_VIDEO_FRAME_WIDTH = 176;
  public static final int GROUND_VIDEO_FRAME_HEIGHT = 144;
  
  /**
   * Retrieves the time stamp of the last received front camera image.
   * @return 
   *   Timestamp in milliseconds (local system time) 
   */
  public long getFrontCameraTimeStamp();
  /**
   * Returns the pixel data of the last received front camera image.
   * @return
   *   Front camera RGB-Values. The array has a size of 
   *   3 * FRONT_VIDEO_FRAME_WIDTH * FRONT_VIDEO_FRAME_HEIGHT and stores
   *   pixel data in rows  
   */
  public byte[] getFrontCameraImage();

  /**
   * Retrieves the time stamp of the last received ground camera image.
   * @return
   *   Timestamp in milliseconds (local system time) 
   */
  public long getGroundCameraTimeStamp();
  /**
   * Returns the pixel data of the last received ground camera image.
   * @return
   *   Ground camera RGB-Values. The array has a size of 
   *   3 * GROUND_VIDEO_FRAME_WIDTH * GROUND_VIDEO_FRAME_HEIGHT and stores
   *   pixel data in rows  
   */
  public byte[] getGroundCameraImage();
}
