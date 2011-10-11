package nl.ru.ai.projects.parrot.ea.evolve;

import org.jgap.Chromosome;
import org.jgap.Configuration;
import org.jgap.Gene;
import org.jgap.InvalidConfigurationException;
import org.jgap.impl.IntegerGene;

public class Chromosomes {
	/**
	 * Creates a new chromosome
	 * @param config The configuration file to be used by the genes
	 * @return A new chromosome
	 * @throws InvalidConfigurationException
	 */
	public static Chromosome createChromosome(Configuration config) throws InvalidConfigurationException {
		Gene[] sampleGenes = new Gene[8];
		
		sampleGenes[0] = new IntegerGene(config, 0, 50);			// Pole found left (angle)
		sampleGenes[1] = new IntegerGene(config, 0, 50);			// Pole found right (angle)
		sampleGenes[2] = new IntegerGene(config, 0, 30); 			// Out of sight left
		sampleGenes[3] = new IntegerGene(config, 0, 30); 			// Out of sight right
		sampleGenes[4] = new IntegerGene(config, 0, 30); 			// Pole lost left
		sampleGenes[5] = new IntegerGene(config, 0, 30); 			// Pole lost right
		sampleGenes[6] = new IntegerGene(config, 50, 350); 			// Close to pole left
		sampleGenes[7] = new IntegerGene(config, 50, 350);			// Close to pole right
		
		return new Chromosome(config, sampleGenes);
	}
}
