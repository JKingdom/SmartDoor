/*************************************************************************
  > File Name: BlinkLight.cpp
  > Author: 
  > Mail: 
  > Created Time: 2015年06月04日 星期四 22时29分11秒
 ************************************************************************/

#include"BlinkLight.h"
#include <wiringPi.h>
#include<iostream>
#include<stdexcept>
#include "debug.h"

void BlinkLight::setup()
{
    wiringPiSetup () ;
}

void BlinkLight::blink(int count, int gpio)
{
    try{
        pinMode (gpio, OUTPUT) ;
        int i;
        for (i = 0; i < count; i++)
        {
            digitalWrite (gpio, HIGH) ; delay (500) ;
            digitalWrite (gpio,  LOW) ; delay (500) ;
        }
    } catch (std::exception &e)
    {
        if(GLOBAL_DEBUG)
        {
            std::cout << "gpio light blink error!\n" << std::endl;
            std::cout << e.what() << std::endl;
        }
    }

    digitalWrite(gpio, HIGH);
}

void BlinkLight::setAllOut()
{
    pinMode(red, OUTPUT);
    pinMode(blue, OUTPUT);
    pinMode(white, OUTPUT);
    digitalWrite (red, HIGH) ; 
    digitalWrite (blue, HIGH) ; 
    digitalWrite (white, HIGH) ; 
}

void BlinkLight::setAllIn()
{
    pinMode(red, OUTPUT);
    pinMode(blue, OUTPUT);
    pinMode(white, OUTPUT);
    digitalWrite (red, LOW) ; 
    digitalWrite (blue, LOW) ; 
    digitalWrite (white, LOW) ;
}

void BlinkLight::Out(int gpio)
{
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, HIGH);
}

void BlinkLight::In(int gpio)
{
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, LOW);
}

