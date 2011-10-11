package nl.ru.ai.projects.parrot.ea2.server;

import org.jgap.Configuration;
import org.jgap.IChromosome;
import org.jgap.InvalidConfigurationException;

public interface GenomeFactory {
  public IChromosome createGenome(Configuration config) throws InvalidConfigurationException;
}
