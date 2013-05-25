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

	int onPortAudioCallback(const void *input, void *output,
				unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo,
				PaStreamCallbackFlags statusFlags);
	

	static int onPortAudioCallbackWrapper(const void *input, void *output,
					      unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo,
					      PaStreamCallbackFlags statusFlags, void *userData);
public:
	DefaultEars() throw(EarsException);
	virtual ~DefaultEars();
};

#endif
