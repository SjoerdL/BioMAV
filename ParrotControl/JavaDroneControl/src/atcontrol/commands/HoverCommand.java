package atcontrol.commands;

public class HoverCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*PCMD=" + commandNr + ",0,0,0,0,0";
	}
}
