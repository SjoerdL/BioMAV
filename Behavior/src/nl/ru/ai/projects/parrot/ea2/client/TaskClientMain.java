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
package nl.ru.ai.projects.parrot.ea2.client;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

public class TaskClientMain implements Runnable {
  private EvaluationStarter evalStarter;
  private TaskClient taskClient;
  private String remoteAddress;
  
  public TaskClientMain(String remoteAddress, List<String> evalCommand) {
    evalStarter = new EvaluationStarter(evalCommand);
    taskClient = new TaskClient();
    this.remoteAddress = remoteAddress;
  }
  
  @Override
  public void run() {
    while (!Thread.interrupted()) {
      if (taskClient.connect(remoteAddress)) {
        System.out.println("Starting new evaluation");
        String result = evalStarter.run(taskClient.getTaskParameters().toString());
        if (result == null) {
          System.out.println("Evaluation failed!");
        } else {
          System.out.println("Evaluation done!");
          taskClient.sendResultsAndFinish(result);
        }
      } else {
        try {
          Thread.sleep(1000);
        } catch (InterruptedException e) {
          Thread.currentThread().interrupt();
        }
      }
    }
  }

  public static void main(String[] argss) {
	  String [] args = {"localhost", "/C:/Users/Laurie/workspace/Behavior2/src/nl/ru/ai/projects/parrot/ea2/evaluator/EvaluatorMain.java" };
    if (args.length < 2) {
      System.out.println("Please specify remote server address and evaluator command");
    } else {
      String remoteAddress = args[0];
      List<String> evaluatorCommand = new ArrayList<String>(Arrays.asList(args));
      evaluatorCommand.remove(0);
      
      TaskClientMain tcm = new TaskClientMain(remoteAddress, evaluatorCommand);
      Thread tcmThread = new Thread(tcm);
      tcmThread.start();
      
      System.out.println("(Press enter to quit)");
      Scanner inScanner = new Scanner(System.in);
      inScanner.nextLine();
      
      tcmThread.interrupt();
    }
  }
}
