#include "jackAudio.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
using namespace std;

jack_ringbuffer_t* ringbuffer = jack_ringbuffer_create(1024);
int main()
{ 
	jackAudio jack_audio (ringbuffer);

	//char event[sizeof(jack_midi_event_t)];

	if (jack_audio.activate() == 0) 
	{
		while (1)
		{
			sleep(1);
			char buffer[sizeof(jack_midi_event_t)];
			jack_midi_event_t *event = new jack_midi_event_t;
			
			cout << "readspace: " << jack_ringbuffer_read_space(ringbuffer) << endl;
			if(jack_ringbuffer_read_space (ringbuffer) >= sizeof(jack_midi_event_t))
			{
				jack_ringbuffer_read(ringbuffer, buffer, sizeof(jack_midi_event_t));	
				memcpy(event, buffer, sizeof(jack_midi_event_t));
				//event = static_cast<jack_midi_event_t*>(buffer);
				cout << "out note on: " << ((*event).buffer[0] == 0x80) << endl;
			}
		}

	}

}
