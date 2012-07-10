#include "Engine.h"
#include "LaunchPad.h"
#include "Scales.h"
#include <unistd.h>
#include <iostream>
//using namespace std;


Engine::Engine()
{
	column = 0;
	ticks = TICKS_PER_COLUMN - 1;
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
	midi_in_rb = new RingBuffer<uint8_t>(MAX_RB_DATA);
	midi_out_to_LP_rb = new RingBuffer<uint8_t>(MAX_RB_DATA);
	midi_out_rb = new RingBuffer<uint8_t>(MAX_RB_DATA);
	//jack_ringbuffer_t * midi_in_rb = jack_ringbuffer_create(1024);
	//jack_ringbuffer_free(midi_in_rb);
}

inline int Engine::send_to_LP(uint8_t row, uint8_t column, uint8_t colour)
{
	uint8_t data[3]; 
	data[0] = LP::coordToMidi[row][column][0];
	data[1] = LP::coordToMidi[row][column][1];
	data[2] = colour;
	midi_out_to_LP_rb->write(data, MIDI_DATA_SIZE);
	return 0;
}

inline int Engine::send_to_general(uint8_t row, uint8_t type)
{
	uint8_t data[3]; 
	data[0] = type; //on ? MIDI::NOTE_ON : MIDI::NOTE_OFF;
	data[1] = 60 + SCALE::major[7 - row];
	data[2] = (type==MIDI::NOTE_ON) ? 127 : 0;
	//send and delete noteoff now if one is queued for this note
	if (type == MIDI::NOTE_ON)
	{
		MidiOutEvent event(MIDI::NOTE_OFF, data[1], 0);
		list<MidiOutEvent>::iterator it = out_queue.begin();
		list<MidiOutEvent>::iterator tmpit;// tmpit = it;
		while (it != out_queue.end())
		{
			tmpit = it;
			++it;
			if(event == (*tmpit))
			{
				midi_out_rb->write((*tmpit).data, MIDI_DATA_SIZE);
				out_queue.erase(tmpit);
			}
		}
	}
	midi_out_rb->write(data, MIDI_DATA_SIZE);
	return 0;
}

inline int Engine::send_later(uint8_t row, uint8_t type, int offset)
{
	uint8_t data[3]; 
	data[0] = type; //on ? MIDI::NOTE_ON : MIDI::NOTE_OFF;
	data[1] = 60 + SCALE::major[7 - row];
	data[2] = (type==MIDI::NOTE_ON) ? 127 : 0;
	MidiOutEvent event(data, offset);
	out_queue.push_back(event);
	return 0;
}


void Engine::run()
{
	while (1)
	{
		usleep(100);
		if(midi_in_rb->read_space() > 0)
		{
			uint8_t buffer[MIDI_DATA_SIZE];
			midi_in_rb->read(buffer, MIDI_DATA_SIZE);

			if (buffer[0] == MIDI::NOTE_OFF)
			{
				buffer[0] = MIDI::NOTE_ON;
			}
			
			if (buffer[0] == MIDI::START)
			{
				ticks = TICKS_PER_COLUMN - 1;
				column = 8;
				page = 0;
			}

			if (buffer[0] == MIDI::TICK || buffer[0] == MIDI::START)
			{
				++ticks;
				//decrement the offsets in the out queue by one tick until they hit zero
				//if they do send and delete them
				if(!out_queue.empty())
				{
					list<MidiOutEvent>::iterator it = out_queue.begin();
					list<MidiOutEvent>::iterator tmpit;// tmpit = it;
					while (it != out_queue.end())
					{
						tmpit = it;
						++it;
						if(--((*tmpit).offset) <= 0)
						{
							midi_out_rb->write((*tmpit).data, MIDI_DATA_SIZE);
							out_queue.erase(tmpit);
						}
					}
				}
				if (ticks >= TICKS_PER_COLUMN)
				{
					ticks = 0;

					//update at the old position of cursor
					for (int i = 0; i < ROWS; ++i)
					{
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
						if (sequence_page[i * COLUMNS + column])
						{
							send_to_LP (i+1, column, LP::AMBER_FULL); 
							send_to_general(i, MIDI::NOTE_ON);
							send_later(i, MIDI::NOTE_OFF, 24);
						}
						else
							send_to_LP (i+1, column, LP::RED_FULL); 

					}
				}
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
									uint8_t data[3]; 
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
void Engine::queue_event(uint8_t* data)
{
	midi_in_rb->write(data, MIDI_DATA_SIZE);
}

void Engine::reset_LP()
{
	//midi_out_to_LP_rb->reset();
	midi_out_to_LP_rb->write(LP::reset, MIDI_DATA_SIZE);
	cout << "resetting LaunchPad" << endl;
	sleep(1);
}


int Engine::read_data_for_LP(uint8_t* data)
{
	if (midi_out_to_LP_rb->read_space() < MIDI_DATA_SIZE)
		return 1;

	midi_out_to_LP_rb->read(data, MIDI_DATA_SIZE);
	return 0;
}

int Engine::read_data_for_general(uint8_t* data)
{
	if (midi_out_rb->read_space() < MIDI_DATA_SIZE)
		return 1;

	midi_out_rb->read(data, MIDI_DATA_SIZE);
	return 0;
}

Engine::~Engine()
{
	cout << "Engine destructor called" << endl;
}
