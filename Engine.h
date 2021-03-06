#ifndef __ENGINE_H__
#define __ENGINE_H__

//#include <jack/jack.h>
//#include <jack/midiport.h>
#include <bitset>
#include <vector>
#include <list>
#include "ringbuffer.h"
#include "Midi.h"
#include <stdint.h>
//#include <jack/ringbuffer.h>
//#include "LaunchPad.h"

#define MIDI_DATA_SIZE 3
#define MAX_RB_DATA 1024
#define COLUMNS 8
#define ROWS 8
#define TICKS_PER_COLUMN 6 

using namespace std;

class Engine {
	public:
		Engine();
		~Engine();
		int init();
		void run();
		//void check_write_space();
		void queue_event(uint8_t*);
		int read_data_for_LP(uint8_t*);
		int read_data_for_general(uint8_t*);
		void reset_LP();
		//void queue_event(jack_midi_event_t*);
		RingBuffer<uint8_t> * midi_in_rb;
		RingBuffer<uint8_t> * midi_out_to_LP_rb;
		RingBuffer<uint8_t> * midi_out_rb;
		//jack_ringbuffer_t * midi_in_rb;
	private:
		int column;
		int ticks;
		int page;
		int send_to_LP(uint8_t row, uint8_t column, uint8_t colour);
		int send_to_general(uint8_t row, uint8_t type);
		int send_later(uint8_t row, uint8_t type, int offset);
		list<MidiOutEvent> out_queue; 
		bitset<ROWS * COLUMNS> sequence_page;
		//int8_t display[ROWS][COLUMNS] = {{0}};
		//bitset<PAGE_SIZE> prev_sequence;
		//enum pad_state {
		//	CURSOR_OFF = -1,
		//	OFF,
		//	ON,
		//	CURSOR_ON,
		//};


};

#endif// __ENGINE_H__
