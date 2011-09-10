package atcontrol;

import interfaces.ControlInterface.FlyingState;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import atcontrol.connection.DroneNavData;

@SuppressWarnings("serial")
public class DroneCommanderFrame extends JFrame implements KeyListener, ActionListener
{
	DroneCommander commander;
	
	private JTextArea explanationArea;
	private JPanel emergencyIndicatorPanel;
	
	private JLabel speedLabel;
	private JTextField speedField;
	private JButton setSpeedBtn;
	
	private JLabel flyingLabel;
	private JLabel batteryLowLabel;
	private JLabel notEnoughPowerLabel;
	private JLabel angelsOutOfRangeLabel;
	private JLabel batteryLabel;
	private JLabel altitudeLabel;
	private JLabel pitchLabel;
	private JLabel yawLabel;
	private JLabel rollLabel;
	private JLabel vxLabel;
	private JLabel vyLabel;
	private JLabel vzLabel;

	private JLabel flyingLabel2;
	private JLabel batteryLowLabel2;
	private JLabel notEnoughPowerLabel2;
	private JLabel angelsOutOfRangeLabel2;
	private JLabel batteryLabel2;
	private JLabel altitudeLabel2;
	private JLabel pitchLabel2;
	private JLabel yawLabel2;
	private JLabel rollLabel2;
	private JLabel vxLabel2;
	private JLabel vyLabel2;
	private JLabel vzLabel2;
	
	public DroneCommanderFrame(DroneCommander commander)
	{
		super("Drone Control");
		
		this.commander = commander;
		
		initFrame();
	}
	
	private void initFrame()
	{
		this.setLayout(null);
		this.addWindowListener(new WindowAdapter()
		{
			public void windowClosing(WindowEvent we)
			{
				quitProgram();
			}
		});
		this.setSize(800, 600);

		this.addKeyListener(this);
		
		initComponents();
		updateLabels();

		this.requestFocus();
		
		this.setVisible(true);
	}

	private void initComponents()
	{
		explanationArea = new JTextArea();
		explanationArea.setWrapStyleWord(true);

		String explanationString = 
			"Use W, S, A and D to move forward, backward, to the left or to the right\n" +
			"Use the arrow keys to go up, down, turn left or turn right\n" +
			"Use Page up and Page down to take off or to land\n" + 
			"Use Home and End to clear or enter emergency mode\n" +
			"Spacebar lets the drone simply hover";
		explanationArea.setText(explanationString);
		
		explanationArea.setBounds(10, 10, 400, 85);
		explanationArea.setFocusable(false);
		
		add(explanationArea);	
		
		emergencyIndicatorPanel = new JPanel();
		emergencyIndicatorPanel.setBounds(420, 10, 50, 50);
		emergencyIndicatorPanel.setFocusable(false);
		emergencyIndicatorPanel.setBackground(Color.GREEN);
		add(emergencyIndicatorPanel);
		
		speedLabel = new JLabel("Speed: ");
		speedField = new JTextField(Double.toString(commander.speed));
		speedLabel.setBounds(480, 10, 50, 20);
		speedField.setBounds(525, 10, 40, 20);
		speedLabel.setFocusable(false);
	//	speedField.setFocusable(false);
		add(speedLabel);
		add(speedField);
		
		setSpeedBtn = new JButton("Confirm");
		setSpeedBtn.setBounds(570, 10, 75, 20);
		setSpeedBtn.setFocusable(false);
		setSpeedBtn.addActionListener(this);
		setSpeedBtn.setActionCommand("confirm speed");
		add(setSpeedBtn);
		
		initLabels();
	}
	
	private void initLabels()
	{
		flyingLabel = new JLabel("Flying: ");
		batteryLowLabel = new JLabel("Battery low: ");
		notEnoughPowerLabel = new JLabel("Not enough power: ");
		angelsOutOfRangeLabel = new JLabel("Angels OOR: ");
		batteryLabel = new JLabel("Battery: ");
		altitudeLabel = new JLabel("Altitude: ");
		pitchLabel = new JLabel("Pitch: ");
		yawLabel = new JLabel("Yaw: ");
		rollLabel = new JLabel("Roll: ");
		vxLabel = new JLabel("Vx: ");
		vyLabel = new JLabel("Vy: ");
		vzLabel = new JLabel("Vz: ");

		flyingLabel2 = new JLabel();
		batteryLowLabel2 = new JLabel();
		notEnoughPowerLabel2 = new JLabel();
		angelsOutOfRangeLabel2 = new JLabel();
		batteryLabel2 = new JLabel();
		altitudeLabel2 = new JLabel();
		pitchLabel2 = new JLabel();
		yawLabel2 = new JLabel();
		rollLabel2 = new JLabel();
		vxLabel2 = new JLabel();
		vyLabel2 = new JLabel();
		vzLabel2 = new JLabel();
		
		flyingLabel.setFocusable(false);
		batteryLowLabel.setFocusable(false);
		notEnoughPowerLabel.setFocusable(false);
		angelsOutOfRangeLabel.setFocusable(false);
		batteryLabel.setFocusable(false);
		altitudeLabel.setFocusable(false);
		pitchLabel.setFocusable(false);
		yawLabel.setFocusable(false);
		rollLabel.setFocusable(false);
		vxLabel.setFocusable(false);
		vyLabel.setFocusable(false);
		vzLabel.setFocusable(false);

		flyingLabel2.setFocusable(false);
		batteryLowLabel2.setFocusable(false);
		notEnoughPowerLabel2.setFocusable(false);
		angelsOutOfRangeLabel2.setFocusable(false);
		batteryLabel2.setFocusable(false);
		altitudeLabel2.setFocusable(false);
		pitchLabel2.setFocusable(false);
		yawLabel2.setFocusable(false);
		rollLabel2.setFocusable(false);
		vxLabel2.setFocusable(false);
		vyLabel2.setFocusable(false);
		vzLabel2.setFocusable(false);
		
		flyingLabel.setBounds(10, 100, 75, 20);
		batteryLowLabel.setBounds(10, 125, 75, 20);
		notEnoughPowerLabel.setBounds(10, 150, 125, 20);
		angelsOutOfRangeLabel.setBounds(10, 175, 75, 20);
		batteryLabel.setBounds(10, 200, 75, 20);
		altitudeLabel.setBounds(10, 225, 75, 20);
		pitchLabel.setBounds(10, 250, 75, 20);
		yawLabel.setBounds(10, 275, 75, 20);
		rollLabel.setBounds(10, 300, 75, 20);
		vxLabel.setBounds(10, 325, 75, 20);
		vyLabel.setBounds(10, 350, 75, 20);
		vzLabel.setBounds(10, 375, 75, 20);

		flyingLabel2.setBounds(140, 100, 100, 20);
		batteryLowLabel2.setBounds(140, 125, 100, 20);
		notEnoughPowerLabel2.setBounds(140, 150, 100, 20);
		angelsOutOfRangeLabel2.setBounds(140, 175, 100, 20);
		batteryLabel2.setBounds(140, 200, 100, 20);
		altitudeLabel2.setBounds(140, 225, 100, 20);
		pitchLabel2.setBounds(140, 250, 100, 20);
		yawLabel2.setBounds(140, 275, 100, 20);
		rollLabel2.setBounds(140, 300, 100, 20);
		vxLabel2.setBounds(140, 325, 100, 20);
		vyLabel2.setBounds(140, 350, 100, 20);
		vzLabel2.setBounds(140, 375, 100, 20);
		
		add(flyingLabel);
		add(batteryLowLabel);
		add(notEnoughPowerLabel);
		add(angelsOutOfRangeLabel);
		add(batteryLabel);
		add(altitudeLabel);
		add(pitchLabel);
		add(yawLabel);
		add(rollLabel);
		add(vxLabel);
		add(vyLabel);
		add(vzLabel);

		add(flyingLabel2);
		add(batteryLowLabel2);
		add(notEnoughPowerLabel2);
		add(angelsOutOfRangeLabel2);
		add(batteryLabel2);
		add(altitudeLabel2);
		add(pitchLabel2);
		add(yawLabel2);
		add(rollLabel2);
		add(vxLabel2);
		add(vyLabel2);
		add(vzLabel2);
	}
	
	private void quitProgram()
	{
/*		if(commander.droneConnection != null && commander.droneNavDataReceiver != null)
		{
			arCommander.droneNavDataReceiver.stop();
		}*/
		
		System.exit(0);
	}
	
	public void updateLabels()
	{
		if(emergencyIndicatorPanel != null && emergencyIndicatorPanel.getGraphics() != null)
		{
			if(commander.getFlyingState() == FlyingState.EMERGENCY)
			{
				emergencyIndicatorPanel.getGraphics().setColor(Color.RED);
			}
			else
			{
			/*	if(commander.latestNavData != null && commander.latestNavData.isEmergency())
				{
					emergencyIndicatorPanel.getGraphics().setColor(Color.RED);
				}
				else*/
				{
					emergencyIndicatorPanel.getGraphics().setColor(Color.GREEN);
				}
			}
			
			emergencyIndicatorPanel.getGraphics().fillRect(0, 0, 50, 50);
		}
		
		if(commander.latestNavData != null)
		{
			DroneNavData nd = commander.latestNavData;
			
			flyingLabel2.setText(nd.isFlying() + "");
			batteryLowLabel2.setText(nd.isBatteryTooLow() + "");
			notEnoughPowerLabel2.setText(nd.isNotEnoughPower() + "");
			angelsOutOfRangeLabel2.setText(nd.isAngelsOutOufRange() + "");
			batteryLabel2.setText(nd.getBattery() + "");
			altitudeLabel2.setText(nd.getAltitude() + "");
			pitchLabel2.setText(nd.getPitch() + "");
			yawLabel2.setText(nd.getRoll() + "");
			rollLabel2.setText(nd.getYaw() + "");
			vxLabel2.setText(nd.getVx() + "");
			vyLabel2.setText(nd.getLongitude() + "");
			vzLabel2.setText(nd.getVz() + "");
		}
	}

	@Override
	public void keyPressed(KeyEvent e)
	{
	//	System.out.println("key pressed");
		
		String action = "";

		int keyCode = e.getKeyCode();
		
		switch (keyCode)
		{
			case KeyEvent.VK_W:
				action = "Flying forward";
				commander.flyForward(-commander.speed);
			break;
			case KeyEvent.VK_S:
				action = "Flying backward";
				commander.flyForward(commander.speed);
			break;
			case KeyEvent.VK_A:
				action = "Strafing left";
				commander.flySideways(-commander.speed);
			break;
			case KeyEvent.VK_D:
				action = "Strafing right";
				commander.flySideways(commander.speed);
			break;
			case KeyEvent.VK_UP: // Up
				action = "Flying Higher";
				commander.changeHeight(commander.speed);
			break;
			case KeyEvent.VK_DOWN: // Down
				action = "Flying Lower";
				commander.changeHeight(-commander.speed);
			break;
			case KeyEvent.VK_LEFT: // Left
				action = "Rotate left";
				commander.spin(-commander.speed);
			break;
			case KeyEvent.VK_RIGHT: // Right
				action = "Rotate right";
				commander.spin(commander.speed);
			break;
			case KeyEvent.VK_SPACE: // SpaceBar
				action = "Hovering";
				commander.hover();
			break;
			case KeyEvent.VK_PAGE_UP: // PageUp
				action = "Takeoff";
				commander.takeoff();
			break;
			case KeyEvent.VK_PAGE_DOWN: // PageDown
				action = "Landing";
				commander.land();
			break;
			case KeyEvent.VK_HOME: // PageDown
				action = "Clearing Emergency";
				commander.clearEmergencyFlag();
			break;
			case KeyEvent.VK_END: // PageDown
				action = "Entering Emergency";
				commander.setEmergencyFlag();
			break;
			default:
			break;
		}

		updateLabels();
		
	//	System.out.println("Speed: " + speed);
		System.out.println("Action: " + action);
	//	send_at_cmd(at_cmd);
	}

	@Override
	public void keyReleased(KeyEvent arg0)
	{
		commander.hover();
	}

	@Override
	public void keyTyped(KeyEvent arg0)
	{
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		if(e.getActionCommand().equals("confirm speed"))
		{			
			try
			{
				commander.speed = Double.parseDouble(speedField.getText());
			}
			catch(NumberFormatException ee)
			{
				ee.printStackTrace();
			}
		}
		
		this.requestFocus();
	}
}
