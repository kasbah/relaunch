#include "jackAudio.h"

jackAudio::jackAudio()
{
  client = jack_client_open (
	                             "JackClientTutorial",
	                             JackNullOption,
                               0,
	                             0 
	                                                   );

	sample_rate = jack_get_sample_rate(client);
	jack_set_process_callback  (client, process , this);

	fromLP_midiIn = jack_port_register   (
	                                        client,
	                                        "from LP in",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsInput,
	                                        0
	                                                                 );
	toLP_midiOut = jack_port_register    (
	                                        client,
	                                        "to LP out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	general_midiOut = jack_port_register (
	                                        client,
	                                        "general out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	jack_activate(client);
}

jackAudio::~jackAudio()
{
	jack_deactivate(client);
	jack_client_close(client);
}

int jackAudio::_process(jack_nframes_t nframes, void* arg) 
{
	return static_cast<jackAudio*> (arg)->process(nframes);
}

int jackAudio::process(jack_nframes_t nframes)
{
	void * fromLP_midiInPortBuf = jack_port_get_buffer (fromLP_midiIn, nframes);
	void * toLP_midiOutPortBuf = jack_port_get_buffer (toLP_midiOut, nframes);
	void * general_midiOutPortBuf = jack_port_get_buffer (general_midiOut, nframes);
}
