#ifndef CAMERA_STATE_H
#define CAMERA_STATE_H
// UNKNOW_ERROR 与其他都无关，不可组合
// CAMERA_OPEN 和 CAMERA_CLOSE 相互对立，不可组合
// 其他可与 open 与 close 组合
#define UNKNOW_ERROR 0
#define CAMERA_OPEN 4
#define CAMERA_CLOSE 1
#define OPENCV_FILE_NOT_EXIST 2
#define INTERNET_NOT_WORK 8
#define ALL_WORK 15
// 摄像头能打开，但是opencv文件不存在(opencv离线检测模块) CAMERA_OPEN | OPENCV_FILE_NOT_EXIST = 6
// 摄像头能打开，但是不能连上因特网(face++网络监测模块) CAMERA_OPEN | INTERNET_NOT_WORK = 12
// 摄像头能打开，但是文件不存在也不能连上因特网 CAMERA_OPEN | OPENCV_FILE_NOT_EXIST | INTERNET_NOT_WORK = 14
// 摄像头打不开，且文件不存在 CAMERA_CLOSE | OPENCV_FILE_NOT_EXIST = 3
// 摄像头打不开，且没有网络连接 CAMERA_CLOSE | INTERNET_NOT_WORK = 9
// 摄像头打不开，且没有网络连接，也没有文件 (最坏状态) CAMERA_CLOSE | OPENCV_FILE_NOT_EXIST | INTERNET_NOT_WORK = 7

// 以下是当前摄像头部分的执行阶段状态
#define COLLECT_INTENT 1 << 4
#define COMPARE 2 << 4
// 摄像头能打开，但是opencv文件不存在(opencv离线检测模块), 并且处于采集意图模块阶段 CAMERA_OPEN | OPENCV_FILE_NOT_EXIST | COLLECT_INTENT 
// 摄像头能打开，但是不能连上因特网(face++网络监测模块) 并且处于采集意图模块阶段, CAMERA_OPEN | INTERNET_NOT_WORK | COLLECT_INTENT 
// 摄像头能打开，但是文件不存在也不能连上因特网 并且处于采集意图模块阶段, CAMERA_OPEN | OPENCV_FILE_NOT_EXIST | INTERNET_NOT_WORK | COLLECT_INTENT 
// 摄像头能打开，但是opencv文件不存在(opencv离线检测模块), 并且处于比较模块阶段 CAMERA_OPEN | OPENCV_FILE_NOT_EXIST | COMPARE 
// 摄像头能打开，但是不能连上因特网(face++网络监测模块) 并且处于比较模块阶段, CAMERA_OPEN | INTERNET_NOT_WORK | COMPARE 
// 摄像头能打开，但是文件不存在也不能连上因特网 并且处于比较模块阶段, CAMERA_OPEN | OPENCV_FILE_NOT_EXIST | INTERNET_NOT_WORK | COMPARE 

#endif