package atcontrol.commands;

public class MoveSidewaysCommand
{
	public static String createCommand(int commandNr, int speed)
	{
		return "AT*PCMD=" + commandNr + ",1," + speed + ",0,0,0";
	}
}
