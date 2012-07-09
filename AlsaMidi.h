#ifndef __ALSAMIDI_H__
#define __ALSAMIDI_H__

#include <alsa/asoundlib.h>
#include <pthread.h>

#include "MidiDriver.h"
#include "Engine.h"

class AlsaMidi 
	: public MidiDriver
{
	public:
		AlsaMidi(Engine* e);
		~AlsaMidi();
	private:
		snd_seq_t * seq_handle;
		pthread_t processthread;
		int midiPort;
		int toLP_midiOut;	
		static void* _process(void* arg);
		void process();
};
	

#endif// __ALSAMIDI_H__
