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
package nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol.test;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.net.SocketException;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;
import nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol.DroneGroundStation;

/**
 * A little test class demonstrating the image streaming capabilities of the JavaDroneControl
 * library. 
 * 
 * @author Paul Konstantin Gerke
 */
public class Test01 extends JFrame {
  private static DroneGroundStation drone = null;
  
  // Buffered image to hold data from the image stream
  private BufferedImage image = new BufferedImage(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, BufferedImage.TYPE_3BYTE_BGR);
  
  public Test01() {
    super("Test vision frame");
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    setSize(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT);
    
    // Timer used to poll images from drone
    Timer timer = new Timer();
    timer.scheduleAtFixedRate(new TimerTask() {
      @Override
      public void run() {
        byte[] imageData = drone.getFrontCameraImage();
        System.out.println("Front camera timestamp: " + drone.getFrontCameraTimeStamp());
        if (imageData != null) {
          WritableRaster raster = Raster.createBandedRaster(DataBuffer.TYPE_BYTE, VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, 3, new Point(0, 0));
          raster.setDataElements(0, 0, VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, imageData);
          image.setData(raster);
        }
        
        repaint();
      }
    }, 200, 200);
  }
  
  @Override
  public void paint(Graphics g) {
    // Clutch the image inside the window (could be done nicer - whatever)
    g.drawImage(image, 0, 0, this);
  }
  
  public static void main(String[] args) {
    try {
      // First step: create a ground station for a given drone
      drone = new DroneGroundStation();
      // Assuming the WiFi connection has been established, connect to the default IP 
      drone.connect("192.168.1.1");

      // If done, initialize test-window
      SwingUtilities.invokeLater(new Runnable() {
        @Override
        public void run() {
          final Test01 main = new Test01();
          main.setVisible(true);
        }
      });
    } catch (SocketException e) {
      e.printStackTrace();
    }
  }
}
