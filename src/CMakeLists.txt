# 添加头文件查找目录
include_directories(${PROJECT_SOURCE_DIR}/include)
message("current project src is " ${PROJECT_SOURCE_DIR})

# 指定科执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# 设置编译文件
set(SOURCE_LISTS main.cpp CameraHandler.cpp general.cpp ImageDataBase.cpp MyModel.cpp MyServer.cpp md5.cpp BlinkLight.cpp) #MD5Creator.cpp)
find_package(OpenCV REQUIRED)
find_package(Threads)


# 指定相关原文件
add_executable(SmartCamDoor ${SOURCE_LISTS})


# 线程链接库
target_link_libraries(SmartCamDoor ${CMAKE_THREAD_LIBS_INIT})
# opencv 链接库
target_link_libraries(SmartCamDoor ${OpenCV_LIBS})
# wiringPi.h#
target_link_libraries(SmartCamDoor wiringPi)
