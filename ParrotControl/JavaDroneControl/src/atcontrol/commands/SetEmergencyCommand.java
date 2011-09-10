package atcontrol.commands;

public class SetEmergencyCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*REF=" + (commandNr) + ",256";
	}
}
