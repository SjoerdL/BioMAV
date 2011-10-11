package com.codeminders.ardrone;

public interface VideoReceivedInterface {
  public void videoFrameReceived(int x, int y, int width, int height, int[] data);
}
