#include "robo_assistant.h"

#include "default_mouth.h"
#include "default_ears.h"

int main()
{
	DefaultEars ears;
	DefaultMouth mouth;

	Brain brain(&ears, &mouth);
	
	RoboAssistant jarvis(&brain);
}

/*
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <pocketsphinx.h>
#include "speak_lib.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 1024 << 6

#define APPLICATION_NAME "Jarvis"

#define JARVIS_ACOUSTIC_MODEL "hmm/en_US/hub4wsj_sc_8k"
#define JARVIS_DICT "9133.dic"
#define JARVIS_LANG_MODEL "7859.lm"

// Time in seconds that the user has to give jarvis a command
// after waking him up
#define JARVIS_TIME_TO_OBEY 5

pa_simple* playback = NULL;

// Time when the command mode was last activated
time_t command_mode_activated = 0;

int espeak_synth_callback(short* wav, int numsamples, espeak_EVENT* events)
{
	printf("event type: %d, num_samples: %d\n", events->type, numsamples);

	// Are there samples to play?
	if (numsamples == 0)
		return 0;	
	
	pa_simple* playback = (pa_simple*) events->user_data;

	int err = 0;

	// numsamples * 2 because pa_simple_write expects bytes and not shorts
	if (pa_simple_write(playback, wav, numsamples * 2, &err) < 0)
	{
		printf("error playing espeak sample: %s\n", pa_strerror(err));
		return 1;
	}

	return 0;
}

void say(char const* word)
{
	espeak_Synth(
		word,
		strlen(word) + 1,
	 	0,
	 	POS_CHARACTER,
	 	0,
	 	espeakCHARS_AUTO, 
	 	NULL, 
	 	(void*) playback
	);
	
	if (espeak_Synchronize() < 0)
	{
		printf("failed to synthesize word: %s\n", word);
	}
}

void handle_word(char const* word)
{
	printf("handle_word() %s", word);

	if (command_mode_activated > 0)
	{
		time_t now = time(NULL);
		time_t diff = now - command_mode_activated;
		
		if (diff / 1000 < JARVIS_TIME_TO_OBEY)
		{
			if (strcmp(word, "TIME") == 0)
			{
				time_t t = time(NULL);
				tm* now = localtime(&t);
		
				char buf[128];
		
				strftime(buf, sizeof(buf), "Todays date is %A %e %B %G", now);
		
				say(buf);
			}
		}
		
		command_mode_activated = 0;
	}
	else
	{
		if (strcmp(word, "JARVIS") == 0)
		{
			command_mode_activated = time(NULL);
	
			say("Yes sir? What would you have me do?");
		}
		else if (strcmp(word, "HELLO") == 0)
		{
			say("Hello");
		}
		else
		{
			printf("Didn't recgonize command: %s\n", word);
		}
	}
}

int main()
{
	int sample_rate = espeak_Initialize(
		AUDIO_OUTPUT_RETRIEVAL,
		0, // Buffer length
		NULL, // espeak data path, null for default
		0 // options?
	);

	pa_sample_spec playback_spec;
	
	playback_spec.format = PA_SAMPLE_S16NE;
	playback_spec.channels = 2;
	playback_spec.rate = 8000;
	
	playback = pa_simple_new(
			  NULL, 		// Use the default server.
		          APPLICATION_NAME,     // Our application's name.
		          PA_STREAM_PLAYBACK,   // Use this server to play sound
		          NULL,                 // Use the default device.
		          "Jarvis mouth",       // Description of our stream.
		          &playback_spec,       // Our sample format.
		          NULL,                 // Use default channel map
		          NULL,                 // Use default buffering attributes.
		          NULL                  // Ignore error code.
			);

	// Let espeak use the english voice
	espeak_SetVoiceByName("english-us");
	
	// Callback handler for synth
	espeak_SetSynthCallback(&espeak_synth_callback);

	printf("espeak sample rate: %d\n", sample_rate);

	pa_simple *s = NULL;
	pa_sample_spec ss;

	ss.format = PA_SAMPLE_S16NE;
	ss.channels = 1;
	ss.rate = 16000;

	s = pa_simple_new(
			NULL,               // Use the default server.
		        APPLICATION_NAME,   // Our application's name.
		        PA_STREAM_RECORD,   // Use the server to record audio samples
		        NULL,               // Use the default device.
		        "Jarvis ears",      // Description of our stream.
		        &ss,                // Our sample format.
		        NULL,               // Use default channel map
		        NULL,               // Use default buffering attributes.
		        NULL               // Ignore error code.
		);

	ps_decoder_t* ps;
	cmd_ln_t* config;
	
	// Load pocketsphinx config
	config = cmd_ln_init(
			NULL,
			ps_args(),
			TRUE,
	                "-hmm", JARVIS_ACOUSTIC_MODEL,
	                "-lm", JARVIS_LANG_MODEL,
	                "-dict", JARVIS_DICT,
	                NULL
		);

	if (config == NULL)
	{
		printf("error while initializing pocketsphinx config");
		return 1;
	}
	
	ps = ps_init(config);

	if (ps == NULL)
	{
		printf("error initializing pocketsphinx decoder");
		return 1;	
	}

	for (;;)
	{
		char buf[BUF_SIZE];
		int error;	

		if (pa_simple_read(s, buf, BUF_SIZE, &error) < 0)
		{
			printf("error while reading input: %s\n", pa_strerror(error));
			return 1;
		}
			
		if (ps_start_utt(ps, "test") < 0)
		{
			printf("error starting utterance");
			return 1;
		}	
	
		// Dividing the buffer size by 2 because of the cast to short
		if (ps_process_raw(ps, (short*) buf, BUF_SIZE / 2, 0, 1) < 0)
		{
			printf("error decoding raw data");
			return 1;
		}

		if (ps_end_utt(ps) < 0)
		{
			printf("error ending utterance");
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
			
			continue;
		}

		printf("SCORE: %d", score);

		// TODO: Listen for more words?
		handle_word(hyp);	
	}

	if (ps)
	{
		ps_free(ps);
	}

	if (s)
	{
		pa_simple_free(s);
	}
	
	if (playback)
	{
		pa_simple_free(playback);
	}
	
	return 0;        
}
*/
