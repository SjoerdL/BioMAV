package nl.ru.ai.projects.parrot.ea2;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.jgap.BaseChromosome;
import org.jgap.BulkFitnessFunction;
import org.jgap.Chromosome;
import org.jgap.Configuration;
import org.jgap.FitnessEvaluator;
import org.jgap.FitnessFunction;
import org.jgap.Gene;
import org.jgap.Genotype;
import org.jgap.IChromosome;
import org.jgap.IGeneConstraintChecker;
import org.jgap.InvalidConfigurationException;
import org.jgap.NaturalSelector;
import org.jgap.Population;
import org.jgap.UnsupportedRepresentationException;
import org.jgap.event.EventManager;
import org.jgap.impl.CrossoverOperator;
import org.jgap.impl.DoubleGene;
import org.jgap.impl.FittestPopulationMerger;
import org.jgap.impl.GABreeder;
import org.jgap.impl.MutationOperator;
import org.jgap.impl.StockRandomGenerator;
import org.jgap.impl.WeightedRouletteSelector;
import org.jgap.impl.job.EvolveData;
import org.jgap.impl.job.EvolveResult;
import org.jgap.impl.job.IEvolveJob;
import org.jgap.impl.job.IPopulationSplitter;

public class Test implements Runnable {
  public static void main(String[] args) {
    Test test = new Test();
    test.run();
  }
  
  public static class ParalellBreeder extends GABreeder {
    @Override
    protected void updateChromosomes(Population a_pop, Configuration a_conf) {
      int currentPopSize = a_pop.size();

      BulkFitnessFunction bulkFunction = a_conf.getBulkFitnessFunction();
      boolean bulkFitFunc = (bulkFunction != null);
      if (!bulkFitFunc) {
        List<Thread> threads = new ArrayList<Thread>();
        
        for (int i = 0; i < currentPopSize; i++) {
          final IChromosome chrom = a_pop.getChromosome(i);
          Thread t = new Thread() {
            @Override
            public void run() {
              chrom.getFitnessValue();
            }
          };
          
          t.start();
          threads.add(t);
        }
        
        try {
          for (Thread t : threads) {
            t.join();
          }
        }
        catch (InterruptedException e) {
          Thread.currentThread().interrupt();
        }
      }
      super.updateChromosomes(a_pop, a_conf);
    }
  }
  
  @Override
  public void run() {
    try {
      Configuration config = new Configuration();

      config.setBreeder(new ParalellBreeder());
      config.setPopulationSize(10);
      
      MutationOperator mOp = new MutationOperator(config);
      mOp.setMutationRate(10);
      config.addGeneticOperator(mOp);
      
      CrossoverOperator cOp = new CrossoverOperator(config, 1);
      config.addGeneticOperator(cOp);

      config.addNaturalSelector(new WeightedRouletteSelector(config), false);
      
      config.setEventManager(new EventManager());
      config.setRandomGenerator(new StockRandomGenerator());
      
      config.setFitnessFunction(new FitnessFunction() {
        private static final long serialVersionUID = -5862377151466128097L;

        @Override
        protected double evaluate(IChromosome c) {
//          System.out.println("Evaluate start");
          try {
            Thread.sleep(100);
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
//          System.out.println("Evaluate stop");
          return ((Double) c.getGene(0).getAllele()) * ((Double) c.getGene(1).getAllele());
        }
      });

      config.setFitnessEvaluator(new FitnessEvaluator() {
        private static final long serialVersionUID = -7074360999257740401L;

        @Override
        public boolean isFitter(IChromosome c1, IChromosome c2) {
          return isFitter(c1.getFitnessValue(), c2.getFitnessValue());
        }
        
        @Override
        public boolean isFitter(double v1, double v2) {
          return v1 > v2;
        }
      });
      
      Gene[] sampleGenes = new Gene[2];
      sampleGenes[0] = new DoubleGene(config, 0, 100);
      sampleGenes[1] = new DoubleGene(config, 0, 100);
      Chromosome sampleChrom = new Chromosome(config, sampleGenes);
      config.setSampleChromosome(sampleChrom);

      config.setPreservFittestIndividual(true);
      
      Genotype genotype = Genotype.randomInitialGenotype(config);
      
      int i = 0;
      while (i++ < 1000) {
        genotype.evolve();

        System.out.println("Step " + i);
        System.out.println("  Best individual " + genotype.getFittestChromosome().getFitnessValue());
        System.out.println("  Population size " + genotype.getPopulation().size());
      }
    } catch (InvalidConfigurationException e) {
      e.printStackTrace();
    }
  }
}
