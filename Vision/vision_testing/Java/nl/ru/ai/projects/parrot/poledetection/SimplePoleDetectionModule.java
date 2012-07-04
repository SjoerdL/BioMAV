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

import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;

public class SimplePoleDetectionModule implements PolePositionPollInterface {
  private VideoPollInterface vpi;
  private ByteBuffer bb = ByteBuffer.allocateDirect(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH * VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT * 3);
  private long lastTimeStamp = -1;
  
  private double[] circularDepthBuffer = new double[10];
  private int circularBufferIndex = 0;
  
  public SimplePoleDetectionModule(VideoPollInterface vpi) {
    this.vpi = vpi;
  }
  
  @Override
  public long getPolePositionTimeStamp() {
    return vpi.getFrontCameraTimeStamp();
  }

  private boolean presentNewImage() {
    byte[] imageData = vpi.getFrontCameraImage();
    if ((imageData != null) && (lastTimeStamp != vpi.getFrontCameraTimeStamp())) {
      lastTimeStamp = vpi.getFrontCameraTimeStamp();
      
      bb.rewind();
      bb.put(imageData, 0, Math.min(bb.capacity(), imageData.length));
      
      CPPPoleDetector.setData(bb);
      
      return true;
    }
    return false;
  }
  
  @Override
  public Double[] getPolePositions() {
    presentNewImage();
    
    Double[] res = {null, null};
    
    double[] rawResult = CPPPoleDetector.getPolePositions();
    for (int i = 0; i < Math.min(res.length, rawResult.length); i++) {
      res[i] = rawResult[i];
    }
    
    return res;
  }

  @Override
  public Double[] getPoleDistances() {
    Double[] res = {null};
    
    boolean newImage = presentNewImage();
    
    double[] rawResult = CPPPoleDetector.getPoleDistances();
    if (rawResult.length > 0) {
      if (newImage) {
        circularDepthBuffer[0] = rawResult[0] * 9 * 10;
        circularBufferIndex = (circularBufferIndex + 1) % circularDepthBuffer.length;
      }
      
      double avg = 0;
      for (int i = 0; i < circularDepthBuffer.length; i++) {
        avg += circularDepthBuffer[i];
      }
      avg /= circularDepthBuffer.length;
      
      res[0] = avg;
    }
    
    return res;
  }
}
