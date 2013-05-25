#include "default_ears.h"

#include <iostream>

// TODO: Probably don't want to hardcode these values
#define ACOUSTIC_MODEL "hmm/en_US/hub4wsj_sc_8k"
#define DICT "9133.dic"
#define LANG_MODEL "7859.lm"

#define BUF_SIZE 1024 << 6

DefaultEars::DefaultEars() throw(EarsException)
{
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
	
	this->ps = ps_init(config);

	if (this->ps == NULL)
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

int DefaultEars::onPortAudioCallbackWrapper(const void* input, void* output,
					  unsigned long frameCount, 
					  const PaStreamCallbackTimeInfo* timeInfo,
					  PaStreamCallbackFlags statusFlags, void* userData)
{
	std::cout << "onPortAudioCallbackWrapper(): " << statusFlags << ", userData: " << userData << " frames: " << frameCount << std::endl;	

	DefaultEars* ears = static_cast<DefaultEars*>(userData);
	return ears->onPortAudioCallback(input, output, frameCount, timeInfo, statusFlags);
}

void DefaultEars::initializePortAudio() throw(EarsException)
{
	Pa_Initialize();

	// TODO: Check for a device that can be abused. If any at all

	/*
	int numdevices = Pa_GetDeviceCount();
	
	for (int i = 0; i < numdevices; i++)
	{
		const PaDeviceInfo* device = Pa_GetDeviceInfo(i);
		
		std::cout << "device #" << i << ": " << device->name << std::endl;
	}

	const PaDeviceInfo* info = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
	const PaDeviceInfo* info1 = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());

	std::cout << "default output device: " << info->name << std::endl;
	std::cout << "default input device: " << info1->name << std::endl;

	if (numdevices == 0)
		std::cout << "no input or output devices.." << std::endl;
	*/

	PaDeviceIndex defaultInputDeviceIndex = Pa_GetDefaultInputDevice();
	const PaDeviceInfo* defaultInputDevice = Pa_GetDeviceInfo(defaultInputDeviceIndex);

	if (defaultInputDeviceIndex < 0)
	{
		std::cout << "no default device input.." << std::endl;
	}
	
	std::cout << "using " << defaultInputDevice->name << std::endl;

	PaStreamParameters inputStreamParams;
	inputStreamParams.device = defaultInputDeviceIndex;
	inputStreamParams.channelCount = defaultInputDevice->maxInputChannels;
	inputStreamParams.sampleFormat = paInt16;
	inputStreamParams.hostApiSpecificStreamInfo = NULL;
	
	PaError result = Pa_OpenStream(
		&this->stream,
		&inputStreamParams,
		NULL, // Output stream params. Null because this is input-only stream
		8092, // Sample rate. Not sure what the value means
		0, // Frames per buffer. 0 to let port audio do its thing
		paNoFlag, // Optional stream flags
		&DefaultEars::onPortAudioCallbackWrapper, // Function that's called by port audio whenever input is available
		this // User data that is passed to the callback function
	);
	
	if (result != paNoError)
	{
		std::string message;
		message.append("error opening stream: ");
		message.append(Pa_GetErrorText(result));

		throw EarsException(message.c_str());
	}
	
	result = Pa_StartStream(this->stream);
	
	if (result != paNoError)
	{
		std::string message;
		message.append("error starting stream: ");
		message.append(Pa_GetErrorText(result));

		throw EarsException(message.c_str());
	}
}

int DefaultEars::onPortAudioCallback(const void* input, void* output,
				      unsigned long frameCount, 
				      const PaStreamCallbackTimeInfo* timeInfo,
			 	      PaStreamCallbackFlags statusFlags)
{
	short* audioData = (short*) input;

	if (audioData == NULL)
	{
		std::cout << "no audio data" << std::endl;
		return 0;
	}

	if (ps_start_utt(this->ps, "default_ears") < 0)
	{
		//throw EarsException("error starting utterance");
		std::cout << "error starting utterance" << std::endl;
		return 1;
	}	

	int searchedFrames = ps_process_raw(this->ps, audioData, frameCount, 0, 1);

	if (searchedFrames < 0)
	{
		//throw EarsException("error decoding raw data");
		std::cout << "error decoding raw data" << std::endl;
		return 1;
	}

	if (ps_end_utt(this->ps) < 0)
	{
		//throw EarsException("error ending utterance");
		std::cout << "error ending utterance" << std::endl;
		return 1;
	}

	// The word pocketsphinx decoded from the audio sample
	char const* hyp;
	
	// Not sure what the score is meant to indicate
	int score;

	hyp = ps_get_hyp(this->ps, &score, NULL);

	if (hyp == NULL || strlen(hyp) == 0)
	{
		// pocketsphinx was unable to make up
		// anything from the sample		

	}	
	
	std::cout << "searched " << searchedFrames << " frames" << std::endl;
	std::cout << "TODO: The default ears heard something: " << (hyp == NULL ? "" : hyp) << " (SCORE: " << score << ")" << std::endl;

	// Return 0 to keep on going. Any non-zero value will stop the stream	
	return 0;
}
