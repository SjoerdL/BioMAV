package nl.ru.ai.projects.parrot.ea2.client;

import java.util.Map;

import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.FSMController;

public interface FSMFactory {
  public FSMController createFSM(ParrotDroneInterface pdi, PolePositionPollInterface pppi, Map<String, String> configurationData);
}
