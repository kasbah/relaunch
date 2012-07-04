#ifndef __LAUNCHSEQUENCER_H__
#define __LAUNCHSEQUENCER_H__ 1

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>
#include <bitset>
#include "jackAudio.h"
#include "launchPad.h"

#define MIDI_DATA_SIZE 3
#define MAX_RB_DATA 1024
#define PAGE_SIZE 8*8
#define NUM_OF_PAGES 1

#endif //__LAUNCHSEQUENCER_H__
