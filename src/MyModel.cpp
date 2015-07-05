/*
 * MyModel.cpp
 *
 *  Created on: 2015年5月1日
 *      Author: xqf
 */

#include "MyModel.h"
#include "debug.h"
#include "ImageDataBase.h"

MyModel::MyModel() {

    if(access(ROOT_DIR, F_OK) != 0) {
        mkdir(ROOT_DIR,0755);
    }
    if(access(IMAGE_DIR, F_OK) != 0) {
        mkdir(IMAGE_DIR,0755);
    }

    string imagesNumberFileName = string(ROOT_DIR) + "imagesNumber";
    int imagesNumber = 0;
    if(access(imagesNumberFileName.c_str(), F_OK) != 0)
    {
        imagesNumber = ImageDataBaseModel::getInstance().getImagesNumber();

        ofstream ofsNumber(imagesNumberFileName.c_str(),ios::out | ios::binary | ios::trunc);
        if (ofsNumber.is_open()) {
            ofsNumber << imagesNumber;

            if(GLOBAL_DEBUG)
                printf("in MyModel init, imagesNumber=%d\n",imagesNumber);

        }
        ofsNumber.close();

    } else 
    {

        ifstream ifsNumber(imagesNumberFileName.c_str(), ios::in | ios::binary);
        if (ifsNumber.is_open()) {
            ifsNumber >> imagesNumber;

            if(GLOBAL_DEBUG)
                printf("in MyModel init, imagesNumber=%d\n",imagesNumber);

            ImageDataBaseModel::getInstance().setImagesNumber(imagesNumber);

            if(imagesNumber>=2)
                ImageDataBaseModel::getInstance().reloadModel();
        }
        ifsNumber.close();

    }

    string fileName = string(ROOT_DIR) + "key";
    if(access(fileName.c_str(), F_OK) != 0) {
        m_key = "123456";
        ofstream ofsKey(fileName.c_str(),ios::out | ios::binary | ios::trunc);
        if (ofsKey.is_open()) {
            ofsKey << m_key;
        }
        ofsKey.close();
        //noKeyFile
    } else {
        ifstream ifsKey(fileName.c_str(), ios::in | ios::binary);
        if (ifsKey.is_open()) {
            ifsKey >> m_key;
        }
        ifsKey.close();
    }

    string userName;
    string userID;
    fileName = string(ROOT_DIR) + "UserName-UserID";
    ifstream fsUserNameToID(fileName.c_str(), ios::in | ios::binary);

    if (fsUserNameToID.is_open()) {
        while (!fsUserNameToID.eof()) {
            userName = "";
            userID = "";
            fsUserNameToID >> userName >> userID;
            if (userName != "") {
                m_getUserIdByUserName[userName] = userID;
            }
        }
        fsUserNameToID.close();
    } else {
        FILE* fp = fopen(fileName.c_str(), "a");
        fclose(fp);
    }

    string picName;
    int picNum;
    fileName = string(ROOT_DIR) + "UserID-Pic";
    ifstream fsPicsForUserID(fileName.c_str(), ios::in | ios::binary);

    if (fsPicsForUserID.is_open()) {
        while (!fsPicsForUserID.eof()) {
            vector<string> picNames;
            userID = "";
            picName = "";
            picNum = 0;
            fsPicsForUserID >> userID >> picNum;
            if (userID != "") {
                for (; picNum > 0; --picNum) {
                    picName = "";
                    fsPicsForUserID >> picName;
                    picNames.push_back(picName);
                }
                m_allPicNamesForUserId[userID] = picNames;
            }
        }
        fsPicsForUserID.close();
    } else {
        FILE* fp = fopen(fileName.c_str(), "a");
        fclose(fp);
    }

    ValidateData();
}

MyModel::~MyModel() {
    // TODO Auto-generated destructor stub
    UpdateFiles();
    m_getUserIdByUserName.clear();
    map<string, vector<string> >::iterator iter =
        m_allPicNamesForUserId.begin();
    for (; iter != m_allPicNamesForUserId.end(); ++iter) {
        iter->second.clear();
    }
    m_allPicNamesForUserId.clear();
}

void MyModel::UpdateFiles() {
    UpdateUserFile();
    UpdatePicFile();
}

void MyModel::UpdatePicFile() {
    string fileName = string(ROOT_DIR) + "UserID-Pic";
    ofstream fsPicsForUserID(fileName.c_str(),
            ios::out | ios::binary | ios::trunc);
    OutputPics(fsPicsForUserID);
    fsPicsForUserID.close();
}

void MyModel::UpdateUserFile() {
    string fileName = string(ROOT_DIR) + "UserName-UserID";
    ofstream fsUserNameToID(fileName.c_str(),
            ios::out | ios::binary | ios::trunc);
    OutputUserName(fsUserNameToID);
    fsUserNameToID.close();
}

void MyModel::UpdateImagesNumberFile() 
{
    string imagesNumberFileName = string(ROOT_DIR) + "imagesNumber";
    int imagesNumber = ImageDataBaseModel::getInstance().getImagesNumber();

    ofstream ofsNumber(imagesNumberFileName.c_str(),ios::out | ios::binary | ios::trunc);
    if (ofsNumber.is_open()) {
        ofsNumber << imagesNumber;
    }
    ofsNumber.close();
}

void MyModel::OutputPics(ostream &os) {
    map<string, vector<string> >::iterator it = m_allPicNamesForUserId.begin();
    //cout << "beginning OutPutPics!" << endl;
    for (; it != m_allPicNamesForUserId.end(); ++it) {
        os << it->first << " " << it->second.size() << endl;
        vector<string>::iterator iit = it->second.begin();
        for (; iit != it->second.end(); ++iit) {
            os << iit->c_str() << endl;
        }
    }
}

void MyModel::OutPutFiles(std::ostream &os) {
    OutputUserName(os);
    OutputPics(os);
}

void MyModel::OutputUserName(ostream &os) {
    map<string, string>::iterator it = m_getUserIdByUserName.begin();
    for (; it != m_getUserIdByUserName.end(); ++it) {
        os << it->first << " " << it->second << endl;
    }
}

int MyModel::AddPic(string userName, string picName, string srcPath) {
    string dest = IMAGE_DIR + m_getUserIdByUserName[userName] + "/" + picName;
    if (CopyPic(dest, srcPath) == VALID) {
        m_allPicNamesForUserId[m_getUserIdByUserName[userName]].push_back(picName);
        UpdatePicFile();


        // when addPic finished, the Pic Database should be reload to new model!
        ImageDataBaseModel::getInstance().reloadModel();

        UpdateImagesNumberFile();

        return VALID;
    }
    return INVALID;
}

int MyModel::AddUser(string userName) {
    map<string, string>::iterator iter = m_getUserIdByUserName.find(userName);
    if (iter != m_getUserIdByUserName.end()) {
        if(GLOBAL_DEBUG)
            cout << "the userName " << userName << " exists!" << endl;
        return INVALID; // the userName exists
    }
    m_getUserIdByUserName[userName] = CreateID();
    mkdir((IMAGE_DIR + m_getUserIdByUserName[userName]).c_str(), 0755);
    UpdateUserFile();
    return VALID;
}

string MyModel::CreateID() {
    string newID;
    int flag = 1;

    //cout << "CreateID begin...." << endl;
    while (flag) {
        newID = "";
        int i = 4;
        for (; i > 0; --i) {
            int a;
            char c;
            a = rand() % 10;
            c = 48 + a;
            newID = newID + c;
        }
        flag = 0;
        map<string, string>::iterator it = m_getUserIdByUserName.begin();
        for (; it != m_getUserIdByUserName.end(); ++it) {
            if (it->second == newID) {
                flag = 1;
                //cout << "reset newID..." << endl;
                break;
            }
            //	cout << "checking..." << endl;
        }
    }
    //cout << "CreateID end!" << endl;
    return newID;
}

int MyModel::CheckKey(string key) {
    if( key == m_key)
    {
        return VALID;
    } else {
        return INVALID;
    }
}

int MyModel::ModifyKey(string key, string newKey) {
    if( key == m_key)
    {
        string fileName = string(ROOT_DIR) + "key";
        ofstream ofsKey(fileName.c_str(), ios::in | ios::binary | ios::trunc);
        if (ofsKey.is_open()) {
            m_key = newKey;
            ofsKey << m_key;
            return VALID;
        } else  {
            if(GLOBAL_DEBUG)
                cout << "can't open key file when modify key!" << endl;
        }
    }
    return INVALID;
}

int MyModel::ReplacePic(string userName, string picName, string srcPath) {
    string dest = IMAGE_DIR + m_getUserIdByUserName[userName] + "/" + picName;
    if (CopyPic(dest, srcPath)) {
        m_allPicNamesForUserId[m_getUserIdByUserName[userName]].push_back(
                picName);
        UpdatePicFile();
    }
    return INVALID;
}

int MyModel::RemovePic(string userName, string picName) {
    if(GLOBAL_DEBUG)
        cout << "removePic" << endl;

    vector<string>::iterator it =
        m_allPicNamesForUserId[m_getUserIdByUserName[userName]].begin();
    while (it != m_allPicNamesForUserId[m_getUserIdByUserName[userName]].end()) {
        if (*it == picName) {
            m_allPicNamesForUserId[m_getUserIdByUserName[userName]].erase(it ++);
            if(GLOBAL_DEBUG)
                cout << "deletePicName !" << endl;\
                    string dest = IMAGE_DIR + m_getUserIdByUserName[userName] + "/"
                    + picName;
            remove(dest.c_str());
            if(GLOBAL_DEBUG)
                cout << "updataFiles" << endl;
            UpdatePicFile();

            // when delete imgs, the datebase should be reload to new model!
            ImageDataBaseModel::getInstance().reloadModel();
            UpdateImagesNumberFile();

            return VALID;
        } else {
            it++;
        }
    }
    if(GLOBAL_DEBUG)
        cout << "not find PicName !" << endl;
    return INVALID;
}

int MyModel::RemoveUser(string userName) {
    map<string, string>::iterator iter = m_getUserIdByUserName.find(userName);
    if (iter != m_getUserIdByUserName.end()) {
        map<string, vector<string> >::iterator it = m_allPicNamesForUserId.find(
                m_getUserIdByUserName[userName]);
        string userDir = IMAGE_DIR + m_getUserIdByUserName[userName];
        RemoveDir(userDir);
        remove((IMAGE_DIR + m_getUserIdByUserName[userName]).c_str());
        it->second.clear();
        m_allPicNamesForUserId.erase(it);
        m_getUserIdByUserName.erase(iter);
        UpdateFiles();
        return VALID;
    }
    return INVALID;
}

int MyModel::ModifyUser(string userName, string newName) {
    map<string, string>::iterator iter = m_getUserIdByUserName.find(userName);
    if (iter != m_getUserIdByUserName.end()) {
        map<string, string>::iterator iter2 = m_getUserIdByUserName.find(
                newName);
        if (iter2 == m_getUserIdByUserName.end()) {
            m_getUserIdByUserName[newName] = iter->second;
            m_getUserIdByUserName.erase(iter);
            UpdateFiles();
            return VALID;
        }
        return INVALID;
    }
    return INVALID;
}

int MyModel::GetUsers(vector<string> &users) {
    map<string, string>::iterator iter = m_getUserIdByUserName.begin();
    while (iter != m_getUserIdByUserName.end()) {
        users.push_back(iter->first);
        iter++;
    }
    return VALID;
}

string MyModel::GetPicsByUserName(string userName, vector<string> &pics) {
    map<string, string>::iterator iter = m_getUserIdByUserName.find(userName);
    if (iter != m_getUserIdByUserName.end()) {
        pics = m_allPicNamesForUserId[m_getUserIdByUserName[userName]];
        return IMAGE_DIR + m_getUserIdByUserName[userName] + "/";
    }
    if(GLOBAL_DEBUG)
        cout << "no userName:" << userName << endl;
    return "";
}

int MyModel::ValidateData() {
    vector<string> userIDs;
    DIR *dir;
    struct dirent *ptr;
    if ((dir = opendir(IMAGE_DIR)) == NULL) {
        perror("Open dir error...");
        exit(1);
    }
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 4) { // dir
            userIDs.push_back(ptr->d_name);
        }
    }
    // clear useless users
    map<string, string>::iterator iterNameID = m_getUserIdByUserName.begin();
    int useless = 0;
    while (iterNameID != m_getUserIdByUserName.end()) {
        vector<string>::iterator iterIDs = userIDs.begin();
        useless = 1;
        while (iterIDs != userIDs.end()) {
            if (iterNameID->second == *iterIDs) {
                useless = 0;
                break;
            }
            ++iterIDs;
        }
        if (useless) {
            m_allPicNamesForUserId[iterNameID->second].clear();
            m_allPicNamesForUserId.erase(iterNameID->second);
            m_getUserIdByUserName.erase(iterNameID++);
        }
        ++iterNameID;
    }

    /*	// clear useless dirs
        vector<string>::iterator iterIDs = userIDs.begin();
        useless = 0;
        for (; iterIDs != userIDs.end(); ++iterIDs) {
        map<string, string>::iterator iterNameID =
        m_getUserIdByUserName.begin();
        useless = 1;
        for (; iterNameID != m_getUserIdByUserName.end(); ++iterNameID) {
        if (iterNameID->second == *iterIDs) {
        useless = 0;
        break;
        }
        }
        if (useless) {
        string userDir = IMAGE_DIR + *iterIDs;
        RemoveDir(userDir);
        userIDs.erase(iterIDs);
        }
        }*/
    //clear useless picNames or picFiles
    //cout << "clear useless picNames or picFiles" << endl;
    iterNameID = m_getUserIdByUserName.begin();
    while (iterNameID != m_getUserIdByUserName.end()) {
        vector<string>::iterator iterPic =
            m_allPicNamesForUserId[iterNameID->second].begin();
        while (iterPic != m_allPicNamesForUserId[iterNameID->second].end()) {
            string picPath = IMAGE_DIR + iterNameID->second + "/" + *iterPic;
            if (access(picPath.c_str(), F_OK) != 0) {
                iterPic = m_allPicNamesForUserId[iterNameID->second].erase(
                        iterPic);
                if(GLOBAL_DEBUG)
                    cout << "clear useless " + picPath << endl;
            } else {
                iterPic++;
            }
        }
        ++iterNameID;
    }

    UpdateFiles();
    return VALID;
}
