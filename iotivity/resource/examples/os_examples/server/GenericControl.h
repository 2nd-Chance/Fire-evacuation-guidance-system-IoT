#ifndef GENERIC_CONTROL_H__
#define GENERIC_CONTROL_H__

class GenericControl
{
public:
    GenericControl(void);
	string GetValue(void);
	void SetValue(const string& value);
	void Control(const string& value);
private:
	string m_value;
};

#endif
