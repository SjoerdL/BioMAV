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
package nl.ru.ai.projects.parrot.ea2.server;

import java.io.IOException;

//import nl.ru.ai.projects.parrot.ea2.LaurieEA;
import nl.ru.ai.projects.parrot.ea2.LaurieEA;
import nl.ru.ai.projects.parrot.ea2.SjoerdEA;

import org.jgap.FitnessFunction;
import org.jgap.Gene;
import org.jgap.IChromosome;
import org.jgap.InvalidConfigurationException;

public class GAServerMain implements Runnable {
  public static void main(String[] args) {
    GAServerMain serverMain = new GAServerMain();
    serverMain.run();
  }

  private TaskServer taskServer = new TaskServer();
  private GeneticAlgorithm geneticAlgorithm = null;
  private Thread gaThread = null;
  
  private class ServerTaskFitnessEvaluator extends FitnessFunction {
    private static final long serialVersionUID = 5936932791655827294L;

    @Override
    protected double evaluate(IChromosome c) {
      TaskParameters taskParams = new TaskParameters();
      
      Gene[] genes = c.getGenes();
      for (int i = 0; i < genes.length; i++) {
        taskParams.getMap().put("" + i, "" + genes[i].getAllele());
      }
      
      Task task = new Task(taskParams);
      synchronized (task) {
        taskServer.addTask(task);
        try {
          task.wait();

          // Evaluate results
          if (task.hasResult()) {
            double result = 0;
            String[] results = task.getResult().trim().split("\n");
            for (String r : results) {
              result += Double.parseDouble(r.trim());
            }
            
            return result / results.length;
          } else {
            System.err.println("Error! Server task has not computed a result!");
          }
        } catch (InterruptedException e) {
          Thread.currentThread().interrupt();
        }
        catch (NumberFormatException e) {
          e.printStackTrace();
        }
      }
      return 0;
    }
  }
  
  @Override
  public void run() {
    GenomeFactory genFactory = new LaurieEA();
    
    try {
      taskServer.start();
      geneticAlgorithm = new GeneticAlgorithm(new ServerTaskFitnessEvaluator(), genFactory);
      gaThread = new Thread(geneticAlgorithm);
      System.out.println("Server running!");
      gaThread.run();
      taskServer.stop();
    } catch (InvalidConfigurationException e) {
      e.printStackTrace();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}
