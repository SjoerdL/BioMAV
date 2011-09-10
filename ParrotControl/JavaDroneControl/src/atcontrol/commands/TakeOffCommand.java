package atcontrol.commands;

public class TakeOffCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*REF=" + commandNr + ",290718208";
	}
}
