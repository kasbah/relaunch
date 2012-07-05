#ifndef __MIDI_H__
#define __MIDI_H__

namespace MIDI {

	enum type {
		NOTE_ON  = 0x90,
		NOTE_OFF = 0x80,
		START    = 0xFA,
		TICK     = 0XF0
	};

}

#endif// __MIDI_H__
