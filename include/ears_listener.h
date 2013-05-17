#ifndef EARS_LISTENER_H
#define EARS_LISTENER_H

class EarsListener
{
public:
	EarsListener();
	virtual ~EarsListener();
	virtual void notifyHeardText(const std::string& text);
};

#endif
