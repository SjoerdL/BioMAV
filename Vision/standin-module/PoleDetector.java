package biomav;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

public class PoleDetector implements PolePositionPollInterface
{
	//What color to detect
	public final int R = 255;
	public final int G = 144;
	public final int B = 0;
	
	int[] image;
	byte[] imageBytes;
	boolean usingByteImage;
	
	int width = VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH;
	int height= VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT;
	
	int[] orangePixelCountColumns;
	
	// the colour-ratios for orange. (estimated)
	double RG;// = 2;
	double RB;// = 3.9;
	double GB;// = 1.95;
	
	double allowedDelta = 0.2;		// amount a colour-ratio may deviate from our pre-set colour-ratios
									// for colours in order for the colour to still be classified as orange
	
	double cameraAngle = 93; // oh ja joh?        viewing angle of the camera. paul says it's a bit smaller than 93 probably
	double poleWidth = 0.15; // in meter, en.. oh ja joh?       // width of the pool 
	int minimalOrangePixelsForPoleColumn = 15; 	// number of orange pixels there have to be in one column
												// in order to classify the column as part of the pole
												// should be dependant on image height really
	
	double[] angleDistancePole = new double[2];
			
	public static void main(String args[])
	{
		PoleDetector pd = new PoleDetector();
		
		int[] image = pd.readImage("C:/Users/Laurie/Desktop/simulatorPole.PNG");
		pd.setImage(image);

		System.out.println("pole angle in radions from function: " + pd.getPositionPole());
		
	}
	public PoleDetector()
	{
		orangePixelCountColumns = new int[width];
		calculateRatios(R, G, B);
		//detectOrangePixels(image);
		
		//System.out.println("pole angle in radions from function: " + getPositionPole());
	}
	
	public void calculateRatios(double r, double g, double b)
	{
		if (g == 0)
			g = 0.01;
		if(b == 0)
			b = 0.01;
		
		RG = (double) r / (double) g;
		RB = (double) r / (double) b;
		GB = (double) g / (double) b;
	}
	
	/**
	 * Returns the angle and distance of a pole.
	 * @param image The image in which the pole has to be detected.
	 * @return returns a double-array of length 2. At index 0 the angle of the pole is stored in radians
	 *         and at index 2 the distanceof the pole is stored in m.
	 */
	public double[] getAngleDistancePole(byte[] image)
	{
		angleDistancePole = new double[2];
		
		setImage(image);
		detectOrangePixels(image);
		getPositionPole();
		
		return angleDistancePole;
	}
	
	public double getPositionPole()
	{
		double nColumns = 0;
		int[] columnNumbers = new int[orangePixelCountColumns.length];
		double meanColumn;
		
		boolean leftOfMiddle;
		double percentagePoleInView = 0;		
		double viewWidthAtPole = 0;
		double viewDistanceAtPole = 0;
		double poleDistance = 0;
		double poleAngle = 0;
		double pixelsFromCenter = 0;
		double distanceFromCenter = 0;
		
		for(int i=0;i<orangePixelCountColumns.length; i++)
		{
			if(orangePixelCountColumns[i] > minimalOrangePixelsForPoleColumn)
			{
				columnNumbers[(int) nColumns] = i;
				nColumns++;
			}
		}
		
		meanColumn = 0;
		for(int i=0;i<nColumns;i++)
		{
			meanColumn += columnNumbers[i];
		}
		meanColumn /= nColumns;
		
		leftOfMiddle = meanColumn <= (double) (width/2);
		
		if(Math.abs(meanColumn - (double) width/2) < nColumns/2 ) // pole is in the middle of the picture
		{
			//System.out.println("pole in middle");
			percentagePoleInView = nColumns / (double) (width);
			//System.out.println("perc pole in view: " + percentagePoleInView);
			viewWidthAtPole = poleWidth / percentagePoleInView;	
			//System.out.println("view width at pole: " + viewWidthAtPole);
			
			viewDistanceAtPole = (viewWidthAtPole/2.0) / Math.tan(Math.toRadians(cameraAngle/2.0));
			
			pixelsFromCenter = Math.abs(((double) width/2) - meanColumn);
			distanceFromCenter = pixelsFromCenter * (viewWidthAtPole / ((double) width));
			
			poleDistance = Math.sqrt(Math.pow(viewDistanceAtPole, 2) + Math.pow(distanceFromCenter, 2));
			poleAngle = Math.atan(distanceFromCenter/viewDistanceAtPole);
		}
		else // pole is somewhere along the sides
		{
			//System.out.println("pole along sides");
			percentagePoleInView = nColumns / (double) (width / 2);
			viewWidthAtPole = poleWidth / percentagePoleInView;	
			
			viewDistanceAtPole = viewWidthAtPole / Math.tan(Math.toRadians(cameraAngle / 2.0));
			
			pixelsFromCenter = Math.abs(((double) width/2) - meanColumn);
			distanceFromCenter = pixelsFromCenter * (viewWidthAtPole / ((double) width / 2.0));
			
			poleDistance = Math.sqrt(Math.pow(viewDistanceAtPole, 2) + Math.pow(distanceFromCenter, 2));
			poleAngle = Math.atan(distanceFromCenter/viewDistanceAtPole);
		}
		
		//System.out.println("view distance at pole: " + viewDistanceAtPole);
		//System.out.println("pole distance: " + poleDistance);
		//System.out.println("pole angle in degrees: " + Math.toDegrees(poleAngle));
		
		angleDistancePole[0] = poleAngle;
		angleDistancePole[1] = poleAngle;
		
		if(leftOfMiddle)
		{
			return -poleAngle;
		}
		else
		{
			return poleAngle;
		}
		//return poleAngle;
	}
		
	public void detectOrangePixels(byte[] image)
	{
		orangePixelCountColumns = new int[width];
		
		for(int i=0; i<width; i++)
		{
			for(int j=0; j<height; j++)
			{
				if(isOrange(i, j, image))
				{
					orangePixelCountColumns[i]++;
				}
			}
		}
	}
	
	public void detectOrangePixels(int[] image)
	{
		orangePixelCountColumns = new int[width];
		
		for(int i=0; i<width; i++)
		{
			for(int j=0; j<height; j++)
			{
				if(isOrange(i, j, image))
				{
					orangePixelCountColumns[i]++;
				}
			}
		}
	}
	
	public boolean isOrange(int i, int j, byte[]image)
	{
		double r, g, b;
		
		//System.out.println(image[(j*width + i) * 3]);
		
		
			r = image[(j*width + i) * 3];
			g = image[(j*width + i) * 3 + 1];
			b = image[(j*width + i) * 3 + 2];
		
			if(r < 0)
			{
				r += 256;
			}
			if(g < 0)
			{
				g += 256;
			}
			if(b < 0)
			{
				b += 256;
			}
			//System.out.println("rgb:" + r + " " + g + " " + b);
			
		if(g == 0)
		{
			g = 0.01;
		}
		if(b == 0)
		{
			b = 0.01;
		}
		
		double rg, rb, gb;
		rg = r/g;
		rb = r/b;
		gb = g/b;
		
		//System.out.println(rg + " " + rb + " " + gb);
		
		if((Math.abs(rg - RG) < allowedDelta) && (Math.abs(rb - RB) < allowedDelta) && (Math.abs(gb - GB) < allowedDelta))
		{
			return true;
		}
		
		return false;
	}
	
	public boolean isOrange(int i, int j, int[]image)
	{
		double r, g, b;
		
		int pixel = image[j*width + i];	
		
			r = ((pixel & 0x00ff0000) >> 16);// red
			g = ((pixel & 0x0000ff00) >> 8); // green
			b = pixel & 0x000000ff;		// blue
		

		if(g == 0)
		{
			g = 0.01;
		}
		if(b == 0)
		{
			b = 0.01;
		}
		
		double rg, rb, gb;
		rg = r/g;
		rb = r/b;
		gb = g/b;
		
		if((Math.abs(rg - RG) < allowedDelta) && (Math.abs(rb - RB) < allowedDelta) && (Math.abs(gb - GB) < allowedDelta))
		{
			return true;
		}
		
		return false;
	}
	
	public int[] readImage (String filename)
    {
        try 
        {
            File file = new File(filename);
            BufferedImage tempImage = ImageIO.read(file);
            width = tempImage.getWidth();
            height = tempImage.getHeight();
            image = tempImage.getRGB(0, 0, width, height, image, 0, width);
            usingByteImage = false;
            
            return image;
        } 
        catch (IOException e) {}
		return null;
    }
	
	
	public void setImage(int[] _image)
	{
		image = _image;
		usingByteImage = false;
				
		detectOrangePixels(image);
		// uses default width and height values: 320*240
	}
	
	public void setImage(byte[] _image)
	{
		imageBytes = _image;
		usingByteImage = true;
		
		detectOrangePixels(imageBytes);
		// uses default width and height values: 320*240
	}
	
	@Override
	public long getPolePositionTimeStamp() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Double[] getPolePositions() {
		
		Double [] polePositions = new Double[1];
		if( Double.isNaN(getPositionPole()))
			return new Double[0];
		
		polePositions[0] = getPositionPole();
		
		return polePositions;
	}
}
