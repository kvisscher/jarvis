#include "default_mouth.h"

#include <iostream>

#include <portaudio.h>
#include "speak_lib.h"

DefaultMouth::DefaultMouth()
{
	int sample_rate = espeak_Initialize(
			AUDIO_OUTPUT_RETRIEVAL,
			0, // Buffer length
			NULL, // espeak data path, null for default
			0 // options?
	);

	// Let espeak use the english voice
	espeak_SetVoiceByName("english-us");
	
	// Callback handler for synth
	espeak_SetSynthCallback(&DefaultMouth::eSpeakCallbackWrapper);
	
//	Pa_Initialize();
}

DefaultMouth::~DefaultMouth()
{

}

void DefaultMouth::speak(const std::string& text)
{

	return;

	espeak_Synth(
		text.c_str(),
		text.length(),
		0,
		POS_CHARACTER,
		0,
		espeakCHARS_AUTO, 
		NULL, 
		this
	);
        
	if (espeak_Synchronize() < 0)
	{
		std::cout << "failed to synthesize word: " << text << std::endl;
	}
}

int DefaultMouth::eSpeakCallback(short* wav, int numsamples)
{
	if (numsamples == 0)
	{
		std::cout << "no samples to play" << std::endl;
		return 0;
	}

	return 0;
	
	/*
	PaStream* stream;
        
    StreamData data;
    data.numSamples = numsamples;
    data.wav = wav;
    data.stream = stream;
        
	PaError result = Pa_OpenStream(
		&stream,
		paNoDevice,
		0, // Number of input channels to use
		0, // Sample format for input
		NULL, // Input device driver info. Optional
		Pa_GetDefaultOutputDeviceID(), // ID of output device
		2, // Num channels to use for output. 2 for stereo
		paInt16, // Sample format?
		NULL, // ?
		11000, // Sample rate
		8092, // framesPerBuffer. Not sure
		1, // The number of buffers. Also, not sure :D
		paNoFlag, // No special flags for the stream
		&DefaultMouth::portAudioCallbackWrapper,
		&data
	);
	
	st--d::cout << "num samples: " << data.numSamples << std::endl;	*/
}

int DefaultMouth::eSpeakCallbackWrapper(short* wav, int numsamples, espeak_EVENT* events)
{
	if (numsamples == 0)
	{
		std::cout << "No samples to play" << std::endl;
		return 0;
	}

	DefaultMouth* mouth = static_cast<DefaultMouth*>(events->user_data);
	return mouth->eSpeakCallback(wav, numsamples);
}

PaError DefaultMouth::portAudioCallbackWrapper(void* inputBuffer, void* outputBuffer,
										unsigned long framesPerBuffer,
										double outTime, void *userData)
{
	std::cout << "Portaudio callback" << std::endl;
	
	StreamData* data = static_cast<StreamData*>(userData);
	
	std::cout << "num samples: " << data->numSamples << std::endl;
}
