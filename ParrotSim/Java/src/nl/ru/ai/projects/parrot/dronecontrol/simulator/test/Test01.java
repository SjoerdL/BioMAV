package nl.ru.ai.projects.parrot.dronecontrol.simulator.test;

import java.awt.Graphics;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.util.Arrays;
import java.util.Collections;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import nl.ru.ai.projects.parrot.dronecontrol.SensorDataObserver;
import nl.ru.ai.projects.parrot.dronecontrol.SensoryDataInterface;
import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;

public class Test01 extends JFrame implements SensorDataObserver {
  private static final long serialVersionUID = 5576308635876419113L;

  private Simulator simulator = null;

  private SimulatedDrone drone = null;

  private BufferedImage image = new BufferedImage(
      VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH,
      VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, BufferedImage.TYPE_3BYTE_BGR);

  private BufferedImage groundImage = new BufferedImage(
      VideoPollInterface.GROUND_VIDEO_FRAME_WIDTH,
      VideoPollInterface.GROUND_VIDEO_FRAME_HEIGHT,
      BufferedImage.TYPE_3BYTE_BGR);

  public Test01() {
    super("Simulator Test Application 01");

    simulator = Simulator.getInstance();

    simulator.activateRendering(true);
    simulator.setCoupleTime(false); // Decouples the simulation time from the
                                    // system time
    drone = simulator.getDrone(0);

    // simulator.activateVideoInterface(false); // Can be deactivated to speed
    // up the simulation

    drone.takeoff();
    drone.setDronePosition(4, 4.5, 2.5);

    drone.addSensorDataObserver(this);
    
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    setSize(800, 600);
    setLocation(0, 30);
    setVisible(true);

    Timer timer = new Timer();
    timer.schedule(new TimerTask() {
      @Override
      public void run() {
        drone.spin(0.5);

        /*        Double[] polePositions = drone.getPolePositions();
        if (polePositions != null) {
          System.out.println(Arrays.asList(polePositions));
        }*/

        if (simulator.isVideoActivated()) {
          {
            byte[] rgbData = drone.getFrontCameraImage();
            if ((rgbData != null) && (rgbData.length > 0)) {
              WritableRaster raster = Raster.createBandedRaster(
                  DataBuffer.TYPE_BYTE,
                  VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH,
                  VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, 3, new Point(0, 0));
              raster.setDataElements(0, 0,
                  VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH,
                  VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, rgbData);
              image.setData(raster);
            }
          }

          // if (false) { // This is crap-ass buggy - still trying to work it
          // out :-/
          // byte[] rgbData = drone.getGroundCameraImage();
          // if (rgbData != null) {
          // WritableRaster raster =
          // Raster.createBandedRaster(DataBuffer.TYPE_BYTE,
          // VideoPollInterface.GROUND_VIDEO_FRAME_WIDTH,
          // VideoPollInterface.GROUND_VIDEO_FRAME_HEIGHT,
          // 3,
          // new Point(0, 0));
          // raster.setDataElements(0, 0,
          // VideoPollInterface.GROUND_VIDEO_FRAME_WIDTH,
          // VideoPollInterface.GROUND_VIDEO_FRAME_HEIGHT, rgbData);
          // groundImage.setData(raster);
          // }
          // image.getGraphics().drawImage(groundImage, 5, 5, null);
          // }

          repaint();
        }
      }
    }, 10, 10);
  }

  @Override
  public void paint(Graphics g) {
    g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
  }

  @Override
  public void newSensorData(SensoryDataInterface sender) {
    System.out.println("New sensor data");
  }

  public static void main(String[] args) {
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        Simulator.waitReady();
        new Test01();
      }
    });
    Simulator.runSimulator();
  }
}
