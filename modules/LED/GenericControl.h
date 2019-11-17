#ifndef GENERIC_CONTROL_H__
#define GENERIC_CONTROL_H__

#include <iostream>
#include <string>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>

template <class T>
class GenericControl
{
public:
    GenericControl(void);
	T GetValue(void);
    void SetValue(const T& value);
    void Control(void);
private:
	T m_value;
};
#include "GenericControl.cpp"

#endif
