package atcontrol.commands;

public class MaxAltitudeCommand
{
	public static String createCommand(int commandNr, int maxAltitude)
	{
		return "AT*CONFIG=" + commandNr + ",\"control:altitude_max\",\"" + maxAltitude +  "\""; 
	}
}
