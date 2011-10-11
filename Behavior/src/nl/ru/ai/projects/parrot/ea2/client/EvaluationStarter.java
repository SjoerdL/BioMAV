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
