package atcontrol.commands;

public class MoveForwardCommand
{
	public static String createCommand(int commandNr, int speed)
	{
		return "AT*PCMD=" + commandNr + ",1,0," + speed + ",0,0";
	}
}
