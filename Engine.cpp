#include "Engine.h"
#include <unistd.h>
#include <iostream>
//using namespace std;


Engine::Engine()
{
}

int Engine::init()
{
	midi_in_rb = new RingBuffer<jack_midi_data_t>(1024);
	midi_out_rb = new RingBuffer<jack_midi_data_t>(1024);
	//jack_ringbuffer_t * midi_in_rb = jack_ringbuffer_create(1024);
	//jack_ringbuffer_free(midi_in_rb);
}

void Engine::run()
{
	while (1)
	{
		usleep(100);
		//cout << "read space:" << jack_ringbuffer_read_space(midi_in_rb) << endl;
		size_t read_space = midi_in_rb->read_space();
		//cout << "in read space:" << midi_in_rb->read_space() << endl;
		if(read_space > 0)
		{
			jack_midi_data_t buffer[3];
			midi_in_rb->read(buffer,3);
			//cout << "in type" << (int)buffer[0] << endl;
			//cout << "in note" << (int)buffer[1] << endl;
			//cout << "in vel" << (int)buffer[2] << endl;
			midi_out_rb->write(buffer,3);
		}
	}
}
void Engine::queue_event(jack_midi_event_t* event)
{
	//cout << "write space:" << jack_ringbuffer_write_space(midi_in_rb) << endl;
	//jack_ringbuffer_write(midi_in_rb, (*event).buffer, 3);
	//cout << "in write space:" << midi_in_rb->write_space() << endl;
	midi_in_rb->write(event->buffer, 3);
}

int Engine::read_data(jack_midi_data_t* data)
{
	//cout << "out read space:" << midi_out_rb->read_space() << endl;
	if (midi_out_rb->read_space() == 0)
		return 1;

	midi_out_rb->read(data,3);
	return 0;
}

//
//void Engine::queue_event(jack_midi_event_t* event)
//{
//	//if(midi_in_rb->write_space() > 0)
//	//	midi_in_rb->write(&event, 1);
//}


Engine::~Engine()
{
}
