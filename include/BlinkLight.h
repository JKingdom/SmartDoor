/*************************************************************************
	> File Name: BlinkLight.h
	> Author: 
	> Mail: 
	> Created Time: 2015年06月04日 星期四 22时29分50秒
 ************************************************************************/

#ifndef _BLINKLIGHT_H
#define _BLINKLIGHT_H
class BlinkLight
{
public:
    static void setup();
    static void blink(int count, int gpio);
    static void Out(int gpio);
    static void In(int gpio);
    static void setAllOut();
    static void setAllIn();

public:
    static const int red = 8; // gpio2
    static const int blue = 9; // gpio3
    static const int white = 7; // gpio4
};

#endif
