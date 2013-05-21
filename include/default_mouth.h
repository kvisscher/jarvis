#ifndef DEFAULT_MOUTH_H
#define DEFAULT_MOUTH_H

#include "mouth.h"

#include <portaudio.h>
#include "speak_lib.h"

struct StreamData {
	int numSamples;
	short* wav;
	PaStream* stream;
};

class DefaultMouth : public Mouth
{
private:
	PaStream* outputStream;
	
	static int eSpeakCallbackWrapper(short* wav, int numsamples, espeak_EVENT* events);
	
	static PaError portAudioCallbackWrapper(void* inputBuffer, void* outputBuffer,
											unsigned long framesPerBuffer,
											PaTimestamp outTime, void *userData);	
public:
	DefaultMouth();
	virtual ~DefaultMouth();
	virtual void speak(const std::string& text);
};

#endif
