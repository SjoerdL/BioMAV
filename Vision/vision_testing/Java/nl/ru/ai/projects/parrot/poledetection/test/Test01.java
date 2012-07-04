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
package nl.ru.ai.projects.parrot.poledetection.test;

import java.awt.Graphics;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Scanner;

import javax.swing.JFrame;

import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;
import nl.ru.ai.projects.parrot.dronecontrol.javadronecontrol.DroneGroundStation;
import nl.ru.ai.projects.parrot.poledetection.CPPPoleDetector;
import nl.ru.ai.projects.parrot.poledetection.SimplePoleDetectionModule;

public class Test01 extends JFrame implements Runnable {
  public static void main(String[] args) {
    Test01 t;
    try {
      t = new Test01();
    
      Thread mainThread = new Thread(t);
      mainThread.setDaemon(true);
      mainThread.start();

      Scanner inScanner = new Scanner(System.in);
      System.out.println("(Press enter to quit)");
      inScanner.nextLine();
      System.out.println("Quitting");
      
      mainThread.interrupt();
      System.exit(0);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  private DroneGroundStation groundStation;
  
  private BufferedImage image = new BufferedImage(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, BufferedImage.TYPE_3BYTE_BGR);
  private WritableRaster imageRaster = Raster.createBandedRaster(DataBuffer.TYPE_BYTE, image.getWidth(), image.getHeight(), 3, new Point());
  
  private SimplePoleDetectionModule spdm;
  
  public Test01() throws IOException {
    super("Test01");
    
    try {
      groundStation = new DroneGroundStation();
      groundStation.connect("192.168.1.1");
      spdm = new SimplePoleDetectionModule(groundStation);
    } catch (IOException e) {
      e.printStackTrace();
    }
    
    setVisible(true);
    setSize(400, 300);
  }
  
  @Override
  public void paint(Graphics g) {
    synchronized (imageRaster) {
      g.drawImage(image, getInsets().left, getInsets().top, getWidth() - getInsets().right, getHeight() - getInsets().bottom, this);
    }
  }
  
  @Override
  public void run() {
    ByteBuffer bb = null;
    bb = ByteBuffer.allocateDirect(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH * VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT * 3);
    
    while (!Thread.currentThread().isInterrupted()) {
      try {
        Thread.sleep(50);
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      
      byte[] frontCameraImage = groundStation.getFrontCameraImage();
      if (frontCameraImage != null) {
        System.out.println(Arrays.asList(spdm.getPolePositions()) + " " + Arrays.asList(spdm.getPoleDistances()));
        
        synchronized (imageRaster) {
          imageRaster.setDataElements(0, 0, image.getWidth(), image.getHeight(), frontCameraImage);
          image.setData(imageRaster);
        }
        repaint();
      }
    }
    
    groundStation.disconnect();
  }
}
