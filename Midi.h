#ifndef __MIDI_H__
#define __MIDI_H__

namespace MIDI {

	enum type {
		NOTE_ON  = 0x90,
		NOTE_OFF = 0x80,
		CC       = 0xB0,
		START    = 0xFA,
		TICK     = 0xF8
	};

}

	struct MidiOutEvent {
		MidiOutEvent(jack_midi_data_t* d, int o) : offset(o) {data[0] = d[0]; data[1] = d[1]; data[2] = d[2];};
		MidiOutEvent(jack_midi_data_t d0, jack_midi_data_t d1, jack_midi_data_t d2, int o=0) : offset(o) {data[0] = d0; data[1] = d1; data[2] = d2;};
		bool operator==(MidiOutEvent e2) {return ((e2.data[0] == data[0]) && (e2.data[1] == data[1]) && (e2.data[2] == data[2]));};
		jack_midi_data_t data[3];
		int offset;
	};

#endif// __MIDI_H__
