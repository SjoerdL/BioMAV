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
package nl.ru.ai.projects.parrot.ea.fitness;

import java.util.ArrayList;


import nl.ru.ai.projects.parrot.ea.evolve.EAConfiguration;
import nl.ru.ai.projects.parrot.ea.evolve.EASetup;
import nl.ru.ai.projects.parrot.tools.PositionLogger;

import org.jgap.IChromosome;

public class FitnessDrone extends EAFitness {

	private static final long serialVersionUID = 1L;

	public FitnessDrone(EAConfiguration config) {
		super(config);
	}	

	@Override
	protected double evaluate(IChromosome aSubject) {
		// The evaluate method is called for each individual in the population
		// When interrupted this method should not execute anymore
		if ( EASetup.interrupt ) 	
			return 0.00; 
		
		// Run simulation
		try {
			PositionLogger.resetPositions();
			setupEA(aSubject);
		} catch (InterruptedException e) {
			System.err.println("Interrupted");
			EASetup.interrupt = true; 		// Cannot throw exception again
			return 0.00;
		}
		
		// Calculate fitness based on coordinates
		ArrayList<double[]> coords = PositionLogger.getPositions();
		double fitness = 0.0;
		
		double pole1 = 5.0, pole2 = -5.0; 	// Coordinates of poles: (-5, 0) and (5, 0)
		double threshold = 0.01; //1 cm
		int caseCounter = 0;
		for ( int i=0; i<coords.size(); i++ ) {
			double[] coord = coords.get(i);
			if(i > 0)
			{
				if(Math.abs(coord[0] - coords.get(i-1)[0]) > threshold )
						//|| Math.abs(coord[1] - coords.get(i-1)[1]) > threshold)
					fitness += 0.001;
			}
			
			switch( caseCounter ) {
				case 0:  		// Pole 1, left pass
					if ( coord[1] > 0 && coord[0] > pole1 ) {
						fitness++;
						caseCounter++;
					}
					break;
				case 1: 		// Pole 1, right pass (return to center)
					if ( coord[1] < 0 && coord[0] < pole1 ) {
						fitness++;
						caseCounter++;
					}
					break;
				case 2: 		// Pole 2, right pass
					if ( coord[1] > 0 && coord[0] < pole2 ) {
						fitness++;
						caseCounter++;
					}
					break;
				case 3: 		// Pole 2, left pass (return to center)
					if ( coord[1] < 0 && coord[0] > pole2 ) {
						fitness++;
						caseCounter = 0;
					}
					break;
				
				default: 
					caseCounter = 0;
			}
		}
		
		System.out.print(".");
		
		return fitness;
	}	
}
