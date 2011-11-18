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
package nl.ru.ai.projects.parrot.ea2;

import java.util.Map;

import nl.ru.ai.projects.parrot.behaviors.Hover;
import nl.ru.ai.projects.parrot.behaviors.InitializeDrone;
import nl.ru.ai.projects.parrot.behaviors.PassLeft;
import nl.ru.ai.projects.parrot.behaviors.PassRight;
import nl.ru.ai.projects.parrot.behaviors.SearchPole;
import nl.ru.ai.projects.parrot.behaviors.TurnLeft;
import nl.ru.ai.projects.parrot.behaviors.TurnRight;
import nl.ru.ai.projects.parrot.behaviors.TwitterBehavior;
import nl.ru.ai.projects.parrot.dronecontrol.ParrotDroneInterface;
import nl.ru.ai.projects.parrot.dronecontrol.PolePositionPollInterface;
import nl.ru.ai.projects.parrot.fsm.Behavior;
import nl.ru.ai.projects.parrot.fsm.FSMController;
import nl.ru.ai.projects.parrot.fsm.State;
import nl.ru.ai.projects.parrot.fsm.Transition;
import nl.ru.ai.projects.parrot.transitions.OutOfSight;
import nl.ru.ai.projects.parrot.transitions.PoleFound;
import nl.ru.ai.projects.parrot.transitions.TurnComplete;

import org.jgap.Chromosome;
import org.jgap.Configuration;
import org.jgap.Gene;
import org.jgap.IChromosome;
import org.jgap.InvalidConfigurationException;
import org.jgap.impl.IntegerGene;

public class SjoerdEA implements DroneGAFactory {
  @Override
  public FSMController createFSM(ParrotDroneInterface pdi, PolePositionPollInterface pppi, Map<String, String> configurationData) {
    FSMController fsm = new FSMController();
    
    // Define states
    State searchPole = new State("Search pole");
    State leftPass = new State("Pass left");
    State turnRight = new State("Turn right");
    State rightPass = new State("Pass right");
    State turnLeft = new State("Turn left");

    // Add behaviors to states
    Behavior initializeDroneBehavior = new InitializeDrone(pdi, pppi);
    initializeDroneBehavior.setDuration(2500);
    Behavior hoverBehavior = new Hover(pdi, pppi);
    hoverBehavior.setDuration(2000);
    Behavior twitterBehavior = new TwitterBehavior(pdi, pppi);
    Behavior searchPoleBehavior = new SearchPole(pdi, pppi);
    searchPoleBehavior.setDuration(200);
    Behavior passLeftBehavior = new PassLeft(pdi, pppi);
    searchPoleBehavior.setDuration(250);
    Behavior passRightBehavior = new PassRight(pdi, pppi);
    passRightBehavior.setDuration(250);
    Behavior turnLeftBehavior = new TurnLeft(pdi, pppi);
    turnLeftBehavior.setDuration(250);
    Behavior turnRightBehavior = new TurnRight(pdi, pppi);
    turnRightBehavior.setDuration(250);

    searchPole.setEntryBehavior(initializeDroneBehavior);   // Put the drone in the air        
    searchPole.addBehavior(searchPoleBehavior);     // Initial state
    searchPole.setExitBehavior(hoverBehavior);
    leftPass.addBehavior(passLeftBehavior);       // State 1
    leftPass.addBehavior(twitterBehavior);
    leftPass.setExitBehavior(hoverBehavior);        
    turnRight.addBehavior(turnRightBehavior);       // State 2
    turnRight.addBehavior(twitterBehavior);   
    turnRight.setExitBehavior(hoverBehavior);
    rightPass.addBehavior(passRightBehavior);       // State 3
    rightPass.addBehavior(twitterBehavior); 
    rightPass.setExitBehavior(hoverBehavior);
    turnLeft.addBehavior(turnLeftBehavior);       // State 4
    turnLeft.setExitBehavior(hoverBehavior);
    
    // Add transitions between states
    Transition poleFound = new PoleFound(leftPass, pdi, pppi);
    poleFound.setDuration(15);
    searchPole.addTransition(poleFound);
    Transition outOfSightRight = new OutOfSight(turnRight, pdi, pppi);
    outOfSightRight.setDuration(8);
    leftPass.addTransition(outOfSightRight);
    Transition turnCompleteRight = new TurnComplete(rightPass, pdi, pppi);
    turnCompleteRight.setDuration(195);
    turnRight.addTransition(turnCompleteRight);
    Transition outOfSightLeft = new OutOfSight(turnLeft, pdi, pppi);
    outOfSightLeft.setDuration(8);
    rightPass.addTransition(outOfSightLeft);
    Transition turnCompleteLeft = new TurnComplete(leftPass, pdi, pppi);
    turnCompleteLeft.setDuration(195);
    turnLeft.addTransition(turnCompleteLeft);

    fsm.setInitialState(searchPole);

    initializeDroneBehavior.setDuration(Integer.parseInt(configurationData.get("0")));
    hoverBehavior.setDuration(Integer.parseInt(configurationData.get("1"))); 
    poleFound.setDuration(Integer.parseInt(configurationData.get("2")));
    outOfSightRight.setDuration(Integer.parseInt(configurationData.get("3"))); 
    turnCompleteRight.setDuration(Integer.parseInt(configurationData.get("4")));
    outOfSightLeft.setDuration(Integer.parseInt(configurationData.get("5")));
    turnCompleteLeft.setDuration(Integer.parseInt(configurationData.get("6")));

    return fsm;
  }

  @Override
  public IChromosome createGenome(Configuration config) throws InvalidConfigurationException {
    Gene[] sampleGenes = new Gene[7];
    
    sampleGenes[0] = new IntegerGene(config, 1000, 5000);     // Initialize
    sampleGenes[1] = new IntegerGene(config, 0, 5000);      // Hover
    sampleGenes[2] = new IntegerGene(config, 5, 90);      // Find Pole
    sampleGenes[3] = new IntegerGene(config, 0, 50);      // Out of Sight
    sampleGenes[4] = new IntegerGene(config, 100, 300);     // Turn Complete
    sampleGenes[5] = new IntegerGene(config, 0, 50);      // Out of Sight 2
    sampleGenes[6] = new IntegerGene(config, 100, 300);     // Turn Complete 2

    Chromosome result = new Chromosome(config, sampleGenes);
    
    return result;
  }
}
