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

public class Test01 extends JFrame {
  private static DroneGroundStation drone = null;
  
  private BufferedImage image = new BufferedImage(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, BufferedImage.TYPE_3BYTE_BGR);
  
  public Test01() {
    super("Test vision frame");
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    setSize(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT);
    
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
    g.drawImage(image, 0, 0, this);
  }
  
  public static void main(String[] args) {
    try {
      drone = new DroneGroundStation();
      drone.connect("192.168.1.1");

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
