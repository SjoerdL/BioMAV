package nl.ru.ai.projects.parrot.ea2.client;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.NoSuchElementException;
import java.util.Scanner;

import nl.ru.ai.projects.parrot.ea2.server.Task;
import nl.ru.ai.projects.parrot.ea2.server.TaskParameters;
import nl.ru.ai.projects.parrot.ea2.server.TaskServer;

public class TaskClient {
  private Socket receiveSocket = null;
  
  private OutputStream socketOutStream = null;

  private TaskParameters connectionParameters = null;
  
  private synchronized void disconnect() {
    if (receiveSocket != null) {
      try {
        receiveSocket.close();
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
    socketOutStream = null;
    connectionParameters = null;
  }
  
  public synchronized boolean connect(String target) {
    disconnect();
    
    try {
      receiveSocket = new Socket();
      receiveSocket.setSoTimeout(5000);
      receiveSocket.connect(new InetSocketAddress(target, TaskServer.PORT));
      
      InputStream inStream = receiveSocket.getInputStream();
      Scanner inScanner = new Scanner(inStream);
      String line = inScanner.nextLine().trim();
      if ("None".equals(line)) {
        receiveSocket.close();
        receiveSocket = null;
        return false;
      }
      if (line.matches("^Lines\\s+[0-9]+$")) {
        int lineCount = Integer.parseInt(line.split("\\s+")[1]);
        String inLines = "";
        for (int i = 0; i < lineCount; i++) {
          inLines += inScanner.nextLine().replaceAll("[\n\r]", "") + "\n";
        }
        
        connectionParameters = new TaskParameters(inLines);
        socketOutStream = receiveSocket.getOutputStream();
        
        return true;
      }
      
      return false;
    } catch (NoSuchElementException e) {
      e.printStackTrace();
    } catch (IOException e) {
      e.printStackTrace();
    }
    disconnect();
    return false;
  }

  public synchronized TaskParameters getTaskParameters() {
    return connectionParameters;
  }
  
  public synchronized void sendResultsAndFinish(String results) {
    if (receiveSocket != null) {
      while ((results.length() > 0) && results.substring(results.length() - 1).matches("[\n\r]")) {
        results = results.substring(0, results.length() - 1);
      }
      results += "\n" + Task.RESULT_FINISHED_STRING + "\n";
      try {
        socketOutStream.write(results.getBytes());
        socketOutStream.flush();
      } catch (IOException e) {
        e.printStackTrace();
      }
      disconnect();
    }
  }
}
