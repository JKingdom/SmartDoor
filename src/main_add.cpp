/*
 * main.cpp
 *
 *  Created on: 2015年5月1日
 *      Author: xqf
 */
#include "consts.h"
#include "MyServer.h"
#include "general.h"
#include "MD5Creator.h"

int state;

int main() {
	state = 5;
	 MyServer server(state);
	 server.Start();
	return 0;
}
