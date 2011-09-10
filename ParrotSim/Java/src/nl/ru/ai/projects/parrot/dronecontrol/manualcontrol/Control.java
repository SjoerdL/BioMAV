/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.ru.ai.projects.parrot.dronecontrol.manualcontrol;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

/**
 *
 * @author Vincent
 */
public class Control implements KeyListener {

    private boolean left = false;     // 37
    private boolean up = false;       // 38
    private boolean right = false;    // 39
    private boolean down = false;     // 40
    private boolean zButton = false;  // 90
    private boolean xButton = false;  // 88
    private boolean aButton = false;  // 65
    private boolean dButton = false;  // 68

    public boolean getUp() {
        return up;
    }

    public boolean getDown() {
        return down;
    }

    public boolean getLeft() {
        return left;
    }

    public boolean getRigth() {
        return right;
    }

    public boolean getXButton() {
        return xButton;
    }

    public boolean getZButton() {
        return zButton;
    }

    public boolean getAButton() {
        return aButton;
    }

    public boolean getDButton() {
        return dButton;
    }

    public void keyTyped(KeyEvent e) {
        // System.out.println("Typed "+ e.getKeyCode());
    }

    public void keyPressed(KeyEvent e) {
        System.out.println("Typed " + e.getKeyCode());

        switch (e.getKeyCode()) {

            case 37:
                left = true;
                break;
            case 38:
                up = true;
                break;
            case 39:
                right = true;
                break;
            case 40:
                down = true;
                break;
            case 88:
                xButton = true;
                break;
            case 90:
                zButton = true;
                break;
            case 65:
                aButton = true;
                break;
            case 68:
                dButton = true;
                break;
            default:
                ;
                break;
        }
    }

    public void keyReleased(KeyEvent e) {
        switch (e.getKeyCode()) {

            case 37:
                left = false;
                break;
            case 38:
                up = false;
                break;
            case 39:
                right = false;
                break;
            case 40:
                down = false;
                break;
            case 88:
                xButton = false;
                break;
            case 90:
                zButton = false;
                break;
            case 65:
                aButton = false;
                break;
            case 68:
                dButton = false;
                break;
            default:
                ;
                break;
        }
    }
}
