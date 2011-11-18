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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

/**
 * Writes text to an output file
 * @author sjoerdlagarde
 */
public class Writer {

    private static Writer instance;
    
    private String filename = "output";
    private BufferedWriter out = null;
    private String newlineChar;
    
    private Writer() {
        newlineChar = System.getProperty("line.separator"); 
    }
    
    public static Writer getInstance() {
        if ( instance == null )
            instance = new Writer();
        
        return instance;
    }
    
    /**
     * Sets the filename that needs to be used to write output to
     * @param filename The filename to use
     */
    public void setFilename(String filename) {
    	this.filename = filename;
    }
    
	private void setupFile() {
		File file;
		int counter = 0;
		do { 
			String tmpFilename = filename+"_"+counter+".txt";
			file = new File(tmpFilename);
			counter++;
		} while ( file.exists() );
		
		try {
			out = new BufferedWriter(new FileWriter(file));
		} catch (IOException ex) {
			System.err.println("Failed to open output-file");
		}
	}
    
    /**
     * Writes text to the output file. Text will be ended with a newline
     * @param text The text to write to the output file
     */
    public void write(String text) {
        write(text, true);
    }
    
    /**
     * Writes text to the output file
     * @param text The text to write to the output file
     * @param newline True when the text should be ended with a newline, false otherwise
     */
    public void write(String text, boolean newline) {
    	if ( out == null )
    		setupFile();
    	
        if ( out != null ) {
            try {           
               out.write(text);
               if ( newline )
                   out.write(newlineChar);
            } catch (IOException ex) {
                System.err.println("Could not write output to file");
            }
        }
    }
    
    /**
     * Flushes the stream
     */
    public void flush() {
    	if ( out == null )
    		setupFile();
    	
    	try {
			out.flush();
		} catch (IOException e) {
			System.err.println("Could not flush stream");
		}
    }
    
    /**
     * Closes the output file
     */
    public void close() {
        try {
        	flush();
            out.close();
        } catch (IOException ex) {
            System.err.println("Could not close output file");
        }
    }
}
