#ifndef __MIDIDRIVER_H__
#define __MIDIDRIVER_H__
#include "Engine.h"
class MidiDriver {
	public:
		MidiDriver (Engine* e) : engine(e) {};
		virtual ~MidiDriver() {};
		virtual void close() {};
	protected:
		Engine* engine;
};

struct DriverSettings {
	uint8_t remap[8][8];
	enum ClockSource {
		SELF,
		MIDI,
		AUTO
	};
	uint8_t clock_source;
};
#endif// __MIDIDRIVER_H__
