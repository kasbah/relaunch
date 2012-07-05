#include "Engine.h"
#include "Midi.h"
#include "LaunchPad.h"
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

			if (buffer[0] == MIDI::NOTE_OFF)
			{
				buffer[0] = MIDI::NOTE_ON;
			}

			if (buffer[0] == MIDI::TICK)
			{
				++ticks;
				if (ticks >= TICKS_PER_COLUMN)
				{
					ticks = 0;
					++column;
					if (column >= NUMBER_OF_COLUMNS)
					{
						column = 0;
					}
					for(int i = 1; i < (NUMBER_OF_ROWS + 1); ++i)
					{
						cerr << "column: " << column << endl;
						jack_midi_data_t pos_data[3]; 
						pos_data[0] = LP::coordToMidi[i][column] >> 16;
						pos_data[1] = (LP::coordToMidi[i][column] >> 8) & 0xFF;
						pos_data[2] = sequence_page[(i-1)*NUMBER_OF_ROWS + column] ? LP::YELLOW : LP::RED_FULL;
						midi_out_rb->write(pos_data, MIDI_DATA_SIZE);
					}
				}
			}
			else if (buffer[0] == MIDI::START)
			{
				ticks = 0;
				column = 0;
			}
			for (int i = 0; i < NUMBER_OF_ROWS; ++i)
			{
				for (int j = 0; j < NUMBER_OF_COLUMNS; ++j)
				{
					if ((buffer[0] == MIDI::NOTE_ON) || (buffer[0] == MIDI::CC))
					{
						if (buffer[2] == 127) 
						{
							if (buffer[0] == (LP::coordToMidi[i+1][j] >> 16))
							{
								if (buffer[1] == ((LP::coordToMidi[i+1][j] >> 8) & 0xFF))
								{
									sequence_page.flip(i*NUMBER_OF_ROWS + j);
									//update the light for this pad immediately
									jack_midi_data_t data[3]; 
									data[0] = buffer[0];
									data[1] = buffer[1];
									data[2] = sequence_page[i*NUMBER_OF_ROWS + j] ? LP::AMBER_FULL : LP::OFF;
									midi_out_rb->write(data, MIDI_DATA_SIZE);
								}
							}
						}
					}
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

void Engine::reset_LP()
{
	cout << "resetting LP" << endl;
	jack_midi_data_t data[3];
	data[0] = LP::RESET >> 16;
	data[1] = 0;
	data[2] = 0;
	midi_out_rb->write(data, MIDI_DATA_SIZE);
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
	cout << "Engine destructor called" << endl;
}
