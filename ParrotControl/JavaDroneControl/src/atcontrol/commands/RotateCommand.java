package atcontrol.commands;

public class RotateCommand
{
	public static String createCommand(int commandNr, int speed)
	{
		return "AT*PCMD=" + commandNr + ",1,0,0,0," + speed;
	}
}
