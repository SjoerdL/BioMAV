package atcontrol.commands;

public class ClearEmergencyCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*REF=" + (commandNr) + ",0";
	}
}
