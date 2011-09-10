package atcontrol.commands;

public class AcknowledgeCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*CTRL=" + commandNr + ",5,0";
	}
}
