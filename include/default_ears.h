#ifndef DEFAULT_EARS_H
#define DEFAULT_EARS_H

#include "ears.h"

#include <pocketsphinx.h>
#include <portaudio.h>

class DefaultEars : public Ears
{
private:
	ps_decoder_t* ps;
	PaStream* stream;
	
	void initializePortAudio() throw(EarsException);
	PaError onPortAudioCallback(void* inputBuffer, void* outputBuffer,
                                                unsigned long framesPerBuffer,
                                                PaTimestamp outTime);
	static PaError portAudioCallbackWrapper(void* inputBuffer, void* outputBuffer,
						unsigned long framesPerBuffer,
						PaTimestamp outTime, void *userData);
public:
	DefaultEars() throw(EarsException);
	virtual ~DefaultEars();
};

#endif
