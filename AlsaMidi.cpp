#include "AlsaMidi.h"
#include <iostream>
#include <unistd.h>
#include "Midi.h"
using namespace std;

AlsaMidi::AlsaMidi(Engine* e)
	: MidiDriver(e)
{
	snd_seq_open
		(
			  &seq_handle
			, "default"
			, SND_SEQ_OPEN_DUPLEX 
			, SND_SEQ_NONBLOCK
		);

	snd_seq_set_client_name(seq_handle, "ReLaunch");
	client_id = snd_seq_client_id(seq_handle);

	midiPort = snd_seq_create_simple_port
		(
			  seq_handle
			, "General"
			, SND_SEQ_PORT_CAP_WRITE 
			| SND_SEQ_PORT_CAP_SUBS_WRITE 
			| SND_SEQ_PORT_CAP_READ
			| SND_SEQ_PORT_CAP_SUBS_READ 
			, SND_SEQ_PORT_TYPE_APPLICATION
		); 

	toLP_midiOut = snd_seq_create_simple_port
		(
			  seq_handle
			, "LaunchPad LED control"
			, SND_SEQ_PORT_CAP_READ 
			| SND_SEQ_PORT_CAP_SUBS_READ
			, SND_SEQ_PORT_TYPE_APPLICATION
		); 

	connect_to_System();
	is_connected_to_LP = connect_to_LP();

	pthread_create(&process_thread, NULL, _process, this);
}

AlsaMidi::~AlsaMidi()
{
	cout << "AlsaMidi destructor called" << endl;
	close();
}

bool AlsaMidi::connect_to_System()
{
	snd_seq_addr_t our_address;
	our_address.client = client_id;
	our_address.port = midiPort;

	snd_seq_addr_t sys_address;
	sys_address.client = 0;
	sys_address.port = 1; //System Announce Port

	snd_seq_port_subscribe_t *subs;
	snd_seq_port_subscribe_alloca(&subs);
	snd_seq_port_subscribe_set_sender(subs, &sys_address);
	snd_seq_port_subscribe_set_dest(subs, &our_address);
	snd_seq_subscribe_port(seq_handle, subs);
}
bool AlsaMidi::disconnect_from_System()
{
	snd_seq_addr_t our_address;
	our_address.client = client_id;
	our_address.port = midiPort;

	snd_seq_addr_t sys_address;
	sys_address.client = 0;
	sys_address.port = 1; //System Announce Port

	snd_seq_port_subscribe_t *subs;
	snd_seq_port_subscribe_alloca(&subs);
	snd_seq_port_subscribe_set_sender(subs, &sys_address);
	snd_seq_port_subscribe_set_dest(subs, &our_address);
	snd_seq_unsubscribe_port(seq_handle, subs);
}

bool AlsaMidi::connect_to_LP ()
{
	subscriptions.clear();
	bool found = false;
	snd_seq_client_info_t * info;
	snd_seq_client_info_malloc(&info);
	snd_seq_get_any_client_info(seq_handle, 0, info);
	snd_seq_addr_t LP_address;
	do {
		string name = snd_seq_client_info_get_name(info);
		if (name == "Launchpad")
		{
			LP_address.client = snd_seq_client_info_get_client(info);
			LP_id = LP_address.client;
			LP_address.port = 0;
			found = true;
			break;
		}

	} while (snd_seq_query_next_client(seq_handle, info) == 0);

	snd_seq_client_info_free(info);

	if (found) {
			snd_seq_addr_t our_address;
			our_address.client = client_id;
			our_address.port = midiPort;

			snd_seq_port_subscribe_t *subs;
			snd_seq_port_subscribe_alloca(&subs);
			snd_seq_port_subscribe_set_sender(subs, &LP_address);
			snd_seq_port_subscribe_set_dest(subs, &our_address);
			snd_seq_subscribe_port(seq_handle, subs);

			subscriptions.push_back (SubsPair(LP_address, our_address));

			our_address.port = toLP_midiOut;

			snd_seq_port_subscribe_t *subs2;
			snd_seq_port_subscribe_alloca(&subs2);
			snd_seq_port_subscribe_set_sender(subs2, &our_address);
			snd_seq_port_subscribe_set_dest(subs2, &LP_address);
			snd_seq_subscribe_port(seq_handle, subs2);

			subscriptions.push_back (SubsPair(our_address, LP_address));

			//subscriptions.push_back(subs2);
			//snd_seq_port_subscribe_free(subs);
	}
	else 
		LP_id = -1;
	return found;
}

void AlsaMidi::close() 
{
	disconnect_from_System();
	list<SubsPair>::iterator it;
	for (it = subscriptions.begin(); it != subscriptions.end(); ++it)
	{
		snd_seq_port_subscribe_t *subs;
		snd_seq_port_subscribe_alloca(&subs);
		snd_seq_port_subscribe_set_sender(subs, &(it->first));
		snd_seq_port_subscribe_set_dest(subs, &(it->second));
		snd_seq_unsubscribe_port(seq_handle, subs);
	}
	cout << "AlsaMidi close called" << endl;
	pthread_cancel(process_thread);
	pthread_join(process_thread, NULL);
	snd_seq_close(seq_handle);
}

void * AlsaMidi::_process(void* arg)
{
	static_cast<AlsaMidi*>(arg)->process();
}

void AlsaMidi::process()
{
	cout << "AlsaMidi process called" << endl;
	while (1) 
	{
		snd_seq_event_t* event;
		while (snd_seq_event_input(seq_handle, &event) >= 0)
		{
			uint8_t data[3];
			switch(event->type)
			{
				case SND_SEQ_EVENT_NOTEON:
					data[1] = event->data.raw8.d[1]; 
					data[2] = event->data.raw8.d[2]; 
					data[0] = (data[2] == 127) ? MIDI::NOTE_ON : MIDI::NOTE_OFF;
					//cout << "in alsa to engine:" << endl;
					//cout << (int)data[0] << endl;
					//cout << (int)data[1] << endl;
					//cout << (int)data[2] << endl;
					engine->queue_event(data);
					break;
				case SND_SEQ_EVENT_CLOCK:
					data[0] = MIDI::TICK;
					data[1] = 0;
					data[2] = 0;
					engine->queue_event(data);
					break;
				case SND_SEQ_EVENT_START:
					data[0] = MIDI::START;
					data[1] = 0;
					data[2] = 0;
					engine->queue_event(data);
				case SND_SEQ_EVENT_STOP:
					data[0] = MIDI::STOP;
					data[1] = 0;
					data[2] = 0;
					engine->queue_event(data);
				case SND_SEQ_EVENT_CLIENT_START:
					if (!is_connected_to_LP)
						is_connected_to_LP = connect_to_LP();
					break;
				case SND_SEQ_EVENT_CLIENT_EXIT:
					if (event->data.addr.client == LP_id)
					{
						is_connected_to_LP = false;
					}
					break;
					
			}
		}
		uint8_t out_data[3];
		snd_seq_event_t out_event;
		while ( engine->read_data_for_general(out_data) == 0)
		{
			snd_seq_ev_clear(&out_event);
			//cout << "in alsa for gen:" << endl;
			//cout << (int)out_data[0] << endl;
			//cout << (int)out_data[1] << endl;
			//cout << (int)out_data[2] << endl;
			snd_seq_ev_set_source(&out_event, midiPort);
			snd_seq_ev_set_subs(&out_event);
			snd_seq_ev_set_direct(&out_event);
			if (out_data[0] == MIDI::NOTE_ON)
				out_event.type = SND_SEQ_EVENT_NOTEON;
			else if (out_data[0] == MIDI::NOTE_OFF)
				out_event.type = SND_SEQ_EVENT_NOTEOFF;
			out_event.data.raw8.d[1] = out_data[1];
			out_event.data.raw8.d[2] = out_data[2];
			snd_seq_event_output_direct(seq_handle, &out_event);
			//snd_seq_drain_output(seq_handle);
		}
		while ( engine->read_data_for_LP(out_data) == 0)
		{
			//cout << "in alsa for LP:" << endl;
			//cout << (int)out_data[0] << endl;
			//cout << (int)out_data[1] << endl;
			//cout << (int)out_data[2] << endl;
			snd_seq_ev_clear(&out_event);
			snd_seq_ev_set_source(&out_event, toLP_midiOut);
			snd_seq_ev_set_subs(&out_event);
			snd_seq_ev_set_direct(&out_event);
			if (out_data[0] == MIDI::NOTE_ON)
				out_event.type = SND_SEQ_EVENT_NOTEON;
			else if (out_data[0] == MIDI::CC)
				out_event.type = SND_SEQ_EVENT_CONTROLLER;
			out_event.data.raw8.d[1] = out_data[1];
			out_event.data.raw8.d[2] = out_data[2];
				//out_event.data.note.channel=0;
				//out_event.data.note.note=out_data[1];
				//out_event.data.note.velocity=out_data[2];
				//snd_seq_ev_set_noteon(&out_event, 0, out_data[1], out_data[2]);
			//else if (out_data[0] == MIDI::NOTE_OFF)
			//	snd_seq_ev_set_noteoff(&out_event, 0, out_data[1], out_data[2]);
			//cerr << "error? : " << snd_seq_event_output_direct(seq_handle, &out_event) << endl;
			snd_seq_event_output_direct(seq_handle, &out_event);
			//snd_seq_drain_output(seq_handle);
		}

	}
}

