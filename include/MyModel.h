/*
 * MyModel.h
 *
 *  Created on: 2015年5月1日
 *      Author: xqf
 */

#ifndef SRC_MYMODEL_H_
#define SRC_MYMODEL_H_

#include "consts.h"
#include "general.h"

class MyModel {

private:
	map<string, vector<string> > m_allPicNamesForUserId;
	map<string,string> m_getUserIdByUserName;
	string m_key;
private:
	void OutputPics ( std::ostream &os);
	void OutputUserName ( std::ostream &os );
	void UpdateFiles();
	void UpdatePicFile();
	void UpdateUserFile();
    void UpdateImagesNumberFile();

public:
	MyModel();
	virtual ~MyModel();

	int AddUser(string userName);
	int AddPic(string userName, string picName, string srcPath);
	int CheckKey(string key);
	int ModifyKey(string key, string newKey);
	string CreateID( );
	int ModifyUser(string userName, string newName);
	void OutPutFiles(std::ostream &os);
	int RemovePic(string userName, string picName);
	int RemoveUser(string userName);
	int ReplacePic(string userName, string picName, string srcPath);
	int ValidateData();

	int GetUsers(vector<string> &users);
	string GetPicsByUserName(string userName, vector<string> &pics);
};

#endif /* SRC_MYMODEL_H_ */
