#include "Engine.h"
#include "Midi.h"
#include <unistd.h>
#include <iostream>
//using namespace std;


Engine::Engine()
{
	column = 0;
	ticks = 0;
	page = 0;
}

int Engine::init()
{
	midi_in_rb = new RingBuffer<jack_midi_data_t>(MAX_RB_DATA);
	midi_out_rb = new RingBuffer<jack_midi_data_t>(MAX_RB_DATA);
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
			jack_midi_data_t buffer[MIDI_DATA_SIZE];
			midi_in_rb->read(buffer, MIDI_DATA_SIZE);
			//cout << "in type" << (int)buffer[0] << endl;
			//cout << "in note" << (int)buffer[1] << endl;
			//cout << "in vel" << (int)buffer[2] << endl;
			//midi_out_rb->write(buffer,MIDI_DATA_SIZE);
			if (buffer[0] == 248)
			{
				++ticks;
				if (ticks >= TICKS_PER_COLUMN)
				{
					ticks = 0;
					++column;
					if (column >= NUMBER_OF_COLUMNS)
						column = 0;
				cerr << "column: " << column << endl;
				}
			}
		}
	}
}
void Engine::queue_event(jack_midi_data_t* data)
{
	//cout << "write space:" << jack_ringbuffer_write_space(midi_in_rb) << endl;
	//jack_ringbuffer_write(midi_in_rb, (*event).buffer, MIDI_DATA_SIZE);
	//cout << "in write space:" << midi_in_rb->write_space() << endl;
	midi_in_rb->write(data, MIDI_DATA_SIZE);
}

int Engine::read_data(jack_midi_data_t* data)
{
	//cout << "out read space:" << midi_out_rb->read_space() << endl;
	if (midi_out_rb->read_space() == 0)
		return 1;

	midi_out_rb->read(data, MIDI_DATA_SIZE);
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
