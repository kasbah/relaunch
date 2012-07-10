#ifndef __ALSAMIDI_H__
#define __ALSAMIDI_H__

#include <alsa/asoundlib.h>
#include <list>
#include <pthread.h>

#include "MidiDriver.h"
#include "Engine.h"

using namespace std;

class AlsaMidi 
	: public MidiDriver
{
	public:
		AlsaMidi(Engine* e);
		~AlsaMidi();
		virtual void close();
	private:
		int client_id;
		int LP_id;
		//list<snd_seq_port_subscribe_t> subscriptions;
		typedef pair<snd_seq_addr_t, snd_seq_addr_t> SubsPair;
	  list<SubsPair> subscriptions;
		bool is_connected_to_LP;
		bool connect_to_LP ();
		bool connect_to_System();
		bool disconnect_from_System();
		snd_seq_t * seq_handle;
		pthread_t process_thread;
		int midiPort;
		int toLP_midiOut;	
		static void* _process(void* arg);
		void process();
};
	

#endif// __ALSAMIDI_H__
