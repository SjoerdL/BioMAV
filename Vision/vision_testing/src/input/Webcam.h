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
/* 
 * File:   Webcam.h
 * Author: jelangevoort
 *
 * Created on August 14, 2011, 11:15 AM
 * 
 * Using the OpenCV high-gui libraries opens a webcam as capture device
 *  and returns the data ( uint8_t rgb width*height*3 )
 */

#ifndef WEBCAM_H_
#define WEBCAM_H_

#include <stdint.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

class Webcam
{
public:
        Webcam(int captureDevice, int width, int height);
        ~Webcam();

        uint8_t* getData();
        int getWidth(){return capture->get(CV_CAP_PROP_FRAME_WIDTH);};
        int getHeigth(){return capture->get(CV_CAP_PROP_FRAME_HEIGHT);};
private:
        cv::Mat frame;
        cv::VideoCapture* capture;
};

#endif /* WEBCAM_H_ */
