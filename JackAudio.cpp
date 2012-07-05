#include "JackAudio.h"
#include <iostream>
using namespace std;

int JackAudio::_process(jack_nframes_t nframes, void* arg) 
{
	return static_cast<JackAudio*> (arg)->process(nframes);
}

int JackAudio::process(jack_nframes_t nframes)
{
	void * midiInPortBuf = jack_port_get_buffer (midiIn, nframes);
	void * toLP_midiOutPortBuf = jack_port_get_buffer (toLP_midiOut, nframes);
	void * general_midiOutPortBuf = jack_port_get_buffer (general_midiOut, nframes);


	jack_midi_clear_buffer(toLP_midiOutPortBuf);	
	jack_midi_clear_buffer(general_midiOutPortBuf);	
	jack_midi_event_t* midi_event;

	uint32_t num_of_in_events = jack_midi_get_event_count(midiInPortBuf);
	if (num_of_in_events != 0) 
	{
	
		for (int i = 0; i < num_of_in_events; ++i)
		{
			jack_midi_event_t * midi_event = new jack_midi_event_t;
			if(jack_midi_event_get(midi_event, midiInPortBuf, i) == 0)
			{
				engine->queue_event(midi_event);
				//jack_ringbuffer_write(midi_in_rb, (*midi_event).buffer, MIDI_DATA_SIZE); // need -fpermissive to cast buffer to (const char*)
			}
		}
	}
	jack_midi_data_t * out_data = new jack_midi_data_t;
	if( engine->read_data(out_data) == 0)
	{
		cout << "sending midi" << endl;
		jack_midi_event_write(toLP_midiOutPortBuf, 0, out_data, 3);
	}

	//cout << jack_ringbuffer_read_space(seq_rb) << endl;
//	if(jack_ringbuffer_read_space(seq_rb) >= 64)
//	{
//		uint8_t seq[8] = {0,0,0,0,0,0,0,0};
//		//jack_ringbuffer_read(seq_rb, seq, 64);	
//		for(int i = 0; i < 8; ++i)
//		{
//			//cout << "in process seq: " << i << " is " << (unsigned int)seq[i] << endl;
//		}
//	}
//
	return 0;
}

JackAudio::JackAudio(Engine* e)
	: engine(e)
{
  client = jack_client_open (
	                             "launchSequencer",
	                             JackNullOption,
                               0,
	                             0 
	                                                   );
	if (client == NULL)
	{
		cerr << "ERROR: jack client open failed" << endl;
		return;
	}

	sample_rate = jack_get_sample_rate(client);
	jack_set_process_callback  (client, _process , this);

	midiIn =          jack_port_register (
	                                        client,
	                                        "midi_in",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsInput,
	                                        0
	                                                                 );
	toLP_midiOut =    jack_port_register (
	                                        client,
	                                        "to_LP_midi_out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	general_midiOut = jack_port_register (
	                                        client,
	                                        "general_midi_out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	//jack_ringbuffer_free(midi_in_rb);
}


int JackAudio::activate()
{
	if (jack_activate(client) != 0)
	{
		cerr << "ERROR: could not activate jack client" << endl;	
		return 1;
	}
	return 0;
}

int JackAudio::close()
{
	cerr << "deactivating and closing jack client" << endl;
	if((jack_deactivate(client) != 0) || (jack_client_close(client) != 0))
	{
		cerr << "ERROR: could not deactivate and/or close jack client" << endl;
		return 1;
	}
	return 0;
}


JackAudio::~JackAudio()
{
	cerr << "JackAudio destructor called" << endl;
	if (client != NULL)
		close();
	else
		cerr << "ERROR: JackAudio client does not exist and cannot be closed" << endl;
}
