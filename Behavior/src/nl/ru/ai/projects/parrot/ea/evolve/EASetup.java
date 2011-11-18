/*
    This file is part of the BioMAV project.

    The BioMAV project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The BioMAV project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The BioMAV project. If not, see <http://www.gnu.org/licenses/>.
*/
package nl.ru.ai.projects.parrot.ea.evolve;

import java.util.ArrayList;

import nl.ru.ai.projects.parrot.ea.fitness.FitnessDrone;
import nl.ru.ai.projects.parrot.tools.TerminalCondition;
import nl.ru.ai.projects.parrot.tools.TerminatorNumberOfIterations;

import org.jgap.FitnessFunction;
import org.jgap.GeneticOperator;
import org.jgap.Genotype;
import org.jgap.IChromosome;
import org.jgap.InvalidConfigurationException;
import org.jgap.impl.CrossoverOperator;
import org.jgap.impl.MutationOperator;

public class EASetup {

	private EAConfiguration config;
	private IChromosome finalChromosome;
	
	public static boolean interrupt = false;

	public EASetup() {
		try {
			config = new EAConfiguration();
			
			FitnessFunction fitness = new FitnessDrone(config);
			GeneticOperator crossover = new CrossoverOperator(config, 0.35d);
			GeneticOperator mutation = new MutationOperator(config, 12);
			
			config.addGeneticOperator(crossover);
			config.addGeneticOperator(mutation);
			config.setFitnessFunction(fitness);
			
			config.setSampleChromosome(Chromosomes.createChromosome(config));
			config.setPopulationSize(config.POPULATION_SIZE);
			
			TerminatorNumberOfIterations numberOfIterationsTerminator = new TerminatorNumberOfIterations();
			numberOfIterationsTerminator.setNumberOfIterations(config.MAX_EVOLUTIONS);
			config.setStopCondition(numberOfIterationsTerminator);			
		} catch (InvalidConfigurationException e) {
			System.err.println("Error in configuration of JGAP. EA did not run");
		}
	}
	
	public void run() {
		try {
			Genotype population = Genotype.randomInitialGenotype(config);
			IChromosome bestSolutionSoFar = null;
			
			int iteration = 1;
			while ( !reachedTerminalCondition() ) {
				population.evolve();
				bestSolutionSoFar = population.getFittestChromosome();

				if ( config.OUTPUT_FITNESS_HISTORY ) {
					System.out.println("Best fitness at iteration "+iteration+" is "+bestSolutionSoFar.getFitnessValue());
					if ( config.OUTPUT_TIMINGS_ATITERATION!=0 && iteration%config.OUTPUT_TIMINGS_ATITERATION==0 )
						displayTimings(bestSolutionSoFar);
				}
				iteration++;
			}
			
			finalChromosome = bestSolutionSoFar;
			if ( finalChromosome == null ) {
				if ( population.getPopulation().size() > 0 )
					finalChromosome = population.getPopulation().getChromosome(0);
			}
			
			if ( config.OUTPUT_FITNESS_END )
				System.out.println("Fittest individual has a fitness of "+finalChromosome.getFitnessValue());
		} catch (InvalidConfigurationException e) {
			System.err.println("Error in JGAP configuration while trying to start the evolution. EA did not run");
		}
	}
	
	/**
	 * Returns the timings that were found by the EA
	 * @return An array of integers representing the timings of the EA
	 */
	public Integer[] getTimings() {
		if ( finalChromosome == null )
			return new Integer[0];
		
		Integer[] timings = new Integer[finalChromosome.size()];
		for ( int i=0; i<finalChromosome.size(); i++ )
			timings[i] = (Integer)finalChromosome.getGene(i).getAllele();
		
		return timings;
	}
	
	/**
	 * Returns the fitness value associated with the best individual
	 * @return The fitness value of the best individual
	 */
	public double getFitnessBestChromosome() {
		return finalChromosome.getFitnessValueDirectly();
	}
	
	/**
	 * Checks whether or not a terminal condition is reached
	 * @return True when a terminal condition is reached, false otherwise
	 */
	private boolean reachedTerminalCondition() {
		if ( interrupt )
			return true;
		
		ArrayList<TerminalCondition> stopConditions = config.getStopConditions();
		for ( int i=0; i<stopConditions.size(); i++ ) {
			if ( stopConditions.get(i).terminate() )
				return true;
		}
		return false;
	}
	
	/**
	 * Displays the timings of an individual
	 */
	private void displayTimings(IChromosome individual) {
		System.out.print("Timings: ");
		for ( int i=0; i<individual.size(); i++ )
			System.out.print((Integer)individual.getGene(i).getAllele()+"; ");
		System.out.print("\n");
	}
}
