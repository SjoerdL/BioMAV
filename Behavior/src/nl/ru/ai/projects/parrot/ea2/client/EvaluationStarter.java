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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;
import java.util.Scanner;

public class EvaluationStarter {
  private ProcessBuilder evaluatorBuilder;
  
  public EvaluationStarter(List<String> evalCommand) {
    evaluatorBuilder = new ProcessBuilder(evalCommand);
  }
  
  public String run(String input) {
    String result = "";
    try {
      Process process = evaluatorBuilder.start();
      
      OutputStream outStream = process.getOutputStream();
      outStream.write(input.getBytes());
      outStream.close();

      InputStream inStream = process.getInputStream();
      Scanner inScanner = new Scanner(inStream);
      
      process.waitFor();
      
      while (inScanner.hasNextLine()) {
        result += inScanner.nextLine().replaceAll("[\n\r]", "") + "\n";
      }
      
      return result;
    } catch (IOException e) {
      e.printStackTrace();
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
    return null;
  }
}
