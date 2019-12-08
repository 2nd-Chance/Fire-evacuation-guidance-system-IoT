#ifndef GENERIC_CONTROL_CPP
#define GENERIC_CONTROL_CPP

#include "GenericControl.h"

#define LED_R 1
#define LED_G 2
#define LED_B 3

template <class T>
GenericControl<T>::GenericControl(void)
{
	if(wiringPiSetup() < 0)
		std::cout << "Unable to setup wiringPi: " << strerror(errno) << std::endl;

	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	pinMode(LED_B, OUTPUT);
}

template <class T>
T GenericControl<T>::GetValue(void)
{
	return m_value; 
}

template <class T>
void GenericControl<T>::SetValue(const T& value)
{
	m_value = value;
}

template <class T>
void GenericControl<T>::Control(void)
{
	switch(m_value) {
	case 0:
		digitalWrite(LED_R, HIGH);
		digitalWrite(LED_G, LOW);
		digitalWrite(LED_B, LOW);
		break;
	case 1:
		digitalWrite(LED_R, LOW);
		digitalWrite(LED_G, HIGH);
		digitalWrite(LED_B, LOW);
		break;
	case 2:
		digitalWrite(LED_R, LOW);
		digitalWrite(LED_G, LOW);
		digitalWrite(LED_B, HIGH);
		break;
	case 3:
		digitalWrite(LED_R, HIGH);
		digitalWrite(LED_G, HIGH);
		digitalWrite(LED_B, HIGH);
		break;
	default:
		digitalWrite(LED_R, HIGH);
		digitalWrite(LED_G, HIGH);
		digitalWrite(LED_B, HIGH);
	}
}

#endif
