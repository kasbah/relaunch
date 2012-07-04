#include "launchSequencer.h"
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <cstring>
//#include <vector>
using namespace std;

jack_ringbuffer_t* midiInRB = jack_ringbuffer_create(MAX_RB_DATA);
jack_ringbuffer_t* seqRB = jack_ringbuffer_create(MAX_RB_DATA);
jackAudio jack_audio (midiInRB, seqRB);

//vector<bool> sequence;
//vector<bool>::iterator seq_it;

bitset<PAGE_SIZE> sequence;
bitset<PAGE_SIZE> prev_sequence;

static void signal_handler(int sig)
{
	cout <<  endl << "exiting .." << endl;
	//delete midiInRB;
	exit(0); //jack_audio destructor will be called if jack_audio is global
}

int main()
{  
		// install a signal handler to properly quit jack client 
		signal(SIGQUIT, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGHUP, signal_handler);
		signal(SIGINT, signal_handler);

		jack_ringbuffer_free(seqRB);
		//cout << "coord: " << (int)(LP.coordToMidi[1][8] >> 16) << endl;

	//char event[sizeof(jack_midi_event_t)];
	if (jack_audio.client != NULL) 
	{
		if (jack_audio.activate() == 0) 
		{
			while (1)
			{
				sleep(1);

				if (jack_ringbuffer_read_space(midiInRB) >= (MAX_RB_DATA - 1))
					cerr << "WARNING: too many midi messages queued, may skip some" << endl;
				while(jack_ringbuffer_read_space(midiInRB) >= MIDI_DATA_SIZE)
				{
					jack_midi_data_t midi_data[MIDI_DATA_SIZE];
					jack_ringbuffer_read(midiInRB, midi_data, MIDI_DATA_SIZE);	
					//convert noteOff to noteOn 
					if (midi_data[0] == 0x80)
						midi_data[0] = 0x90;
					for (int i = 1; i < 9; ++i)
					{
						for (int j = 0; j < 8; ++j)
						{
							if (midi_data[0] == (LP::coordToMidi[i][j] >> 16))
							{
								if(midi_data[1] == ((LP::coordToMidi[i][j] >> 8) & 0xFF))
								{
									cout << "(" << i << "," << j << ")" << ":" << (int)midi_data[2] << endl;
									sequence[(i-1)*8 + j] = sequence[(i-1)*8 + j] xor (midi_data[2] == 127);
								}
							}
						}
					}
				}
				if (sequence != prev_sequence)
				{
					uint8_t seq[8] = {0,0,0,0,0,0,0,0};
					//char seq[8];// =  {0,0,0,0,0,0,0,0};
					for(int i = 0; i < 8; ++i)
					{
						seq[i] = 8;
						//for(int j = 0; j < 8; ++j)
						//{
						//	seq[i] |= sequence[i*8 + j] << j;	
						//}
						cout << "seq: " << i << " is " << (unsigned int)seq[i] << endl;
					}
					cout << "write space: " << jack_ringbuffer_write_space(seqRB) << endl;
					jack_ringbuffer_write(seqRB, seq, 64); // need -fpermissive to cast buffer to (const char*)
					cerr << "sequence:" << sequence << endl;
				}
				prev_sequence = sequence;
			}

		}
	}
}
