#ifndef DEFAULT_EARS_H
#define DEFAULT_EARS_H

#include "ears.h"

#include <pocketsphinx>
#include <portaudio>

class DefaultEars : public Ears
{
private:
	ps_decoder_t* ps;
	PaStream* stream;
	
	void initializePortAudio() throws();
	int onPortAudioCallback(const void* input, void* output,
				unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags, void *userData) throws();	
public:
	DefaultEars();
	virtual ~DefaultEars();
};

#endif
