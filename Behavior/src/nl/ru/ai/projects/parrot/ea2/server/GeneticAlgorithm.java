package nl.ru.ai.projects.parrot.ea2.server;

import org.jgap.Configuration;
import org.jgap.FitnessEvaluator;
import org.jgap.FitnessFunction;
import org.jgap.Gene;
import org.jgap.Genotype;
import org.jgap.IChromosome;
import org.jgap.InvalidConfigurationException;
import org.jgap.event.EventManager;
import org.jgap.impl.CrossoverOperator;
import org.jgap.impl.MutationOperator;
import org.jgap.impl.StockRandomGenerator;
import org.jgap.impl.WeightedRouletteSelector;



public class GeneticAlgorithm implements Runnable {
	private final int POPULATIONSIZE = 1;
	
  private Configuration config = new Configuration();
  private Genotype genotype;
  
  public GeneticAlgorithm(FitnessFunction asynchronousFitnessFunction, GenomeFactory genFactory) throws InvalidConfigurationException {
    config.setBreeder(new ParallelBreeder());
    
    config.setEventManager(new EventManager());
    
    config.setFitnessFunction(asynchronousFitnessFunction);
    
    config.setFitnessEvaluator(new FitnessEvaluator() {
      private static final long serialVersionUID = -5453431788537409818L;

      @Override
      public boolean isFitter(IChromosome c1, IChromosome c2) {
        return isFitter(c1.getFitnessValue(), c2.getFitnessValue());
      }
      
      @Override
      public boolean isFitter(double v1, double v2) {
        return v1 > v2;
      }
    });
    
    {
      MutationOperator mOp = new MutationOperator(config, 20);
      config.addGeneticOperator(mOp);
    }
    {
      CrossoverOperator cOp = new CrossoverOperator(config, 40);
      config.addGeneticOperator(cOp);
    }
    
    config.setRandomGenerator(new StockRandomGenerator());
    
    config.addNaturalSelector(new WeightedRouletteSelector(config), true);

    config.setPreservFittestIndividual(true);
    
    config.setPopulationSize(POPULATIONSIZE);
    
    config.setSampleChromosome(genFactory.createGenome(config));
    
    genotype = Genotype.randomInitialGenotype(config);
  }

  @Override
  public void run() {
    int generationCounter = 0;
    while (!Thread.interrupted()) {
      genotype.evolve();
      generationCounter++;
      System.out.println("---------------------------------");
      System.out.println("Evolution step " + generationCounter + "  Best individual: " + genotype.getFittestChromosome().getFitnessValue());
      
      int i = 1;
      for (IChromosome c : genotype.getPopulation().getChromosomes()) { 
        System.out.println("Individual " + (i++) + " - Fitness: " + c.getFitnessValue());
        Gene[] genes = c.getGenes();
        
        TaskParameters parameters = new TaskParameters();
        for (int j = 0; j < genes.length; j++) {
          parameters.getMap().put("" + j, "" + genes[j].getAllele());
        }
        System.out.println(parameters);
      }
    }
  }
}
