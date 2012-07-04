#ifndef __JACKAUDIO_H__
#define __JACKAUDIO_H__

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

class jackAudio {
	public:
	jackAudio(jack_ringbuffer_t*);
	~jackAudio();
	jack_client_t* client;

	int activate();
	int close();

	private:
	jack_port_t* midiIn;
	jack_port_t* toLP_midiOut;
	jack_port_t* general_midiOut;
	jack_nframes_t sample_rate;
	jack_ringbuffer_t* rb;

	static int _process(jack_nframes_t nframes, void* arg);
	int process(jack_nframes_t nframes);

};
	

#endif
