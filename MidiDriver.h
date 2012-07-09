#ifndef __MIDIDRIVER_H__
#define __MIDIDRIVER_H__
#include "Engine.h"
class MidiDriver {
	public:
		MidiDriver (Engine* e) : engine(e) {};
		virtual ~MidiDriver() {};
		virtual int exit() {};
	protected:
		Engine* engine;
};
#endif// __MIDIDRIVER_H__
