/*
 * MyServer.h
 *
 *  Created on: 2015年5月1日
 *      Author: xqf
 */

#ifndef SRC_MYSERVER_H_
#define SRC_MYSERVER_H_

#include "consts.h"
#include "MyModel.h"
//#include "MD5Creator.h"
#include "md5.h"


class MyServer {

private:
	int m_socket;
    struct sockaddr_in m_server_addr;
    int m_keepRunning;
    MyModel m_model;

	int m_switchInfo;

    //map<string, string> m_ipAndMD5;
    string m_MD5;
    bool isCamThreadStarted;

    // new add!
    // flag to exit the cam thread;
public:
    static int m_state;
    static bool exitFlag;

private:
    void startCamThread();

public:
//	MyServer(int &state);
	MyServer();
	virtual ~MyServer();

	int AddUser(int socket, string tmpPath);
	int AddPic(int socket, string tmpPath);
	int RemoveUser(int socket);
	int RemovePic(int socket);
	int ModifyUser(int socket);

	int RecvInt(int socket);
	string RecvString(int socket);
	int RecvPic(int socket, string path);
	int SendInt(int socket, int data);
	int SendString(int socket, string data);
	int SendPic(int socket,  string path);

	int CheckKey(int socket);
	int ModifyKey(int socket);

	string RecvMD5(int socket);
	int SendMD5(int socket);
	//int CreateMD5ForIP(string ip);
	//int CheckMD5(string userIP, string MD5);
	int UpdateMD5();
	int CheckMD5( int opCode,string MD5);

	int SendAllInfo(int socket);
	int SetCam(int socket);

	int SimpleReply(int socket, int reCode);

	int Start();
	int Usecase();

	static void* ListenForCon(void *ptr);
	string GetUserIp();
};



#endif /* SRC_MYSERVER_H_ */
