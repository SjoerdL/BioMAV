package atcontrol.commands;

public class ExitBootstrapCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*CONFIG=" + (commandNr) + "\"general:navdata_demo\",\"TRUE\"";
	}
}
