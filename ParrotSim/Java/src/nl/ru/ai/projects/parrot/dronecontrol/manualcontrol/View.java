/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.ru.ai.projects.parrot.dronecontrol.manualcontrol;

import java.awt.Color;
import java.awt.Graphics;
import javax.swing.JFrame;

/**
 *
 * @author Vincent
 */
public class View extends JFrame {

    /*
     * Deze classe moet eigenlijk mt de echte view vervangen wordne maar die ben ik even kwijt.
     */
    public View(Control control) {
        super("Manual SimControl");
        setSize(300, 500);
        setVisible(true);
        this.addKeyListener(control);
        this.repaint();
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);
        g.setColor(Color.RED);
        g.drawString("Click here to controle the drone!", 50, 50);
        g.setColor(Color.BLACK);
        int y = 100;
        g.drawString("Up = flyForward(1.0)", 50, y);
        g.drawString("Down = flyForward(-1.0)", 50, y + 30);
        g.drawString("Right = spin(1.0)", 50, y + 60);
        g.drawString("Left = spin(-1.0)", 50, y + 90);
        g.drawString("AButton() = flySideways(-1.0)", 50, y + 120);
        g.drawString("DButton() = flySideways(1.0)", 50, y + 150);
        g.drawString("XButton() = land()", 50, y + 180);
        g.drawString("ZButton() = takeoff()", 50, y + 210);
    
    }
}
