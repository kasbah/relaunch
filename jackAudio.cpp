#include "jackAudio.h"
#include <iostream>
#include <list>
#include <cstring>
using namespace std;

jackAudio::jackAudio(jack_ringbuffer_t* ringbuffer)
	:rb(ringbuffer)
{
  client = jack_client_open (
	                             "launchSequencer",
	                             JackNullOption,
                               0,
	                             0 
	                                                   );
	if (client == NULL)
		cerr << "ERROR: jack client open failed" << endl;

	sample_rate = jack_get_sample_rate(client);
	jack_set_process_callback  (client, _process , this);

	fromLP_midiIn = jack_port_register   (
	                                        client,
	                                        "from_LP_out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsInput,
	                                        0
	                                                                 );
	toLP_midiOut = jack_port_register    (
	                                        client,
	                                        "to_LP_out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	general_midiOut = jack_port_register (
	                                        client,
	                                        "general_out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	jack_ringbuffer_free(rb);
}

int jackAudio::activate()
{
	if (jack_activate(client) != 0)
	{
		cerr << "ERROR: could not activate jack client" << endl;	
		return 1;
	}
	return 0;
}

int jackAudio::close()
{
	cerr << "deactivating and closing jack client" << endl;
	if((jack_deactivate(client) != 0) || (jack_client_close(client) != 0))
	{
		cerr << "ERROR: could not deactivate and/or close jack client" << endl;
		return 1;
	}
	return 0;
}


jackAudio::~jackAudio()
{
	cerr << "jackAudio destructor called" << endl;
	close();
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

	list<jack_midi_event_t*> midi_event_list;

	//jack_midi_clear_buffer(toLP_midiOutPortBuf);	
	//jack_midi_clear_buffer(general_midiOutPortBuf);	
	jack_midi_event_t* midi_event;

	uint32_t num_of_in_events = jack_midi_get_event_count(fromLP_midiInPortBuf);
	if (num_of_in_events != 0) 
	{
	
		for (int i = 0; i < num_of_in_events; ++i)
		{
			jack_midi_event_t * midi_event;
			midi_event_list.push_back(midi_event);
			if(jack_midi_event_get(midi_event, fromLP_midiInPortBuf, i) == 0)
			{
				//cout << "size: " << sizeof(jack_midi_event_t) << endl;
				cout << "in note on: " << ((*midi_event).buffer[0] == 0x80) << endl;
				char buffer[sizeof(jack_midi_event_t)];
				memcpy(buffer, midi_event, sizeof(jack_midi_event_t));
				jack_ringbuffer_write(rb, buffer, sizeof(jack_midi_event_t));
			}
		}
	}
	return 0;
}
