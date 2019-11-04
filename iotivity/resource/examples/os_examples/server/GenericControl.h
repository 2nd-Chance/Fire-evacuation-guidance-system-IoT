#ifndef GENERIC_CONTROL_H__
#define GENERIC_CONTROL_H__

class GenericControl
{
public:
    GenericControl(void);

	string GetValue(void);

	void Control(const string& value);
};

#endif
