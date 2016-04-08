#ifndef BUTTON_EVENT_ARGS_H
#define BUTTON_EVENT_ARGS_H

#include "IEventArgs.h"

enum Key
{
	LEFT_ARROW,
	RIGHT_ARROW,
	UP_ARROW,
	DOWN_ARROW,
};

// ButtonEventArgs 
class ButtonEventArgs : public IEventArgs
{
private:

	Key key; 
	bool released; 

public:

	ButtonEventArgs(Key key, bool released) : IEventArgs(ButtonEvent)
	{
		this->key = key; 
		this->released = released; 
	}

	bool Released() const
	{
		return this->released;
	}

	Key Key() const
	{
		return this->key;
	}

	virtual ~ButtonEventArgs() {}; 
};

#endif // BUTTON_EVENT_ARGS_H