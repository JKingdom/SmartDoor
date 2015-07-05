/*************************************************************************
  > File Name: main.c
  > Author: 
  > Mail: 
  > Created Time: 2015年05月16日 星期六 11时36分39秒
 ************************************************************************/

#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include"ImageDataBase.h"
#include"CameraHandler.h"

#include "consts.h"
#include "MyServer.h"
#include "general.h"
#include "BlinkLight.h"
#include "debug.h"

int GLOBAL_DEBUG = 1;

//void * thread_function (void * arg);
//int main_state = 0;
//static bool e = false;

int main() 
{

    if(!GLOBAL_DEBUG)
    {
        if(daemon(0,0) == -1)
            return -1;
    }

    if(GLOBAL_DEBUG)
        printf("MyServer init\n");

    BlinkLight::setup();
    BlinkLight::setAllIn();
    //BlinkLight::In(8);
    //BlinkLight::In(9);
    //BlinkLight::In(7);
   
    MyServer server;

    if(GLOBAL_DEBUG)
        printf("Myserver init end\n");
    //pthread_t a_thread;
    //void * thread_result;

    int res;


    // load model 必须要在上传至少2长图片后才可调用, 这里只是用作测试, 若 image目录不存在可能会崩溃
   // printf("load model");
    //double t;
    //t = (double)cvGetTickCount();
    //ImageDataBaseModel::getInstance().reloadModel();
    //t = (double)cvGetTickCount() - t;
    //printf( "run time = %gms\n", t/(cvGetTickFrequency()*1000) );



    if(GLOBAL_DEBUG)
        printf("start server");
    server.Start();
///////////////////////////////////////
/*
    printf("enter thread");
    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    printf("over create thread");
    //sleep(6);
    //ImageDataBaseModel::getInstance().reloadModel();

    printf("start server");

    server.Start();


    printf("block main thread");
    res = pthread_join(a_thread, &thread_result);
*/

    BlinkLight::setAllIn();
    return 0;
}

/*
void * thread_function(void * arg) 
{
    int res = 0;
    // block in this function
    res = cameraHandler(&main_state, &e);
}
*/
