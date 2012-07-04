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
package nl.ru.ai.projects.parrot.poledetection;

import java.nio.ByteBuffer;

import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;

public class CPPPoleDetector {
  private static native void init0(int width, int height, boolean hexel, double hexelDiameter, boolean colorspace, boolean convert, int l, int m, int s, boolean normalize, boolean lateral);
  
  public static synchronized native void setData(ByteBuffer data);
  public static synchronized native double[] getPolePositions();
  public static synchronized native double[] getPoleDistances();

  static {
    System.loadLibrary("PoleDetector");
    
    final Object sync = new Object();
    synchronized (sync) {
      Thread poleDetectorThread = new Thread(new Runnable() {
        @Override
        public void run() {
          synchronized (CPPPoleDetector.class) {
            synchronized (sync) {
              sync.notifyAll();
            }
            init0(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, true, 3.6, true, true, 1, 0, 0, false, true);
          }
        }
      });
      
      poleDetectorThread.setDaemon(true);
      poleDetectorThread.start();
      
      try {
        sync.wait();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }
  }
};
