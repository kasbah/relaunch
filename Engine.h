#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <jack/jack.h>
#include <jack/midiport.h>
//#include <bitset>
#include "ringbuffer.h"
//#include <jack/ringbuffer.h>
//#include "LaunchPad.h"

//#define MIDI_DATA_SIZE 3
//#define MAX_RB_DATA 1024
//#define PAGE_SIZE 8*8
//#define NUM_OF_PAGES 1

using namespace std;

class Engine {
	public:
		Engine();
		~Engine();
		int init();
		void run();
		void check_write_space();
		void queue_event(jack_midi_event_t*);
		int read_data(jack_midi_data_t*);
		//void queue_event(jack_midi_event_t*);
		RingBuffer<jack_midi_data_t> * midi_in_rb;
		RingBuffer<jack_midi_data_t> * midi_out_rb;
		//jack_ringbuffer_t * midi_in_rb;
	private:
		//bitset<PAGE_SIZE> sequence;
		//bitset<PAGE_SIZE> prev_sequence;

};

#endif// __ENGINE_H__
