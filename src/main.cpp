#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

#include <pocketsphinx.h>

#include "speak_lib.h"

#include <stdio.h>

#include <string.h>

#include <time.h>

#define BUF_SIZE 1024 << 6

pa_simple* playback = NULL;

int espeak_synth_callback(short* wav, int numsamples, espeak_EVENT* events)
{
	printf("event type: %d, num_samples: %d\n", events->type, numsamples);

	// Is there a sample to play?
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
	espeak_Synth(word,
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
	if (strcmp(word, "JARVIS") == 0)
	{
		say("Yes sir? What would you have me do?");
	}
	else if (strcmp(word, "TIME") == 0)
	{
		time_t t = time(NULL);
		tm* now = localtime(&t);
		
		char buf[128];
		
		strftime(buf, sizeof(buf), "Todays date is %A %e %B %G", now);
		
		say(buf);
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
	
	playback = pa_simple_new(NULL,               // Use the default server.
		          "Fooap22p",           // Our application's name.
		          PA_STREAM_PLAYBACK,
		          NULL,               // Use the default device.
		          "Playback",            // Description of our stream.
		          &playback_spec,                // Our sample format.
		          NULL,               // Use default channel map
		          NULL,               // Use default buffering attributes.
		          NULL               // Ignore error code.
		         );

	espeak_SetVoiceByName("english-us");

	espeak_SetSynthCallback(&espeak_synth_callback);

	espeak_VOICE* voice = espeak_GetCurrentVoice();

	printf("current voice: %s\n", voice->name);

	printf("espeak sample rate: %d\n", sample_rate);

	pa_simple *s = NULL;
	pa_sample_spec ss;

	ss.format = PA_SAMPLE_S16NE;
	ss.channels = 1;
	ss.rate = 16000;

	s = pa_simple_new(NULL,               // Use the default server.
		          "Fooapp",           // Our application's name.
		          PA_STREAM_RECORD,
		          NULL,               // Use the default device.
		          "Music",            // Description of our stream.
		          &ss,                // Our sample format.
		          NULL,               // Use default channel map
		          NULL,               // Use default buffering attributes.
		          NULL               // Ignore error code.
		         );

	ps_decoder_t* ps;
	cmd_ln_t* config;

	config = cmd_ln_init(NULL, ps_args(), TRUE,
	                     "-hmm", "hmm/en_US/hub4wsj_sc_8k",
	                     "-lm", "7859.lm",
	                     "-dict", "9133.dic",
	                     NULL);

	if (config == NULL)
		return 1;

	ps = ps_init(config);

	if (ps == NULL)
	{
		printf("error initializing ps");
		return 1;	
	}

	char const* uttid = "utt_test";

	for (;;)
	{
		char buf[BUF_SIZE];
		int error;	

		if (pa_simple_read(s, buf, BUF_SIZE, &error) < 0)
		{
			printf("error while reading: %s\n", pa_strerror(error));
			return 1;
		}
			
		printf("read sample\n");
	

		if (ps_start_utt(ps, "test") < 0)
		{
			printf("error starting utterance");
			return 1;
		}	
	
		int rv = ps_process_raw(ps, (short*) buf, BUF_SIZE / 2, 0, 0);
		
		if (rv < 0)
		{
			printf("error decoding raw data: %d", rv);
			return 1;
		}

		if (ps_end_utt(ps))
		{
			printf("error ending utterance");
			return 1;
		}

		char const* hyp;
		int score;
	
		hyp = ps_get_hyp(ps, &score, &uttid);

		if (hyp == NULL || strlen(hyp) == 0)
		{
			continue;
		}

		printf("SCORE: %d", score);

		// TODO: Listen for more words?

		handle_word(hyp);	
	}

	if (s)
		pa_simple_free(s);
	
	if (playback)
		pa_simple_free(playback);

	return 0;        
}
