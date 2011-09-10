package biomav;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.Random;

import javax.swing.JFrame;

public class Vision implements PolePositionPollInterface 
{
	VideoPollInterface videoPollInterface;
	PoleDetector poleDetector;
	
	JFrame displayFrame;
	
	BufferedImage bi;
	int[] im;
	
	int imWidth;
	int imHeight;
	
	public Vision(VideoPollInterface videoPollInterface)
	{
		this.videoPollInterface = videoPollInterface;
		this.poleDetector = new PoleDetector();
		
		initFrame();
		bi = new BufferedImage(imWidth, imHeight, BufferedImage.TYPE_INT_RGB);
		im = new int[imWidth*imHeight];
	}

	private void initFrame()
	{
		displayFrame = new JFrame("Vision module output");
		
		imWidth = videoPollInterface.FRONT_VIDEO_FRAME_WIDTH;
		imHeight = videoPollInterface.FRONT_VIDEO_FRAME_HEIGHT;
		
		displayFrame.setSize(imWidth, imHeight);
		displayFrame.setVisible(true);
	}
	
	private void paintImageToFrame(byte[] image)
	{		
		int index = 0;
		for(int i=0; i<im.length; i++, index+=3)
		{
			im[i] = image[index] << 16 | image[index+1] << 8 | image[index+2];
		}
		
		bi.setRGB(0, 0, imWidth, imHeight, im, 0, imWidth);
		
		Graphics g = displayFrame.getGraphics();
		g.drawImage(bi, 0, 0, null);		
	}
	
	@Override
	public long getPolePositionTimeStamp() 
	{
		Random rand = new Random();
		
		return rand.nextLong();
	}

	@Override
	public Double[] getPolePositions() 
	{
		byte[] image = videoPollInterface.getFrontCameraImage();
		
		paintImageToFrame(image);
		
		poleDetector.setImage(image);
		
		return poleDetector.getPolePositions();
	}
}
