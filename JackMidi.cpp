#include "JackMidi.h"
//#include "LaunchPad.h"
#include <iostream>
using namespace std;

JackMidi::JackMidi(Engine* e)
	: MidiDriver(e)
{
  client = jack_client_open (
	                             "ReLaunch",
	                             JackNullOption,
                               0,
	                             0 
	                                                   );
	if (client == NULL)
	{
		cerr << "ERROR: jack client open failed" << endl;
	//	return;
	}

	//sample_rate = jack_get_sample_rate(client);
	jack_set_process_callback  (client, _process , this);

	midiIn =          jack_port_register (
	                                        client,
	                                        "General in",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsInput,
	                                        0
	                                                                 );
	general_midiOut = jack_port_register (
	                                        client,
	                                        "General out",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	toLP_midiOut =    jack_port_register (
	                                        client,
	                                        "LaunchPad LED control",
	                                        JACK_DEFAULT_MIDI_TYPE,
	                                        JackPortIsOutput,
	                                        0
	                                                                 );
	activate();
}

JackMidi::~JackMidi()
{
	cerr << "JackMidi destructor called" << endl;
	close();
}

void JackMidi::close()
{
	cerr << "deactivating and closing jack client" << endl;
	if((jack_deactivate(client) != 0) || (jack_client_close(client) != 0))
	{
		cerr << "ERROR: could not deactivate and/or close jack client" << endl;
	}
}

//our arg is this instance, this is a workaround so we can
//have our jack process as part of a class
int JackMidi::_process(jack_nframes_t nframes, void* arg) 
{
	return static_cast<JackMidi*> (arg)->process(nframes);
}

int JackMidi::process(jack_nframes_t nframes)
{
	// setup the port buffers and clear the output ones
	void * midiInPortBuf = jack_port_get_buffer (midiIn, nframes);
	void * toLP_midiOutPortBuf = jack_port_get_buffer (toLP_midiOut, nframes);
	void * general_midiOutPortBuf = jack_port_get_buffer (general_midiOut, nframes);

	jack_midi_clear_buffer(toLP_midiOutPortBuf);	
	jack_midi_clear_buffer(general_midiOutPortBuf);	

	//queue in-events into the ringbuffer
	uint32_t num_of_in_events = jack_midi_get_event_count(midiInPortBuf);
	for (int i = 0; i < num_of_in_events; ++i)
	{
		jack_midi_event_t * midi_event = new jack_midi_event_t;
		if(jack_midi_event_get(midi_event, midiInPortBuf, i) == 0)
		{
			engine->queue_event(midi_event->buffer);
		}
	}

	//write any out-events to the appropriate port
	jack_midi_data_t out_data[3];// = new jack_midi_data_t;
	int i = 0;
	while ( engine->read_data_for_general(out_data) == 0)
	{
		jack_midi_event_write(general_midiOutPortBuf, i++, out_data, MIDI_DATA_SIZE);
	}
	i = 0;
	while ( engine->read_data_for_LP(out_data) == 0)
	{
		jack_midi_event_write(toLP_midiOutPortBuf, i++, out_data, MIDI_DATA_SIZE);
	}

	return 0;
}

int JackMidi::activate()
{
	if (jack_activate(client) != 0)
	{
		cerr << "ERROR: could not activate jack client" << endl;	
		return 1;
	}
	return 0;
}

