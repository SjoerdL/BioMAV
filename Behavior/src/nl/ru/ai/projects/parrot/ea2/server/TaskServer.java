package nl.ru.ai.projects.parrot.ea2.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Scanner;

public class TaskServer implements Runnable {
  public final static int PORT = 5531;
  
  private ServerSocket server = null;
  private Thread acceptThread = null;
  
  private List<Task> tasks = new ArrayList<Task>();
  
  private class ClientTaskThread implements Runnable {
    private Task task;
    private Socket socket;
    
    public ClientTaskThread(Socket socket, Task task) {
      this.task = task;
      this.socket = socket;
      
      task.markComputationStart();
      tasks.remove(task);
    }

    @Override
    public void run() {
      try {
        socket.setSoTimeout(500);
        
        OutputStream outStream = socket.getOutputStream();
        InputStream inStream = socket.getInputStream();
        Scanner inScanner = new Scanner(inStream);
        
        {
          String sendString = task.getTaskParameters().toString();
          sendString = "Lines " + task.getTaskParameters().getMap().size() + "\n" + sendString;
          outStream.write(sendString.getBytes());   
          socket.shutdownOutput();
        }
        
        // Wait for result
        try {
          while (!Thread.interrupted() && !task.hasResult() && task.isComputing()) {
            String line = inScanner.nextLine().replaceAll("[\n\r]", "");
            task.pushResultString(line);
          }
        }
        catch (NoSuchElementException e) {
        }
        
        socket.close();
      } catch (IOException e) {
        e.printStackTrace();
      }
      
      if (!task.hasResult()) {
        tasks.add(task);
      }
    }
  }
  
  public synchronized void start() throws IOException {
    server = new ServerSocket(PORT);
    server.setSoTimeout(100);
    
    acceptThread = new Thread(this);
    acceptThread.setDaemon(true);
    acceptThread.start();
  }
  
  public synchronized void stop() {
    if (server != null) {
      acceptThread.interrupt();
      try {
        acceptThread.join();
        server.close();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }

  @Override
  public void run() {
    while (!Thread.interrupted()) {
      try {
        Socket clientSocket = server.accept();
        
        synchronized (tasks) {
          boolean foundTask = false;
          for (Task task : tasks.toArray(new Task[0])) {
            if (!task.isComputing() && !task.hasResult()) {
              ClientTaskThread ctt = new ClientTaskThread(clientSocket, task);
              Thread t = new Thread(ctt);
              t.setDaemon(true);
              t.start();
              foundTask = true;
              break;
            }
          }
          if (!foundTask) {
            clientSocket.getOutputStream().write("None\n".getBytes());
            clientSocket.close();
          }
        }
      } catch (IOException e) {
      }
    }
  }
  
  public void addTask(Task task) {
    synchronized (tasks) {
      tasks.add(task);
    }
  }
}
