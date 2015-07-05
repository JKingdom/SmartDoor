/*
 * general.cpp
 *
 *  Created on: 2015年5月2日
 *      Author: xqf
 */
#include "general.h"

string CurrentTime() {
	string str;
	time_t now;
	char buf[32];
	time(&now);
	tm *timenow = localtime(&now);
	sprintf(buf, "%d", timenow->tm_year);
	str = buf;
	sprintf(buf, "%d", timenow->tm_mon + 1);
	str = str + "." + buf;
	sprintf(buf, "%d", timenow->tm_mday);
	str = str + "." + buf;
	sprintf(buf, "%d", timenow->tm_hour);
	str = str + "." + buf;
	sprintf(buf, "%d", timenow->tm_min);
	str = str + "." + buf;
	sprintf(buf, "%d", timenow->tm_sec);
	str = str + "." + buf;
	return str;
}

int RemoveDir(string userDir) {
	DIR *dir;
	struct dirent *ptr;
	if ((dir = opendir(userDir.c_str())) == NULL) {
		perror("Open dir error...");
		return INVALID;
	}
	while ((ptr = readdir(dir)) != NULL) {
		//cout << ptr->d_name << endl;
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) ///current dir OR parrent dir
			continue;
		else {
			remove((userDir+ "/"+ ptr->d_name).c_str());
			//cout << (string(ROOT_DIR) + ptr->d_name).c_str();
		}
	}
	return VALID;
}

int CopyPic(string dest, string src) {
	int from_fd, to_fd;
	int bytes_read, bytes_write;
	char buffer[BUFFER_SIZE];
	char *ptr;
	/* 打开源文件 */
	if ((from_fd = open(src.c_str(), O_RDONLY)) == -1) /*open file readonly,返回-1表示出错，否则返回文件描述符*/
	{
		fprintf(stderr, "Open %s Error:%s\n", src.c_str(), strerror(errno));
		exit(1);
	}
	/* 创建目的文件 */
	/* 使用了O_CREAT选项-创建文件,open()函数需要第3个参数,
	 mode=S_IRUSR|S_IWUSR表示S_IRUSR 用户可以读 S_IWUSR 用户可以写*/
	if ((to_fd = open(dest.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR))
			== -1) {
		fprintf(stderr, "Open %s Error:%s\n", dest.c_str(), strerror(errno));
		exit(1);
	}
	/* 以下代码是一个经典的拷贝文件的代码 */
	while (bytes_read = read(from_fd, buffer, BUFFER_SIZE)) {
		/* 一个致命的错误发生了 */
		if ((bytes_read == -1) && (errno != EINTR))
			break;
		else if (bytes_read > 0) {
			ptr = buffer;
			while (bytes_write = write(to_fd, ptr, bytes_read)) {
				/* 一个致命错误发生了 */
				if ((bytes_write == -1) && (errno != EINTR))
					break;
				/* 写完了所有读的字节 */
				else if (bytes_write == bytes_read)
					break;
				/* 只写了一部分,继续写 */
				else if (bytes_write > 0) {
					ptr += bytes_write;
					bytes_read -= bytes_write;
				}
			}
			/* 写的时候发生的致命错误 */
			if (bytes_write == -1)
				break;
		}
	}
	close(from_fd);
	close(to_fd);
	return VALID;
}

