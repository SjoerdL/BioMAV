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
package nl.ru.ai.projects.parrot.biomav;

import nl.ru.ai.projects.parrot.dronecontrol.simulator.Simulator;
import nl.ru.ai.projects.parrot.ea.evolve.EASetup;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.tools.BehaviorModule;
import nl.ru.ai.projects.parrot.tools.BehaviorModule1;
import nl.ru.ai.projects.parrot.tools.BehaviorModuleLB;
import nl.ru.ai.projects.parrot.tools.BehaviorModuleSL;
import nl.ru.ai.projects.parrot.tools.TwitterAccess;
import nl.ru.ai.projects.parrot.tools.Writer;


/**
 *
 * @author sjoerdlagarde
 */
public class Main {
	
    /**
     * @param args the command line arguments
     * @throws InterruptedException 
     */
    public static void main(String[] args) throws InterruptedException {
    	TwitterAccess.getInstance().setEnabled(false); 	// True to have the drone tweet
    	State.setLogDronePosition(false); 				// True to log positions of drone to txt-file
    	State.setOutputCurrentState(false);
    	
    	Simulator sim = Simulator.getInstance();
    	sim.activateRendering(true);
    	sim.setCoupleTime(true);
    	
    	
    	//BehaviorModule module = new BehaviorModule1(sim.getDrone(0), sim.getDrone(0));
    	//BehaviorModule module = new BehaviorModule2(sim.getDrone(0), sim.getDrone(0));
    	//BehaviorModule module = new BehaviorModuleLB(sim.getDrone(0), sim.getDrone(0));
    	BehaviorModule module = new BehaviorModuleSL(sim.getDrone(0), sim.getDrone(0));
    	module.executeFSM();
    	System.exit(0);
    	
    	
    	/*
    	EASetup setup = new EASetup();
    	setup.run();
    	
    	Integer[] timings = setup.getTimings();
    	Writer out = Writer.getInstance();
    	out.setFilename("outputTimings.txt");
    	String timing = setup.getFitnessBestChromosome()+"\n";
    	for ( int i=0; i<timings.length; i++ )
    		timing+=timings[i]+"; ";   	
    	out.write(timing);
    	out.close();
    	
    	System.exit(0);
    	*/
    }

}
