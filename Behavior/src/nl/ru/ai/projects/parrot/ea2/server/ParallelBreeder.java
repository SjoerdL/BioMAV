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
package nl.ru.ai.projects.parrot.ea2.server;

import java.util.ArrayList;
import java.util.List;

import org.jgap.BulkFitnessFunction;
import org.jgap.Configuration;
import org.jgap.IChromosome;
import org.jgap.Population;
import org.jgap.impl.GABreeder;

public class ParallelBreeder extends GABreeder {
  private static final long serialVersionUID = -967491341656440333L;

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

