#ifndef __JACKAUDIO_H__
#define __JACKAUDIO_H__

#include <jack/jack.h>
#include <jack/midiport.h>

#include "MidiDriver.h"
#include "Engine.h"

class JackMidi 
	: public MidiDriver
{
	public:
		JackMidi(Engine* e);
		virtual ~JackMidi();
		virtual int exit();
	private:
		jack_client_t* client;
		jack_port_t* midiIn;
		jack_port_t* toLP_midiOut;
		jack_port_t* general_midiOut;
		static int _process(jack_nframes_t nframes, void* arg);
		int process(jack_nframes_t nframes);
		int activate();
		int close();
};
	
#endif
