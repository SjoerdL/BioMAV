package atcontrol.commands;

public class LandCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*REF=" + commandNr + ",290717696";
	}
}
