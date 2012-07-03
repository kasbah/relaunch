#ifndef __JACKAUDIO_H__
#define __JACKAUDIO_H__

#include <jack/jack.h>
#include <jack/midiport.h>


class jackAudio {
	public:
	jackAudio();
	~jackAudio();

	jack_client_t* client;

	jack_port_t* fromLP_midiIn;
	jack_port_t* toLP_midiOut;
	jack_port_t* general_midiOut;

	jack_nframes_t sample_rate;

	static int _process(jack_nframes_t nframes, void* arg);
	int process(jack_nframes_t nframes);

};

#endif
