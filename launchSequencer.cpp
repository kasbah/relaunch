#include "jackAudio.h"
#include "launchPad.h"
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <cstring>
using namespace std;

#define MIDI_DATA_SIZE 3
#define MAX_MIDI_DATA 1024


jack_ringbuffer_t* ringbuffer = jack_ringbuffer_create(MAX_MIDI_DATA);
jackAudio jack_audio (ringbuffer);

static void signal_handler(int sig)
{
	cout <<  endl << "exiting .." << endl;
	//delete ringbuffer;
	exit(0); //jack_audio destructor will be called if jack_audio is global
}

int main()
{  
		// install a signal handler to properly quits jack client 
		signal(SIGQUIT, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGHUP, signal_handler);
		signal(SIGINT, signal_handler);

		//cout << "coord: " << (int)(LP.coordToMidi[1][8] >> 16) << endl;

	//char event[sizeof(jack_midi_event_t)];
	if (jack_audio.client != NULL) 
	{
		if (jack_audio.activate() == 0) 
		{
			while (1)
			{
				//sleep(1);
				//jack_midi_event_t *event = new jack_midi_event_t;
				//jack_midi_data_t data[MIDI_DATA_SIZE];

				if (jack_ringbuffer_read_space(ringbuffer) >= (MAX_MIDI_DATA - 1))
					cerr << "WARNING: too many midi messages queued, may skip some" << endl;
				while(jack_ringbuffer_read_space(ringbuffer) >= MIDI_DATA_SIZE)
				{
					jack_midi_data_t buffer[MIDI_DATA_SIZE];
					jack_ringbuffer_read(ringbuffer, buffer, MIDI_DATA_SIZE);	
					//memcpy(data, buffer, MIDI_DATA_SIZE);
					//event = static_cast<jack_midi_event_t*>(buffer);
					//for (int i = 0; i < MIDI_DATA_SIZE; ++i)
					//{
					//	(jack_midi_data_t)(buffer[i]);
					//}
					//reinterpret_cast<jack_midi_data_t*>(buffer);
					if (buffer[0] == 0x80)
					{
						buffer[0] = 0x90;
						buffer[2] = 0;
					}
					for (int i = 0; i < 9; ++i)
					{
						for (int j = 0; j < 9; ++j)
						{
							if (buffer[0] == (LP.coordToMidi[i][j] >> 16))
							{
								if(buffer[1] == ((LP.coordToMidi[i][j] >> 8) & 0xFF))
								{
									cout << "(" << i << "," << j << ")" << ":" << (int)buffer[2] << endl;
								}
							}
						}
					}
					//cout << "out note on: " << (buffer[0] == 0x90) << endl;
					//cout << "out note key: " << (int)buffer[1] << endl;
					//cout << "out note vel: " << (int)buffer[2] << endl;
				}
			}
		}
	}
}
