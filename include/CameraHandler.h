/*************************************************************************
	> File Name: CameraHandler.h
	> Author: 
	> Mail: 
	> Created Time: 2015年05月16日 星期六 11时39分03秒
 ************************************************************************/

#ifndef _CAMERAHANDLER_H
#define _CAMERAHANDLER_H

#include<opencv2/opencv.hpp>
using namespace cv;

bool detectFace(CascadeClassifier* face_cascade, Mat frame, bool isSecond, int *state);
void analyze(Mat & source, int * state);
int cameraHandler(int *state, bool* e);
void controlGPIO();
#endif
