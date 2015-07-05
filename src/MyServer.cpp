/*
 * MyServer.cpp
 *
 *  Created on: 2015年5月1日
 *      Author: xqf
 */

#include "MyServer.h"
#include"CameraHandler.h"
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include <time.h>
#include <sstream> 
#include"ImageDataBase.h" 
#include "BlinkLight.h"
#include "debug.h"

bool MyServer::exitFlag = false;
int MyServer::m_state = 0;

MyServer::MyServer()  
{
	m_switchInfo = CAM_OFF;
    exitFlag = false;
    m_state = 0;
    isCamThreadStarted = false;

    m_MD5 = INIT_MD5;
    //设置一个socket地址结构server_addr,代表服务器Internet地址, 端口
    if (access(TMP_DIR, F_OK) != 0) {
        mkdir(TMP_DIR, 0755);
    }
    bzero(&m_server_addr, sizeof(m_server_addr)); //把一段内存区的内容全部设置为0
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    m_server_addr.sin_port = htons(PORT);
    m_socket = 0;
    m_keepRunning = INVALID;
    //创建用于Internet的流协议(TCP)socket,用server_socket代表服务器socket
    m_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (m_socket < 0) {
        if(GLOBAL_DEBUG)
            printf("Create Socket Failed!");
        return;
    }

    //把socket和socket地址结构联系起来
    if (bind(m_socket, (struct sockaddr*) &m_server_addr, sizeof(m_server_addr))) 
    {
        if(GLOBAL_DEBUG)
            printf("Server Bind Port : %d Failed!", PORT);
        return;
    }
    return;
}

MyServer::~MyServer() {
    //关闭监听用的socket
    close(m_socket);
}

int MyServer::Usecase() 
{
    if(GLOBAL_DEBUG)
        cout << "m_state = " << m_state << endl;
    
    return VALID;
}

int MyServer::Start() {
    //start listen for connection
    pthread_t id;
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&id, &attr, MyServer::ListenForCon, NULL);
    if (ret) {
        if(GLOBAL_DEBUG)
            cout << "Create pthread listenforcon error!" << endl;

        return 1;
    }



    //m_socket开始监听
    if (listen(m_socket, LENGTH_OF_LISTEN_QUEUE)) 
    {
        if(GLOBAL_DEBUG)
            printf("Server Listen Failed!");
        return INVALID;
    }
    m_keepRunning = VALID;
    
    BlinkLight::blink(5,BlinkLight::red);


    while (m_keepRunning == VALID) //服务器端要一直运行
    {
        int reCode = 0;

        if(GLOBAL_DEBUG)
            cout << endl << "connecting" << endl;

        reCode = INVALID;
        //定义客户端的socket地址结构client_addr
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        int new_server_socket = accept(m_socket,
                (struct sockaddr*) &client_addr, &length);
        if (new_server_socket < 0)
        {
            if(GLOBAL_DEBUG)
                printf("Server Accept Failed!\n");
            break;
        }

        if(GLOBAL_DEBUG)
            cout << "connected" << endl;

        string MD5;
        MD5 = RecvMD5(new_server_socket);
        int opCode = -1;
        opCode = RecvInt(new_server_socket);

        if(GLOBAL_DEBUG)
            cout << "opcode:" << opCode << endl;

        if (CheckMD5(opCode, MD5) == VALID) {
            switch (opCode) {
                case ADD_USER:
                    reCode = AddUser(new_server_socket, TMP_PATH);
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    break;
                case REMOVE_USER:
                    reCode = RemoveUser(new_server_socket);
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    break;
                case MOD_USER:
                    reCode = ModifyUser(new_server_socket);
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    break;
                case ADD_PIC:
                    reCode = AddPic(new_server_socket, TMP_PATH);
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    break;
                case REMOVE_PIC:
                    reCode = RemovePic(new_server_socket);
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    break;
                case INIT_CLIENT:
                    reCode = VALID;
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    SendAllInfo(new_server_socket);
                    break;
                case CHECK_KEY:
                    reCode = CheckKey(new_server_socket);
                    SimpleReply(new_server_socket, reCode);
                    break;
                case MOD_KEY:
                    reCode = ModifyKey(new_server_socket);
                    if (reCode == VALID) {
                        UpdateMD5();
                    }
                    SimpleReply(new_server_socket, reCode);
                    break;
                case CAM_STATE:
                    reCode = VALID;
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
			SendInt(new_server_socket, m_switchInfo);
                    SendInt(new_server_socket, m_state);
                    break;
                case CAM_CON:
                    reCode = SetCam(new_server_socket);
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    break;
                default:
                    reCode = INVALID;
                    SendInt(new_server_socket, reCode);
                    //SimpleReply(new_server_socket, reCode);
                    
                    if(GLOBAL_DEBUG)
                        cout << "unknown opCode!" << endl;
            }
            if(GLOBAL_DEBUG)
                cout << "recode:" << reCode << endl;
        } else {
            if(GLOBAL_DEBUG)
                cout << "MD5 INVALID!" << endl;

            reCode = INVALID_MD5;
            if (opCode == CHECK_KEY || opCode == MOD_KEY) {
                SimpleReply(new_server_socket, reCode);
            } else {
                SendInt(new_server_socket, reCode);
            }
            if(GLOBAL_DEBUG)
                cout << "recode:" << reCode << endl;
        }
        if (reCode == INVALID) {
            if(GLOBAL_DEBUG)
                cout << "clear socket:";

            char buffer[BUFFER_SIZE];
            int recvdLength = BUFFER_SIZE;
            while (recvdLength > 0) 
            {
                recvdLength = recv(new_server_socket, buffer, BUFFER_SIZE, 0);

                if(GLOBAL_DEBUG)
                    cout << recvdLength << "  ";
            }

            if(GLOBAL_DEBUG)
                cout << "cleared!" << endl;
        }
        //关闭与客户端的连接
        close(new_server_socket);
    }
    if(GLOBAL_DEBUG)
        printf("exit MyServer loop\n");
    pthread_exit( NULL);
    return VALID;
}

int MyServer::AddUser(int socket, string tmpPath) {
    string userName = RecvString(socket);
    if (userName == "") {
        if(GLOBAL_DEBUG)
            cout << "AddUser but the name is empty!" << endl;

        return INVALID;
    }
    if (m_model.AddUser(userName) == INVALID) {
        return INVALID;
    }
    string picName = RecvString(socket);
    if (userName == "") {
        if(GLOBAL_DEBUG)
            cout << "AddPic but the picName is empty!" << endl;
        return INVALID;
    }

    if (RecvPic(socket, tmpPath) != VALID) {
        if(GLOBAL_DEBUG)
            cout << "AddPic but the pic is invalid!" << endl;
        return INVALID;
    }
    return m_model.AddPic(userName, picName, tmpPath);
}

int MyServer::ModifyUser(int socket)
{
    if(GLOBAL_DEBUG)
        cout << "ModifyUser !" << endl;

    string userName = RecvString(socket);
    string newName = RecvString(socket);

    if(GLOBAL_DEBUG)
        cout << userName << newName << endl;

    if (userName == "") {
        if(GLOBAL_DEBUG)
            cout << "ModifyUser but the old name is empty!" << endl;

        return INVALID;
    }
    if (newName == "") {
        if(GLOBAL_DEBUG)
            cout << "ModifyUser but the new name is empty!" << endl;

        return INVALID;
    }
    return m_model.ModifyUser(userName, newName);
}

int MyServer::RemoveUser(int socket) {
    string userName = RecvString(socket);
    if (userName == "") 
    {
        if(GLOBAL_DEBUG)
            cout << "RemoveUser but the name is empty!" << endl;
        return INVALID;
    }
    return m_model.RemoveUser(userName);
}

int MyServer::AddPic(int socket, string tmpPath) {
    string userName = RecvString(socket);
    if (userName == "") 
    {
        if(GLOBAL_DEBUG)
            cout << "AddPic but the name is empty!" << endl;
        return INVALID;
    }
    string picName = RecvString(socket);
    if (userName == "") 
    {
        if(GLOBAL_DEBUG)
            cout << "AddPic but the picName is empty!" << endl;
        return INVALID;
    }

    if (RecvPic(socket, tmpPath) != VALID) 
    {
        if(GLOBAL_DEBUG)
            cout << "AddPic but the pic is invalid!" << endl;
        return INVALID;
    }
    return m_model.AddPic(userName, picName, tmpPath);
}

int MyServer::RemovePic(int socket) {
    string userName = RecvString(socket);
    if (userName == "") 
    {
        if(GLOBAL_DEBUG)
            cout << "RemovePic but the userName is empty!" << endl;
        return INVALID;
    }
    string picName = RecvString(socket);
    if (userName == "") 
    {
        if(GLOBAL_DEBUG)
            cout << "RemovePic but the picName is empty!" << endl;
        return INVALID;
    }
    return m_model.RemovePic(userName, picName);
}

int MyServer::RecvInt(int socket) {
    int res = -1;
    int length = 0;
    char buffer[INT_SIZE ];
    bzero(buffer, INT_SIZE);

    length = recv(socket, buffer, INT_SIZE, 0);

    if (length < INT_SIZE) 
    {
        if(GLOBAL_DEBUG)
            printf("Server Receive Data Failed!\n");
        return res;
    }
    // transform net to home
    memcpy(&res, buffer, INT_SIZE);
    //	cout << "the number is :" << res << endl;
    //res = ntohl(res);

    //if (res < 0) {
    //	printf("integer less than zero!\n");
    //	return INVALID;
    //}
    if(GLOBAL_DEBUG)
        cout << "starting recv int:" << res << endl;
    return res;
}

string MyServer::RecvString(int socket) {
    string res = "";
    int length = RecvInt(socket);
    int recvdLength = 0;
    if (length < 0) {
        return res;
    }
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    recvdLength = recv(socket, buffer, length, 0);
    if (recvdLength != length) {
        if(GLOBAL_DEBUG)
            printf("Some mistakes happen when server Receive string !\n");
    }
    res = buffer;

    if(GLOBAL_DEBUG)
        cout << "starting recv string:" << res << endl;
    return res;
}

int MyServer::RecvPic(int socket, string path) {
    int remainLength = RecvInt(socket);
    int recvdLength = 0;

    if (remainLength < 0) {
        return INVALID;
    }
    FILE *in;
    in = fopen(path.c_str(), "w+t");
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    while (remainLength > 0) {
        recvdLength = recv(socket, buffer, BUFFER_SIZE, 0);
        //cout << "recvlength : " << recvdLength << "\t";
        if (recvdLength <= 0) {
            if(GLOBAL_DEBUG)
                cout << "Didn't recv any pic data this time!" << endl;
        } else {

            fwrite(buffer, recvdLength, 1, in);
            remainLength -= recvdLength;
            //cout << "remainLength : " << remainLength << endl;
        }
    }
    fclose(in);
    return VALID;
}

int MyServer::SendInt(int socket, int data) {
    if(GLOBAL_DEBUG)
        cout << "sendint:" << data;

    data = htonl(data);
    int buffer[INT_SIZE ];
    memcpy(buffer, &data, INT_SIZE);
    if (write(socket, buffer, INT_SIZE) == INT_SIZE) 
    {
        if(GLOBAL_DEBUG)
            cout << "\t" << data << endl;
        return VALID;
    }
    return INVALID;
}
int MyServer::SendString(int socket, string data) {
    SendInt(socket, data.size());
    if (write(socket, data.c_str(), data.size()) == data.size()) 
    {
        if(GLOBAL_DEBUG)
            cout << "sendstring:" << data << endl;

        return VALID;
    }
    return INVALID;
}
int MyServer::SendPic(int socket, string path) {
    FILE *out = fopen(path.c_str(), "r");
    if (out == NULL) 
    {
        if(GLOBAL_DEBUG)
            cout << "fopen wrong" << endl;
        return INVALID;
    }
    char buffer[1024];
    int filesize = 0;
    struct stat statbuff;
    if (stat(path.c_str(), &statbuff) >= 0) {
        filesize = statbuff.st_size;
    }
    //cout << filesize << endl;
    SendInt(socket, filesize);
    int readsize = 1024;
    while (fread(buffer, readsize, 1, out) == 1) {
        //cout << "readsize :" << readsize << "\t";
        write(socket, buffer, readsize);
        filesize -= readsize;
        if (filesize < readsize) {
            readsize = filesize;
        }
    }
    if(GLOBAL_DEBUG)
        cout << "sendpic: OK!" << endl;

    fclose(out);
    return VALID;
}

int MyServer::SendAllInfo(int socket) {
    vector<string> users;
    m_model.GetUsers(users);

    int userSize = users.size();
    SendInt(socket, userSize);
    for (int i = 0; i < userSize; ++i) {
        int picSize;
        string dir;
        SendString(socket, users[i]);
        vector<string> pics;
        dir = m_model.GetPicsByUserName(users[i], pics);
        picSize = pics.size();
        SendInt(socket, picSize);
        for (int i = 0; i < picSize; ++i) {
            SendString(socket, pics[i]);
            SendPic(socket, dir + pics[i]);
        }
        pics.clear();
    }
    return VALID;
}

int MyServer::CheckKey(int socket) {
    string key = RecvString(socket);
    return m_model.CheckKey(key);
}

int MyServer::ModifyKey(int socket) {
    string key = RecvString(socket);
    string newKey = RecvString(socket);

    if(GLOBAL_DEBUG)
        cout << "oldkey:" << key << "newkey:" << newKey << endl;
    return m_model.ModifyKey(key, newKey);
}

void * thread_function(void * arg) 
{
    int res = 0;
    // block in this function
    res = cameraHandler(&MyServer::m_state, &MyServer::exitFlag);

    pthread_exit(NULL);
}

void MyServer::startCamThread()
{
    pthread_t a_thread;
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //    void * thread_result;
    int res = 0;

    if(GLOBAL_DEBUG)
        printf("enter thread\n");

    res = pthread_create(&a_thread, &attr, thread_function, NULL);

    if(GLOBAL_DEBUG)
        printf("over create thread\n");
}


int MyServer::SetCam(int socket) {
    int conCode = RecvInt(socket);
    if (conCode == CAM_ON) {
m_switchInfo = CAM_ON;

        exitFlag = false;
        // add cam thread
        if(GLOBAL_DEBUG)
            cout << "turn on CAM!" << endl;

        if(!isCamThreadStarted)
            startCamThread();
		

        BlinkLight::Out(BlinkLight::blue);
        isCamThreadStarted = true;
    } else if (conCode == CAM_OFF) {
m_switchInfo = CAM_OFF;

        exitFlag = true;
        BlinkLight::In(BlinkLight::blue);

        // exit cam thread

        if(GLOBAL_DEBUG)
            cout << "turn off CAM!" << endl;
    } else {

        if(GLOBAL_DEBUG)
            cout << "wrong conCode !" << endl;
        return INVALID;
    }
    return VALID;
}
string MyServer::RecvMD5(int socket) 
{
    if(GLOBAL_DEBUG)
        cout << "starting recv MD5..." << endl;
    string ret = RecvString(socket);

    if(GLOBAL_DEBUG)
    {
        cout << ret.length() << endl;
        cout << "client MD5:" << ret << " server: " << m_MD5 << endl;
    }
    return ret;
}

int MyServer::SendMD5(int socket) {
    return SendString(socket, m_MD5);
}

int MyServer::UpdateMD5() {
    /*
    char tmp[16];
    MD5Creator((void*) tmp);
    for (int i = 0; i < 16; i++) {
        //printf("%d  ",tmp[i] );
        tmp[i] = (tmp[i] + 256) % 128;
        //printf("%d\n",tmp[i] );
    }
    //cout << "create MD5 :" << tmp << endl;
    m_MD5 = string(tmp);
    //cout << "server MD5 :" << m_MD5 << endl;*/
    int seconds=0;
    seconds = time(NULL);
    stringstream strStream; 
    strStream << seconds;  
    string s = strStream.str();  

    CMD5 md5(s);
    m_MD5 = md5.GetMd5();

    return VALID;
}
int MyServer::CheckMD5(int opCode, string MD5) {
    if (opCode == CHECK_KEY) {
        //cout << "CHECKING...." << endl;
        if (MD5 == string(INIT_MD5)) {
            //cout << "update MD5" << endl;
            UpdateMD5();
            return VALID;
        }
        return VALID;
    } else if (m_MD5 == MD5) {
        return VALID;
    }
    return INVALID;
}
/*
   int MyServer::CreateMD5ForIP(string ip) {
   char tmp[16];
   MD5Creator((void*) tmp);
   m_ipAndMD5[ip] = string((char*) tmp);
   cout << "the md5 for ip: " << ip << "is: " << m_ipAndMD5[ip] << endl;
   return VALID;
   }

   int MyServer::CheckMD5(string userIP, string MD5) {
   if (m_ipAndMD5.find(userIP) != m_ipAndMD5.end()) {
   if (m_ipAndMD5[userIP].compare(MD5)) {

   return VALID;
   }
   }
   }*/

void* MyServer::ListenForCon(void *ptr) {
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = (INADDR_ANY);
    //这个端口要和广播方广播端口一致
    server_addr.sin_port = htons(2233);

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = (INADDR_ANY);
    client_addr.sin_port = htons(0);

    int client = socket(AF_INET, SOCK_DGRAM, 0);
    int server = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) 
    {
        if(GLOBAL_DEBUG)
            printf("error\n");
    }

    struct sockaddr_in from;
    int len = sizeof(from);
    int ret;
    char buff[50];

    while (1) 
    {
        if(GLOBAL_DEBUG)
            printf("recv...broadcast\n");
        ret = recvfrom(server, buff, 49, 0, (struct sockaddr *) &from, (socklen_t*) &len);
        if (ret > 0)
        {
            int num;
            num = atoi(buff);

            client_addr.sin_addr.s_addr = inet_addr(inet_ntoa(from.sin_addr));

            client_addr.sin_port = htons(num);

            if(GLOBAL_DEBUG)
                cout << "one user apply for connection: " << client_addr.sin_addr.s_addr << endl;

            char buffer[BUFFER_SIZE];
            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "3333");
            socklen_t n = sizeof(client_addr);


            if(GLOBAL_DEBUG)
                printf("enter the broadcast while\n");
		
	    for(int i = 0; i < 10; ++i){
            	sendto(server,buffer,BUFFER_SIZE,0,(struct sockaddr*)&client_addr,n);
                sleep(0.5f);
                if(GLOBAL_DEBUG)
                    printf("sleep1\n");
            }
        }
        if(GLOBAL_DEBUG)
            cout << "pthread one loop done!" << endl;
    }


    close(server);
    return NULL;
}

int MyServer::SimpleReply(int socket, int reCode) {
    if (reCode == INVALID) {
        SendString(socket, INIT_MD5);
        SendInt(socket, reCode);
    } else {
        SendMD5(socket);
        SendInt(socket, reCode);
    }
    return VALID;
}

string MyServer::GetUserIp() {
    return USER_IP;
}
