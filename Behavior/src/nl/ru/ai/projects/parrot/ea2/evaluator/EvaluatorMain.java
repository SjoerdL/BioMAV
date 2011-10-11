package nl.ru.ai.projects.parrot.ea2.evaluator;

import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Scanner;

import nl.ru.ai.projects.parrot.dronecontrol.SensorDataObserver;
import nl.ru.ai.projects.parrot.dronecontrol.SensoryDataInterface;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.SimulatedDrone;
import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.ea2.LaurieEA;
import nl.ru.ai.projects.parrot.ea2.SjoerdEA;
import nl.ru.ai.projects.parrot.ea2.client.FSMFactory;
import nl.ru.ai.projects.parrot.ea2.server.TaskParameters;
import nl.ru.ai.projects.parrot.fsm.FSMController;
import nl.ru.ai.projects.parrot.tools.TerminatorTimer;

public class EvaluatorMain {
  public static double calculateFitness(List<double[]> positionLog) {
    double fitness = 0.0;
    
    double pole1 = 5.0, pole2 = -5.0;   // Coordinates of poles: (-5, 0) and (5, 0)
    int caseCounter = 0;
    for ( int i=0; i<positionLog.size(); i++ ) {
      double[] coord = positionLog.get(i);
      
      switch( caseCounter ) {
        case 0:     // Pole 1, left pass
          if ( coord[1] > 0 && coord[0] > pole1 ) {
            fitness++;
            caseCounter++;
          }
          break;
        case 1:     // Pole 1, right pass (return to center)
          if ( coord[1] < 0 && coord[0] < pole1 ) {
            fitness++;
            caseCounter++;
          }
          break;
        case 2:     // Pole 2, right pass
          if ( coord[1] > 0 && coord[0] < pole2 ) {
            fitness++;
            caseCounter++;
          }
          break;
        case 3:     // Pole 2, left pass (return to center)
          if ( coord[1] < 0 && coord[0] > pole2 ) {
            fitness++;
            caseCounter = 0;
          }
          break;
        
        default: 
          caseCounter = 0;
      }
    }

    return fitness;
  }

  public static void main(String[] args) {
    // Read input data
    Scanner inScanner = new Scanner(System.in);
    String input = "";
    try {
      while (true) {
        String line = inScanner.nextLine().replaceAll("[\n\r]", "");
        if(line.equals("stop:0"))
        	break;
        input += line + "\n";
        
        
      }
    }
    catch (NoSuchElementException e) {
    }

    final Simulator sim = Simulator.getInstance();
    final SimulatedDrone drone = sim.getDrone(0);
    
    sim.setCoupleTime(false);
    sim.activateRendering(false);
    
    final List<double[]> positionLog = new ArrayList<double[]>();
    drone.addSensorDataObserver(new SensorDataObserver() {
      @Override
      public void newSensorData(SensoryDataInterface sender) {
        double[] pos = drone.getDronePosition();
        if (pos != null) {
          positionLog.add(pos);
        }
      }
    });
    
    TaskParameters taskParameters = new TaskParameters(input);
    FSMFactory fsmFactory = new LaurieEA();//new SjoerdEA();
    FSMController controller = fsmFactory.createFSM(drone, drone, taskParameters.getMap());

    TerminatorTimer timerTerminator = new TerminatorTimer();
    timerTerminator.setThreshold(5 * 60000);
    controller.addTerminalCondition(timerTerminator);
    
    try {
      controller.execute();
      
      // Evaluate fitness
      System.out.println(calculateFitness(positionLog));
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
    
    System.exit(0);
  }
}
