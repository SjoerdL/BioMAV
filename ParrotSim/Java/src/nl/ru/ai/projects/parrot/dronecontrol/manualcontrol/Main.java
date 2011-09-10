/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.ru.ai.projects.parrot.dronecontrol.manualcontrol;
/**
 *
 * @author Vincent
 */
public class Main {


      public static void main(String[] args) throws InterruptedException
      {
          Control control = new Control();
          View view = new View(control);
          Model model = new Model(control);
      }
}
