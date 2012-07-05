#include "Engine.h"
#include "Midi.h"
#include "LaunchPad.h"
#include "Scales.h"
#include <unistd.h>
#include <iostream>
//using namespace std;


Engine::Engine()
{
	column = 0;
	ticks = 0;
	page = 0;
	//for (int i = 0; i < ROWS; ++i)
	//{
	//	for (int j = 0; j < COLUMNS; ++j)
	//	{
	//		display[i][j] = 0;
	//	}
	//}
}

int Engine::init()
{
	midi_in_rb = new RingBuffer<jack_midi_data_t>(MAX_RB_DATA);
	midi_out_to_LP_rb = new RingBuffer<jack_midi_data_t>(MAX_RB_DATA);
	midi_out_rb = new RingBuffer<jack_midi_data_t>(MAX_RB_DATA);
	//jack_ringbuffer_t * midi_in_rb = jack_ringbuffer_create(1024);
	//jack_ringbuffer_free(midi_in_rb);
}

inline int Engine::send_to_LP(uint8_t row, uint8_t column, uint8_t colour)
{
	jack_midi_data_t data[3]; 
	data[0] = LP::coordToMidi[row][column][0];
	data[1] = LP::coordToMidi[row][column][1];
	data[2] = colour;
	midi_out_to_LP_rb->write(data, MIDI_DATA_SIZE);
	return 0;
}

inline int Engine::send_to_general(uint8_t row, bool on)
{
	jack_midi_data_t data[3]; 
	data[0] = on ? MIDI::NOTE_ON : MIDI::NOTE_OFF;
	data[1] = 60 + SCALE::major[row];
	data[2] = on ? 127 : 0;
	midi_out_rb->write(data, MIDI_DATA_SIZE);
	return 0;
}


void Engine::run()
{
	while (1)
	{
		usleep(100);
		size_t read_space = midi_in_rb->read_space();
		if(read_space > 0)
		{
			jack_midi_data_t buffer[MIDI_DATA_SIZE];
			midi_in_rb->read(buffer, MIDI_DATA_SIZE);

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

					//update at the old position of cursor
					for (int i = 0; i < ROWS; ++i)
					{
						//jack_midi_data_t data[3]; 
						//data[0] = LP::coordToMidi[i+1][column][0];
						//data[1] = LP::coordToMidi[i+1][column][1];
						//data[2] = sequence_page[i * COLUMNS + column] ? LP::GREEN_FULL : LP::OFF;
						//midi_out_to_LP_rb->write(data, MIDI_DATA_SIZE);
						if (sequence_page[i * COLUMNS + column])
						{
							send_to_LP (i+1, column, LP::GREEN_FULL); 
							//send_to_general(i, false);
						}
						else
							send_to_LP (i+1, column, LP::OFF); 
					}

					//move cursor
					++column;
					if (column >= COLUMNS)
						column = 0;

					//update at the new position of cursor
					for (int i = 0; i < ROWS; ++i)
					{
						//jack_midi_data_t data[3]; 
						//data[0] = LP::coordToMidi[i+1][column][0];
						//data[1] = LP::coordToMidi[i+1][column][1];
						//data[2] = sequence_page[i * COLUMNS + column] ? LP::AMBER_FULL : LP::RED_FULL;
						//midi_out_to_LP_rb->write(data, MIDI_DATA_SIZE);
						if (sequence_page[i * COLUMNS + column])
						{
							send_to_LP (i+1, column, LP::AMBER_FULL); 
							send_to_general(i, true);
							send_to_general(i, false);
						}
						else
							send_to_LP (i+1, column, LP::RED_FULL); 

					}
				}
			}
			else if (buffer[0] == MIDI::START)
			{
				ticks = 0;
				column = 0;
				page = 0;
			}
			for (int i = 0; i < ROWS; ++i)
			{
				for (int j = 0; j < COLUMNS; ++j)
				{
					if ((buffer[0] == MIDI::NOTE_ON) || (buffer[0] == MIDI::CC))
					{
						if (buffer[2] == 127) 
						{
							if (buffer[0] == LP::coordToMidi[i+1][j][0])
							{
								if (buffer[1] == LP::coordToMidi[i+1][j][1])
								{
									sequence_page.flip(i*COLUMNS + j);
									jack_midi_data_t data[3]; 
									data[0] = buffer[0];
									data[1] = buffer[1];
									if (j == column)
										data[2] = sequence_page[i*COLUMNS + j] ? LP::AMBER_FULL : LP::RED_FULL;
									else
										data[2] = sequence_page[i*COLUMNS + j] ? LP::GREEN_FULL : LP::OFF;
									midi_out_to_LP_rb->write(data, MIDI_DATA_SIZE);
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
	//midi_out_to_LP_rb->reset();
	jack_midi_data_t data[3];
	data[0] = LP::RESET;
	data[1] = 0;
	data[2] = 0;
	midi_out_to_LP_rb->write(data, MIDI_DATA_SIZE);
	cout << "resetting LaunchPad" << endl;
	sleep(1);
}


int Engine::read_data_for_LP(jack_midi_data_t* data)
{
	if (midi_out_to_LP_rb->read_space() < MIDI_DATA_SIZE)
		return 1;

	midi_out_to_LP_rb->read(data, MIDI_DATA_SIZE);
	return 0;
}

int Engine::read_data_for_general(jack_midi_data_t* data)
{
	if (midi_out_rb->read_space() < MIDI_DATA_SIZE)
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
