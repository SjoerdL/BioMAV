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
// classes
Input* input;
Preprocessing* preprocessing;
Processing* processing;
Recognition* recognition;

// define OpenGL variables
#ifdef WITH_OPENGL
GLuint gl_image_tex;
int g_argc;
char **g_argv;
int got_frames(0);
int window(0);
#endif

int width;
int height;
uint8_t* data;

/**
 * Converts activation (float [0-1]) to an image (uint8_t [0-255])
 * 
 * @param activation;       activation that needs to be converted into an image
 */
void convert2Image(float* activation)
{
    int i, j, color;
   
    for (i = 0, j = 0; i < width * height; i++, j+=3)
    {
        color = activation[i] * 255 * 3;
        
        if (color < 255)
        {
            data[j]= 0;
            data[j+1]= 0;
            data[j+2]= color;
        }
        else if (color < 510)
        {
            data[j]= 0;
            data[j+1]= color - 255;
            data[j+2]= 255 - (color - 255);
        }
        else if (color < 765)
        {
            data[j]= color - 510;
            data[j+1]= 255 - (color - 510);
            data[j+2]= 0;
        }
        else
        {
            data[j]= 255;
            data[j+1]= 0;
            data[j+2]= 0;
        }
    }
}

/**
 * Gets data from the input,
 *  processes that data and converts it to an image
 *
 * @return uint8_t RGB (width*height*3)
 */
uint8_t* getData()
{ 
#ifdef SYNC_MUTEX_LOCK_CMD
  SYNC_MUTEX_LOCK_CMD;
#endif
  // get input data, process data, and convert to an image
  uint8_t* newRawData = input->getInput();
  if (newRawData != NULL) {
    preprocessing->process(newRawData);
    float* lineActivations = processing->process(preprocessing->getActivation(), preprocessing->getActivation2());
    float* colorActivations = processing->getBlobData();
    recognition->process(lineActivations, colorActivations);
    
#ifdef WITH_OPENGL
  convert2Image(colorActivations);
#endif
  }

#ifdef SYNC_MUTEX_LOCK_CMD
#ifdef SYNC_MUTEX_UNLOCK_CMD
  SYNC_MUTEX_UNLOCK_CMD;
#endif
#endif

  return data;
}

// ___ TESTING ___ // 

//Called when the window is resized
#ifdef WITH_OPENGL
void handleResize(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(65.0,                  //The camera angle
				   (double)w / (double)h, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   200.0);                //The far z clipping coordinate
}
#endif

#ifdef WITH_OPENGL
void drawHexagon(float t, float centre_x, float centre_y)
{
	//float t = 1;
	float d = (t* sqrt(3))/2;

	//float centre_x = 0;
	//float centre_y = 0;
	float depth = -1.0f;

	float x1 = centre_x;
	float y1 = centre_y-t;

	float x2 = centre_x-d;
	float y2 = centre_y-0.5*t;

	float x3 = centre_x-d;
	float y3 = centre_y+0.5*t;

	float x4 = centre_x;
	float y4 = centre_y+t;

	float x5 = centre_x+d;
	float y5 = centre_y+0.5*t;

	float x6 = centre_x+d;
	float y6 = centre_y-0.5*t;

    /*
	glBegin(GL_TRIANGLES); //Begin triangle coordinates
	// Triangle
	glVertex3f(x3, y3, depth);
	glVertex3f(x4, y4, depth);
	glVertex3f(x5, y5, depth);
	glEnd(); //End hexagonal coordinates

	glBegin(GL_QUADS); //Begin triangle coordinates
	// Triangle
	glVertex3f(x3, y3, depth);
	glVertex3f(x3, y1, depth);
	glVertex3f(x5, y1, depth);
	glVertex3f(x5, y5, depth);
	glEnd(); //End hexagonal coordinates
    */

	
	glBegin(GL_TRIANGLES); //Begin triangle coordinates

	//Hexagon
	glVertex3f(x1, y1, depth);
	glVertex3f(x2, y2, depth);
	glVertex3f(centre_x, centre_y, depth);

	glVertex3f(x2, y2, depth);
	glVertex3f(x3, y3, depth);
	glVertex3f(centre_x, centre_y, depth);

	glVertex3f(x3, y3, depth);
	glVertex3f(x4, y4, depth);
	glVertex3f(centre_x, centre_y, depth);

	glVertex3f(x4, y4, depth);
	glVertex3f(x5, y5, depth);
	glVertex3f(centre_x, centre_y, depth);

	glVertex3f(x5, y5, depth);
	glVertex3f(x6, y6, depth);
	glVertex3f(centre_x, centre_y, depth);

	glVertex3f(x6, y6, depth);
	glVertex3f(x1, y1, depth);
	glVertex3f(centre_x, centre_y, depth);

	glEnd(); //End hexagonal coordinates
	
}
#endif

//Draws the 3D scene
#ifdef WITH_OPENGL
void drawScene() 
{
  uint8_t* rgb;

#ifdef SYNC_MUTEX_LOCK_CMD
  { SYNC_MUTEX_LOCK_CMD;
#endif
  rgb = getData();
    
  //Clear information from last draw
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
  glLoadIdentity(); //Reset the drawing perspective
  
  float t = 1.0/(height*1.5);
  float d = (t* sqrt(3))/2;

  int i, j, counter = 0;
  float r, g, b;
  for (i=0.5*height; i>-0.5*height; i--)
	  for (j=-0.5*width; j<0.5*width; j++)
	  {
		  r = rgb[counter]; r = r/255;
		  g = rgb[counter+1]; g = g/255;
		  b = rgb[counter+2]; b = b/255;
      
		  glColor3f (r, g, b);
		  if ((i % 2) != 0)
			  drawHexagon(t, d+j*(2*d), (1.5*i)*t);
		  else
			  drawHexagon(t, j*(2*d), (1.5*i)*t);
      
      counter+=3;
	  }

  glColor3f(1, 1, 1);
  float* verticalActivationData = recognition->getVerticalScores();
  glBegin(GL_LINE_STRIP);
   for (int x = 0; x < width; x++) {
//     cout << verticalActivationData[x] << endl;
     glVertex3f(-0.5 + (x * 1.0) / width, verticalActivationData[x]*0.25-0.5, -1);
   }
  glEnd();

  glColor3f(1, 0, 0);
  glBegin(GL_LINE_STRIP);
    glVertex3f(-0.5, POLE_DETECTION_THRESHOLD*0.25-0.5, -1);
    glVertex3f( 0.5, POLE_DETECTION_THRESHOLD*0.25-0.5, -1);
  glEnd();

  glColor3f(0, 1, 0);
  float* debugValues = recognition->getDetectionProbabilities();
  glBegin(GL_LINE_STRIP);
   for (int x = 0; x < width; x++) {
     glVertex3f(-0.5 + (x * 1.0) / width, debugValues[x]-0.5, -1);
   }
  glEnd();

#ifdef SYNC_MUTEX_LOCK_CMD
#ifdef SYNC_MUTEX_UNLOCK_CMD
  SYNC_MUTEX_UNLOCK_CMD; }
#endif
#endif

  glutSwapBuffers(); //Send the 3D scene to the screen
}
#endif

/**
 * glutKeyboardFunc Handler
 *
 * @param key;      key pressed
 * @param x;
 * @param y;
 */
#ifdef WITH_OPENGL
void keyPressed(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        glutDestroyWindow(window);
    }
}
#endif

/**
 * OpenGL functions
 */
#ifdef WITH_OPENGL
void DrawGLScene()
{
    uint8_t* rgb;

    rgb = getData();
    got_frames = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, gl_image_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // IF width * 3 BYTES IS NOT DIVISIBLE BY 4!
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &rgb[0]);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
    glTexCoord2f(0, 0); glVertex3f(0,0,0);
    glTexCoord2f(1, 0); glVertex3f(width,0,0);
    glTexCoord2f(1, 1); glVertex3f(width,height,0);
    glTexCoord2f(0, 1); glVertex3f(0,height,0);
    glEnd();
    glutSwapBuffers();
}
#endif

/**
 * initialise OpenGL window
 */
#ifdef WITH_OPENGL
void InitGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);
    glGenTextures(1, &gl_image_tex);
    glBindTexture(GL_TEXTURE_2D, gl_image_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, width, height, 0, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
}
#endif

/**
 * intialise the data in an OpenGL window
 */
#ifdef WITH_OPENGL
void displayData()
{
    glutInit(&g_argc, g_argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    window = glutCreateWindow("Vision - Test Platform");
    
    //glutDisplayFunc(&DrawGLScene);
    //glutIdleFunc(&DrawGLScene);
    //glutKeyboardFunc(&keyPressed);

    glutDisplayFunc(&drawScene);
    glutIdleFunc(&drawScene);
    glutKeyboardFunc(&keyPressed);
    glutReshapeFunc(&handleResize);
    
    InitGL();
    glutMainLoop();
}
#endif

void init(Input* usedInput, int _width, int _height, int hexel, int hexelDiameter, int colorspace, int convert, int l, int m, int s, int normalize, int lateral) {
  input = usedInput;
  width = _width;
  height = _height;
  
  preprocessing = new Preprocessing(width, height, hexel, hexelDiameter, colorspace, convert, l, m, s, normalize, lateral);
  width = preprocessing->getWidth();
  height = preprocessing->getHeight();
  
  processing = new Processing(width, height, preprocessing->estSexangvlatvm());
  
  recognition = new Recognition(width, height, preprocessing->estSexangvlatvm());
  
  data = new uint8_t [width*height*3];
  
#ifdef WITH_OPENGL
  displayData();
#endif
}
