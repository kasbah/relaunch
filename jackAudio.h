#ifndef __JACKAUDIO_H__
#define __JACKAUDIO_H__
#include "launchSequencer.h"

class jackAudio {
	public:
	jackAudio(jack_ringbuffer_t*, jack_ringbuffer_t*);
	~jackAudio();
	jack_client_t* client;

	int activate();
	int close();

	jack_ringbuffer_t* midi_in_rb;
	jack_ringbuffer_t* seq_rb;
	private:
	jack_port_t* midiIn;
	jack_port_t* toLP_midiOut;
	jack_port_t* general_midiOut;
	jack_nframes_t sample_rate;

	static int _process(jack_nframes_t nframes, void* arg);
	int process(jack_nframes_t nframes);

};
	

#endif
