#include "default_ears.h"

#include <iostream>

// TODO: Probably don't want to hardcode these values
#define ACOUSTIC_MODEL "hmm/en_US/hub4wsj_sc_8k"
#define DICT "9133.dic"
#define LANG_MODEL "7859.lm"

#define BUF_SIZE 1024 << 6

DefaultEars::DefaultEars() throw(EarsException)
{
	ps_decoder_t* ps;
	cmd_ln_t* config;
	
	config = cmd_ln_init(
			NULL,
			ps_args(),
			TRUE,
	                "-hmm", ACOUSTIC_MODEL,
	                "-lm", LANG_MODEL,
	                "-dict", DICT,
	                NULL
		);

	if (config == NULL)
	{
		throw EarsException("error while initializing pocketsphinx config");
	}
	
	ps = ps_init(config);

	if (ps == NULL)
	{
		throw EarsException("error initializing pocketsphinx decoder");
	}
	
	this->initializePortAudio();
}

DefaultEars::~DefaultEars()
{
	if (this->ps)
	{
		ps_free(this->ps);
	}
	
	if (this->stream)
	{
		Pa_StopStream(this->stream);
	}
	
	Pa_Terminate();
}

/*

PaError Pa_OpenStream( PortAudioStream** stream,
                       PaDeviceID inputDevice,
                       int numInputChannels,
                       PaSampleFormat inputSampleFormat,
                       void *inputDriverInfo,
                       PaDeviceID outputDevice,
                       int numOutputChannels,
                       PaSampleFormat outputSampleFormat,
                       void *outputDriverInfo,
                       double sampleRate,
                       unsigned long framesPerBuffer,
                       unsigned long numberOfBuffers,
                       PaStreamFlags streamFlags,
                       PortAudioCallback *callback,
                       void *userData );

*/

int DefaultEars::portAudioCallbackWrapper(void* inputBuffer, void* outputBuffer,
					  unsigned long framesPerBuffer, 
					  PaTimestamp outTime, void *userData)
{
	DefaultEars* ears = static_cast<DefaultEars*>(userData);
	return ears->onPortAudioCallback(inputBuffer, outputBuffer, framesPerBuffer, outTime);
}

void DefaultEars::initializePortAudio() throw(EarsException)
{
	Pa_Initialize();

	PaError result = Pa_OpenStream(
		&this->stream,
		Pa_GetDefaultInputDeviceID(),
		1, // Number of input channels to use
		paInt16, // Sample format for input
		NULL, // Input device driver info. Optional
		paNoDevice, // ID of output device
		0, // Num channels. Not used because we haven't given an output device
		0, // Sample format. Also not used
		NULL,
		16000, // Sample rate
		8092, // framesPerBuffer. Not sure
		1, // The number of buffers. Also, not sure :D
		paNoFlag, // No special flags for the stream
		&DefaultEars::portAudioCallbackWrapper,
		this // Supply the current instance as userdata to the callback wrapper
	);
	
	if (result != paNoError)
	{
		throw EarsException("error opening portaudio stream");
	}
	
	result = Pa_StartStream(this->stream);
	
	if (result != paNoError)
	{
		throw EarsException("error starting portaudio stream");
	}
}

int DefaultEars::onPortAudioCallback(void* inputBuffer, void* outputBuffer,
					 unsigned long framesPerBuffer,
					 PaTimestamp outTime)
{
	if (ps_start_utt(ps, "default_ears") < 0)
	{
		//throw EarsException("error starting utterance");
		std::cout << "error starting utterance" << std::endl;
		return 1;
	}	

	if (ps_process_raw(ps, (short*) inputBuffer, framesPerBuffer, 0, 1) < 0)
	{
		//throw EarsException("error decoding raw data");
		std::cout << "error decoding raw data" << std::endl;
		return 1;
	}

	if (ps_end_utt(ps) < 0)
	{
		//throw EarsException("error ending utterance");
		std::cout << "error ending utterance" << std::endl;
		return 1;
	}

	// The word pocketsphinx decoded from the audio sample
	char const* hyp;
	
	// Not sure what the score is meant to indicate
	int score;

	hyp = ps_get_hyp(ps, &score, NULL);

	if (hyp == NULL || strlen(hyp) == 0)
	{
		// pocketsphinx was unable to make up
		// anything from the sample		
	}	
	
	std::cout << "TODO: The default ears heard something: " << hyp << std::endl;

	// Return 0 to keep on going. Any non-zero value will stop the stream	
	return 0;
}
