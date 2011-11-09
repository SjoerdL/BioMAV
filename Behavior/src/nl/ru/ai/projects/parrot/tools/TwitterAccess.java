package nl.ru.ai.projects.parrot.tools;

import java.awt.Point;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.RenderedImage;
import java.awt.image.WritableRaster;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import javax.imageio.ImageIO;

import nl.ru.ai.projects.parrot.dronecontrol.VideoPollInterface;

import org.apache.commons.codec.binary.Base64;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import winterwell.jtwitter.OAuthSignpostClient;
import winterwell.jtwitter.Twitter;
import winterwell.jtwitter.TwitterException;

public class TwitterAccess {

	private static TwitterAccess instance = null;
	
	private OAuthSignpostClient oauthClient;
	private Twitter twitter;
	
	private boolean enabled;
	
	private TwitterAccess() {
		oauthClient = new OAuthSignpostClient("", "",
				"", ""); // insert login details
				 
		twitter = new Twitter("BioMAV", oauthClient);
	}
	
	public static TwitterAccess getInstance() {
		if ( instance == null )
			instance = new TwitterAccess();
		
		return instance;
	}
	
	/**
	 * Sets whether or not tweets should be posted
	 * @param enabled True when tweets should be posted to twitter, false otherwise
	 */
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}
	
	/**
	 * Returns whether or not tweets are posted
	 * @return True when tweets are posted, false otherwise
	 */
	public boolean isEnabled() {
		return enabled;
	}
	
	/**
	 * Sets the status on Twiter with a predefined chance of 75%
	 * @param status The status to set
	 */
	public void setStatus(String status) {
		setStatus(status, 75);
	}
	
	/**
	 * Sets the status on Twitter with a given chance
	 * @param status The status to set
	 * @param chance The chance that the status will be set
	 */
	public void setStatus(String status, int chance) {
		if ( enabled ) {
			int randomValue = (int) (Math.random()*100);
			if ( randomValue <= chance ) {
				try {
					twitter.setStatus(status);
				} catch (TwitterException e) {
					; 		// Probably a duplicate status... Too bad ;(
				}
			}
		}
	}
	
	/**
	 * Uploads an image
	 * @param image The image to upload
	 * @param tweet The text of the tweet that needs to be posted with the image
	 */
	public void uploadImage(byte[] image, String tweet) {
		if ( !enabled )
			return;
		
		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		RenderedImage img = getImageFromCamera(image);
		try {
			ImageIO.write(img, "jpg", baos);
			URL url = new URL("http://api.imgur.com/2/upload.json");
			String data = URLEncoder.encode("image", "UTF-8") + "=" + URLEncoder.encode(Base64.encodeBase64String(baos.toByteArray()), "UTF-8");
			data += "&" + URLEncoder.encode("key", "UTF-8") + "=" + URLEncoder.encode("f9c4861fc0aec595e4a64dd185751d28", "UTF-8");
			
			URLConnection conn = url.openConnection();
			conn.setDoOutput(true);
			OutputStreamWriter wr = new OutputStreamWriter(conn.getOutputStream());
			wr.write(data);
			wr.flush();
			
			StringBuffer buffer = new StringBuffer();
			InputStreamReader isr = new InputStreamReader(conn.getInputStream());
			Reader in = new BufferedReader(isr);
			int ch;
			while ( (ch = in.read()) > -1 ) 
				buffer.append((char)ch);
			
			String imgURL = processJSON(buffer.toString());
			setStatus(tweet+" "+imgURL, 100);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} 
	}
	
	/**
	 * Returns a RenderedImage object from a byte array
	 * @param cameraOutput The camera output to transform into a RenderedImage
	 * @return The RenderedImage that resulted from the camera output
	 */
	private RenderedImage getImageFromCamera(byte[] cameraOutput) {
		BufferedImage image = new BufferedImage(VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, BufferedImage.TYPE_3BYTE_BGR);
		if (cameraOutput != null) {
			WritableRaster raster = Raster.createBandedRaster(DataBuffer.TYPE_BYTE, 
					VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, 
					VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, 
					3, 
					new Point(0, 0));
			raster.setDataElements(0, 0, VideoPollInterface.FRONT_VIDEO_FRAME_WIDTH, VideoPollInterface.FRONT_VIDEO_FRAME_HEIGHT, cameraOutput);
			image.setData(raster);
		}

		return image;
	}
	
	/**
	 * Processes the JSON response from uploading the image
	 * @param input The JSON object that needs to be processed
	 */
	private String processJSON(String input) {
		String returnString = "";
		JsonObject json = new JsonParser().parse(input).getAsJsonObject().getAsJsonObject("upload").getAsJsonObject("links");
		if ( json!=null )
			returnString = json.getAsJsonPrimitive("imgur_page").getAsString();
		
		return returnString;
	}

}
