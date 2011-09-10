package atcontrol.commands;

public class WatchdogCommand
{
	public static String createCommand(int commandNr)
	{
		return "AT*COMWDG=" + commandNr;
	}
}
