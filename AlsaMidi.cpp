#include "AlsaMidi.h"
#include <iostream>
using namespace std;

AlsaMidi::AlsaMidi(Engine* e)
	: MidiDriver(e)
{
	snd_seq_open(&seq_handle, "hw", SND_SEQ_OPEN_INPUT, 0);
	snd_seq_set_client_event_filter(seq_handle, SND_SEQ_EVENT_NOTEON);// | SND_SEQ_EVENT_NOTEOFF);// | SND_SEQ_EVENT_CONTROLLER | SND_SEQ_EVENT_TICK | SND_SEQ_EVENT_START | SND_SEQ_EVENT_STOP);
		//cerr << "ERROR: Could not open Alsa midi client" << endl;
	snd_seq_set_client_name(seq_handle, "ReLaunch");
	midiPort = snd_seq_create_simple_port(seq_handle, "General",  SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE , SND_SEQ_PORT_TYPE_APPLICATION); 
	//midiPort = snd_seq_create_simple_port(seq_handle, "General", SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE | SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_APPLICATION); 
	//toLP_midiOut = snd_seq_create_simple_port(seq_handle, "LaunchPad LED control", SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION); 


	pthread_create(&processthread, NULL, _process, this);
}

AlsaMidi::~AlsaMidi()
{
	cout << "AlsaMidi destructor called" << endl;
	//pthread_join(processthread, NULL);
}

void * AlsaMidi::_process(void* arg)
{
	static_cast<AlsaMidi*>(arg)->process();
}

void AlsaMidi::process()
{
	cout << "AlsaMidi process called" << endl;
	snd_seq_event_t* event;
	while (1) 
	{
		cout <<  snd_seq_event_input(seq_handle, &event)  << endl;
		//while (snd_seq_event_input(seq_handle, &event) >= 0)
		//{
		//	cout << "got event" << endl;
		//	snd_seq_ev_clear(&event);
		//	//snd_seq_drain_output(seq_handle);
		//}
	}
}

