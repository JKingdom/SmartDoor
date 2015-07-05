#include<opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>
#include "ImageDataBase.h"
#include "CameraState.h"
#include "CameraHandler.h"
#include "BlinkLight.h"
#include "debug.h"

using namespace std;
using namespace cv;

/** Function Headers */
//bool detectFace(CascadeClassifier* face_cascade, Mat frame, bool isSecond, int *state);//检测人脸截图并展示出来 

//bool detectFace(CascadeClassifier* face_cascade, Mat frame, bool isSecond, int *state);
//void analyze(Mat & source, int * state);
//int cameraHandler(int *state, bool* exit);

/** Global variables */
static string window_name = "Capture - Face detection";
static string window_name2 = "Captured Face";

//bool out_exit = false;
//int out_state = 0;
bool DEBUG = false;

void printState(int* state)
{
    printf("state: %d\n", *state);
};

int cameraHandler(int *state, bool* exit)
{

    if(GLOBAL_DEBUG)
        printf("enter cameraHandler!!!\n");


    CascadeClassifier face_cascade;
    String face_cascade_name = "haarcascade_frontalface_alt2.xml";
    Mat frame;
    Mat foreground;
    bool isSecond = false;
    //int* state = &out_state;
    //bool* exit = &out_exit;
    *exit = false;

    double t;

    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) )
    {
        if(GLOBAL_DEBUG)
            printf("--(!)Error loading\n"); 
        return -1;
    };


    // 开启摄像头
    VideoCapture cap(0); 
    if(!cap.isOpened())  
    {  
        return -1;  
    }

    while( true )
    {

        if(*exit) 
        {
            sleep(2);
            continue;
        }
        cap >> frame;

        if(DEBUG)
            t = (double)cvGetTickCount();

        if( frame.empty() )
        {

            *state = CAMERA_CLOSE;
            if(DEBUG)
                printState(state);
            if(GLOBAL_DEBUG)
                printf(" --(!) No captured frame -- Break!\n");
            sleep(3);
            continue; 
        }
        else
        { 
            *state = CAMERA_OPEN;
            if(DEBUG)
                printState(state);

            // when in real use, it will be removed
            if(0)
            {
                imshow( window_name, frame );
            }
            isSecond = false;
            if(detectFace(&face_cascade, frame, isSecond, state))
            {

                BlinkLight::Out(BlinkLight::white);

                // 2 seconds intent!
                waitKey(2000);

                *state = CAMERA_OPEN | COLLECT_INTENT;

                //frame = cvQueryFrame( capture );
                cap >> frame;
                isSecond = true;
                detectFace(&face_cascade, frame, isSecond, state);

                BlinkLight::In(BlinkLight::white);
            }
        }
        if(DEBUG)
        {
            t = (double)cvGetTickCount() - t;
            printf( "run time = %gms\n", t/(cvGetTickFrequency()*1000) );
            t = 0;
        }
        waitKey(10);

        // 收到退出码,退出循环
    }


    cap.release();
    if(GLOBAL_DEBUG)
        printf("exit the cam thread\n");

    return 0;
}

/** @function detectFace */
bool detectFace(CascadeClassifier* face_cascade, Mat frame, bool isSecond, int *state)
{
    std::vector<Rect> faces;
    Mat frame_gray;
    Mat detected_face;
    Mat faceROI;

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    //face_cascade.detectMultiScale( frame_gray, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );  
    //CV_HAAR_DO_ROUGH_SEARCH
    //CV_HAAR_DO_CANNY_PRUNING
    face_cascade->detectMultiScale( frame_gray, faces, 1.1, 3, 0|CV_HAAR_FIND_BIGGEST_OBJECT, Size(100, 50) );  
    //face_cascade.detectMultiScale( frame_gray, faces, 1.1, 3, 0|CV_HAAR_DO_ROUGH_SEARCH, Size(30, 30) );  


    for( size_t i = 0; i < faces.size(); i++ )
    {
        if(isSecond)
        {
            int y = faces[i].y - 20;
            if( y < 0 )
                y = 0;
            faces[i].y = y;
            faces[i].height += 40;
            if(faces[i].height + y > frame_gray.rows)
                faces[i].height = frame_gray.rows - y;

            Mat faceROI = frame_gray( faces[i] );


            if(GLOBAL_DEBUG)
                printf("get faceROI\n");

            // my task , start face recognize.
            if(ImageDataBaseModel::getInstance().getImagesNumber() >= 2)
                analyze(faceROI, state);

            // when in real use, it will be removed
            if(0)
            {
                namedWindow(window_name2);
                imshow( window_name2, faceROI );
            }

            if(*state == CAMERA_OPEN | COMPARE)
                *state = ALL_WORK;
        }
        return true;
    }
    return false;
}


std::string imageDateBaseFile = "eigenfaces.yml";

void analyze(Mat & source, int * state)
{


    printf("enter analyze\n");

    // stub
    *state = CAMERA_OPEN | COMPARE; 

    //Mat testSample = imread("/home/king/Desktop/face_recognition/att_faces/s3/5.pgm", 0);
    int testLabel = 2; 
    // test image:
    // 在正式代码中要把testSample 改成 source
    int predictedLabel = -1;
    try{
        predictedLabel = ImageDataBaseModel::getInstance().predict(source);
    }catch (cv::Exception& e)
    {
        if(GLOBAL_DEBUG)
            cout << e.msg <<endl;
        *state = CAMERA_OPEN | OPENCV_FILE_NOT_EXIST | COMPARE;
    }

    if(DEBUG)
    {
        string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, testLabel);
        cout << result_message << endl;
    }

    if(predictedLabel == -1)
    {
        //没有找到适合的对象,匹配不成功
        if(GLOBAL_DEBUG)
            printf("none\n");
    }
    else 
    {
        //匹配成功
        controlGPIO();
        if(GLOBAL_DEBUG)
            printf("match successful\n");
    }
    return;
}

void controlGPIO()
{
    // light
    BlinkLight::blink(3,BlinkLight::blue);
}
