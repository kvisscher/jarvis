#include "default_ears.h"

#include <iostream>

// TODO: Probably don't want to hardcode these values
#define ACOUSTIC_MODEL "hmm/en_US/hub4wsj_sc_8k"
#define DICT "9133.dic"
#define LANG_MODEL "7859.lm"

#define BUF_SIZE 1024 << 6

DefaultEars::DefaultEars()
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

void DefaultEars::initializePortAudio()
{
	Pa_Initialize();
	
	PaStreamParameters params;
	params.channelCount = 1;
	params.device = Pa_GetDefaultInputDevice();
	params.sampleFormat = PaInt16;
	
	PaError result = Pa_OpenStream(
		&this->stream, // Pointer to the stream
		&params, // Parameters for the input
		NULL,  // Parameters for the output. This is an input only stream
		16000, // Sample rate
		0, // Number of frames that are to be received in the callback. Using 0 because the ears don't care
		PaNoFlag, // No special flags
		&this->onPortAudioCallback, // Callback function
		NULL // User data. Not needed because everything is in the class		
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

int DefaultEars::int onPortAudioCallback(const void* input, void* output,
				unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags, void *userData)
{
	if (ps_start_utt(ps, "default_ears") < 0)
	{
		throw EarsException("error starting utterance");
	}	

	if (ps_process_raw(ps, (short*) input, frameCount, 0, 1) < 0)
	{
		throw EarsException("error decoding raw data");
	}

	if (ps_end_utt(ps) < 0)
	{
		throw EarsException("error ending utterance");
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
		return paContinue;
	}	
	
	std::cout << "TODO: The default ears heard something: " << hyp << std::endl;
	
	return paContinue;
}
