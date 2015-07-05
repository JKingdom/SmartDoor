#ifndef SRC_CONSTS_H_

//Globe
#define SRC_CONSTS_H_
#define MAX_USER_NUM 10
#define MAX_PIC_NUM_FOR_EACH_USER 10
#define VALID 0
#define INVALID 1
#define INVALID_MD5 2
#define USER_NAME_REPEAT 2
#define ROOT_DIR "./information/"
#define IMAGE_DIR "./information/image/"
//#define NOFILE 5

//server socket
#define USER_IP "192.168.1.121"
#define TMP_DIR  "./tmp"
#define TMP_PATH  "./tmp/tmp.png"
#define BUFFER_SIZE 1024
#define INT_SIZE (int)4
#define MD5CODE_SIZE 16
#define PORT 2015
#define LENGTH_OF_LISTEN_QUEUE 10

//CAM
#define CAM_ON 1
#define CAM_OFF 0

//server opCode
#define CONFIRM 0
#define ADD_USER 1
#define REMOVE_USER 2
#define MOD_USER 3
#define ADD_PIC 4
#define REMOVE_PIC 5
#define INIT_CLIENT 7
#define CHECK_KEY 8
#define MOD_KEY 9
#define CAM_STATE 10
#define CAM_CON 11

//MD5
#define INIT_MD5 "0000000000000000"


#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
//#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <iterator>
using namespace std;
#endif
