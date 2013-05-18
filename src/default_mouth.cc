#include "default_mouth.h"

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
        espeak_SetSynthCallback(&espeak_synth_callback);
}

DefaultMouth::~DefaultMouth()
{

}

void DefaultMouth::speak(const std::string& text)
{
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
                printf("failed to synthesize word: %s\n", word);
        }
}

void DefaultMouth::eSpeakCallbackWrapper(short* wav, int numsamples, espeak_EVENT* events)
{
	// Are there samples to play?
        if (numsamples == 0)
                return 0;       
        
	M

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
