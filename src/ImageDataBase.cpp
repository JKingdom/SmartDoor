/*************************************************************************
  > File Name: ImageDataBase.cpp
  > Author: 
  > Mail: 
  > Created Time: 2015年05月16日 星期六 17时03分51秒
 ************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "ImageDataBase.h"
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>  

#include "debug.h"

using namespace std;
using namespace cv;

ImageDataBaseModel::ImageDataBaseModel()
{
    //Image_debug = true;
    Image_debug = GLOBAL_DEBUG;
    model_error = false;

    trainFile = "eigenfaces.yml";
    csv = "result.csv";
    image_dir = "information/image";

    visit_dirs = 0;
    visit_files = 0;
    imagesNumber = 0;

    imageWidth = 160;
    imageHeight = 160;

    pthread_mutex_init(&work_mutex, NULL);
    //model = cv::createEigenFaceRecognizer(10,123.0);
    model = cv::createEigenFaceRecognizer(0, 5500);


    if(Image_debug)
        printf("instance init\n");
}

ImageDataBaseModel::~ImageDataBaseModel()
{
    if(Image_debug)
        printf("instance destroy\n");
    pthread_mutex_destroy(&work_mutex);
    model.release();

}

void ImageDataBaseModel::listdir(char *path)
{
    DIR         *ptr_dir;
    struct dirent   *dir_entry;
    int         i = 0;
    char        *child_path;
    char        *file_path;

    child_path = (char*)malloc(sizeof(char)*MAX_PATH_LENGTH);
    if(child_path == NULL){
        if(Image_debug)
            printf("allocate memory for path failed.\n");
        return;
    }
    memset(child_path, 0, sizeof(char)*MAX_PATH_LENGTH);

    file_path = (char*)malloc(sizeof(char)*MAX_PATH_LENGTH);
    if(file_path == NULL){
        if(Image_debug)
            printf("allocate memory for file path failed.\n");
        free(child_path);
        child_path = NULL;
        return;
    }
    memset(file_path, 0, sizeof(char)*MAX_PATH_LENGTH);

    ptr_dir = opendir(path);
    while((dir_entry = readdir(ptr_dir)) != NULL){
        if(dir_entry->d_type & DT_DIR){
            if(strcmp(dir_entry->d_name,".") == 0 || strcmp(dir_entry->d_name,"..") == 0){
                continue;
            }

            sprintf(child_path, "%s/%s", path, dir_entry->d_name);
            strcpy(dirname ,dir_entry->d_name);
            visit_dirs++;
            listdir(child_path);
        }

        if(dir_entry->d_type & DT_REG){
            sprintf(file_path, "%s/%s", path, dir_entry->d_name);
            //printf("%s;%s\n", file_path,dirname);

            strcat(temp[visit_files],file_path);
            strcat(temp[visit_files],";");
            strcat(temp[visit_files],dirname);


            //printf("%s\n",temp[visit_dirs]);
            visit_files++;
        }
    }

    //printf("visited files = %ld\n", visit_files);

    free(child_path);
    child_path = NULL;

    free(file_path);
    file_path = NULL;
}

void ImageDataBaseModel::generateCSV()
{

    if(Image_debug)
        printf("begin generate csv \n");

    char buf[1000];
    getcwd(buf,sizeof(buf));
    strcat(buf,("/"+image_dir).data());
    listdir(buf);

    //char* filename = "result.csv";
    const char * filename = csv.data();

    if((access(filename,F_OK))!=-1)
    {
        if(GLOBAL_DEBUG)
            printf("removed csv!\n");

        remove(filename);
    }

    FILE *pFile = fopen(filename,"w");
    for(int i =0;i<visit_files;i++){
        fputs(temp[i],pFile);
        fputc('\n',pFile);
    }

    // 把已经查找的结果置0, 以免下次再调用时出现数据错误
    for(int i = 0; i < visit_files; i++)
    {
        int j = 0;
        while(temp[i][j] != '\0')
        {
            temp[i][j] = 0;
            j++;
        }
    }

    //记录当前数据库中所有图片的数目 当小于2张时不应该执行predict()函数进行预测
    imagesNumber = visit_files;

    fclose(pFile);

    visit_dirs = 0;
    visit_files = 0;

    if(Image_debug)
        printf("end cvs generate\n");
}

int ImageDataBaseModel::getImagesNumber()
{
    return imagesNumber;
}

void ImageDataBaseModel::setImagesNumber(int number)
{
    imagesNumber = number;
}

void ImageDataBaseModel::reloadModel()
{

    if(Image_debug)
        printf("%s\n", image_dir.data());
    std::cout<< access(image_dir.data(), F_OK) << std::endl;

    if((access(image_dir.data(),F_OK))==-1)
    {
        model_error = true;
        return;
    }
    else 
    {
        model_error = false;
    }

    generateFile();


    if(ImageDataBaseModel::getInstance().getImagesNumber() <= 2)
        return;

    pthread_mutex_lock(&work_mutex);

    if(Image_debug)
        printf("instance model load\n");
    //sleep(5);

    try
    {
        if(!model_error)
        {
            model->load(trainFile);
            model_error = false;
            if(Image_debug)
                printf("instance model end load\n");
        }
    } catch (cv::Exception& e)
    {
        model_error = true;
    //    pthread_mutex_unlock(&work_mutex);
    //    throw cv::Exception(e);
    }

    pthread_mutex_unlock(&work_mutex);
}

Mat ImageDataBaseModel::resizeImg(cv::Mat src)
{
    int cols = 0;
    int rows = 0;
    Rect roi(0,0,0,0);
    //先裁剪
    float rate = (float)imageWidth / (float)imageHeight;

    if(((float)src.cols / (float)src.rows) > rate){
        cols = (int)(src.rows * rate);
        rows = src.rows;

        roi.x =(int) (src.cols - cols)/2;
        roi.y = 0;
        roi.width = cols;
        roi.height = rows;
    } else {
        cols = src.cols;
        rows = (int)(src.cols * (1/rate));

        roi.x = 0;
        roi.y = (int)(src.rows - rows) / 2;
        roi.width = cols;
        roi.height = rows;
    }

    Mat output(src, roi);
    // 后缩放
    Size size(imageWidth,imageHeight);
    resize(output, output, size, 0, 0, INTER_LINEAR);

    return output;

}

static int i = 0;

int ImageDataBaseModel::predict(cv::Mat source)
{
    pthread_mutex_lock(&work_mutex);


    if(Image_debug)
        printf("resizeImg!\n");
    // new fixed 
    // bug: 需要预测的图片必须和数据库的图片大小一致
    // fix: 通过resizeImg函数可以使source帧进行裁剪缩放至数据库图片大小
    Mat result = resizeImg(source);            

    if(Image_debug){
        printf("resizeImg: width = %d, height = %d\n", result.cols, result.rows);

    //imshow("resize", result);
    //waitKey(100);
    }

    if(Image_debug)
        printf("instance model predict\n");

    int predictedLabel = -1;
    try{
        if(!model_error)
        {

            if(Image_debug)
                printf("enter real predict!!!\n");

            predictedLabel= model->predict(result);
        }
    } catch (cv::Exception & e)
    {
        model_error = true;
        std::cout<< e.msg <<std::endl;
    //    pthread_mutex_unlock(&work_mutex);
    //    throw cv::Exception(e);
    }
    pthread_mutex_unlock(&work_mutex);
    return predictedLabel;

}

ImageDataBaseModel & ImageDataBaseModel::getInstance()
{
    return _instance;
}


//使用CSV文件去读图像和标签，主要使用stringstream和getline方法
void ImageDataBaseModel::read_csv(const string& filename, vector<Mat>& images, std::vector<int>& labels) {
    ifstream file(filename.c_str(), ifstream::in);
    // 分隔符
    char separator = ';';

    if (!file) {
        string error_message ="No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty()&&!classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

void ImageDataBaseModel::generateFile()
{
    // generate result.csv file
    generateCSV();


    //读取你的CSV文件路径.
    //    string fn_csv = "x.cvs";

    // 2个容器来存放图像数据和对应的标签
    vector<Mat> images;
    vector<int> labels;
    // 读取数据. 如果文件不合法就会出错
    // 输入的文件名已经有了.
    try {
        //read_csv(fn_csv, images, labels);

        read_csv(csv, images, labels);
    } catch (cv::Exception& e) {
        //cerr <<"Error opening file \""<< fn_csv <<"\". Reason: "<< e.msg << endl;
        cerr <<"Error opening file \""<< csv <<"\". Reason: "<< e.msg << endl;
    }
    // 如果没有读取到足够图片，我们也得退出.
    if(images.size()<=1) {
        //string error_message ="This demo needs at least 2 images to work. Please add more images to your data set!\n";
        //CV_Error(CV_StsError, error_message);
        
        if(Image_debug)
            printf("This demo needs at least 2 images to work. Please add more images to your data set!\n");

        return;
    }

    model->train(images, labels);
    // 保存训练的结果，下一次直接加载使用
    model->save(trainFile);
    if(Image_debug)
        cout << "saved!" << endl;

}

// 生成单例
ImageDataBaseModel ImageDataBaseModel::_instance;
