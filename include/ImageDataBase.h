/*************************************************************************
  > File Name: ImageDataBase.h
  > Author: 
  > Mail: 
  > Created Time: 2015年05月16日 星期六 10时44分39秒
 ************************************************************************/

#ifndef _IMAGEDATABASE_H
#define _IMAGEDATABASE_H

#include<opencv2/opencv.hpp>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>



#define MAX_PATH_LENGTH     512
#define MAX_FILE_EXTENSION  9

class ImageDataBaseModel
{
private:
    cv::Ptr<cv::FaceRecognizer> model;
    pthread_mutex_t work_mutex;
    bool Image_debug;
    bool model_error;
    std::string trainFile;
    unsigned long visit_dirs;
    unsigned long visit_files;
    char   dirname[100];
    char  temp[512][512];
    std::string csv;
    std::string image_dir;
    static ImageDataBaseModel _instance;
    int imagesNumber;
protected:
    ImageDataBaseModel();

public:
    int imageWidth;
    int imageHeight;

public:
    static ImageDataBaseModel & getInstance();
    ~ImageDataBaseModel();

    int getImagesNumber();
    void setImagesNumber(int number);
    void reloadModel();
    int predict(cv::Mat source);

    void generateFile();

private:
    void read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels);      
    void listdir(char *path);
    void generateCSV();
    cv::Mat resizeImg(cv::Mat src);
};
#endif
