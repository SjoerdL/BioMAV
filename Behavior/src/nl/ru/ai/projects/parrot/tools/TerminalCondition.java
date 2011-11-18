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
package nl.ru.ai.projects.parrot.tools;

/**
 * Defines a condition that will stop the FSM when met
 * @author sjoerdlagarde
 */
public abstract class TerminalCondition {

    public TerminalCondition() {

    }

    /**
     * Resets the terminal condition to its original state
     */
    public void reset() {
    	;
    }
    
    public abstract boolean terminate();

}
