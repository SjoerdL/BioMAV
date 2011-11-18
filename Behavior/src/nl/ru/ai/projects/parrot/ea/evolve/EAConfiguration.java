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

import nl.ru.ai.projects.parrot.tools.TerminalCondition;

import org.jgap.Configuration;
import org.jgap.DefaultFitnessEvaluator;
import org.jgap.InvalidConfigurationException;
import org.jgap.event.EventManager;
import org.jgap.impl.BestChromosomesSelector;
import org.jgap.impl.ChromosomePool;
import org.jgap.impl.GABreeder;
import org.jgap.impl.StockRandomGenerator;
import org.jgap.util.ICloneable;

public class EAConfiguration extends Configuration implements ICloneable {

	private static final long serialVersionUID = 1L;
	
	private ArrayList<TerminalCondition> stopConditions;
	
	public final int MAX_EVOLUTIONS = 30;  //150		// Maximum number of evolutions
	public final int POPULATION_SIZE = 25; //50
	public final boolean ELITISM = true;
	
	public final int FSM_TIMER_THRESHOLD = 300000;//600000; 	// The time in ms the FSM will run for each individual on each evolutionary cycle
	
	public final boolean OUTPUT_FITNESS_HISTORY = true; 	// True when fitness of best performing individual at each step in evolutionary cycle should be displayed
	public final boolean OUTPUT_FITNESS_END = true; 		// True when fitness of best performing individual at end of evolutionary cycle should be displayed
	public final int OUTPUT_TIMINGS_ATITERATION = 10; 		// Indicates at what iteration-interval the timings of the best performing individual at that point should be displayed (0=no displaying at all)
	
	public EAConfiguration() {
		super();
		try {
			setBreeder(new GABreeder());
			setRandomGenerator(new StockRandomGenerator());
			setEventManager(new EventManager());
			BestChromosomesSelector bestChromsSelector = new BestChromosomesSelector(this, 0.70d);
			bestChromsSelector.setDoubletteChromosomesAllowed(false);
			addNaturalSelector(bestChromsSelector, false);
			setMinimumPopSizePercent(80);
			setSelectFromPrevGen(0.8d);
			setKeepPopulationSizeConstant(true);
			setFitnessEvaluator(new DefaultFitnessEvaluator());
			setAlwaysCaculateFitness(false);
			setChromosomePool(new ChromosomePool());
			
			setPreservFittestIndividual(ELITISM);
			
			stopConditions = new ArrayList<TerminalCondition>();
		} catch ( InvalidConfigurationException e ) {
			throw new RuntimeException("Fatal error: could not load configuration for the evoluationary algorithm");
		}
	}
	
	/**
	 * Returns all stop conditions
	 * @return A list of stop conditions
	 */
	public ArrayList<TerminalCondition> getStopConditions() {
		return stopConditions;
	}
	
	/**
	 * Adds a stop condition to the list of stop conditions
	 * @param condition The stop condition to add
	 */
	public void setStopCondition(TerminalCondition condition) {
		stopConditions.add(condition);
	}
}
