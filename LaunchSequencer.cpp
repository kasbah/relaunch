#include "LaunchSequencer.h"
#include "JackAudio.h"
#include "Engine.h"
#include <iostream>
#include <signal.h>
//#include <unistd.h>
//#include <vector>
using namespace std;

Engine engine;
JackAudio jack_audio(&engine);

int main()
{  

	cout << "activating JackAudio" << endl;

	jack_audio.activate();

	//install a signal handler to properly quit 
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);

	cout << "initializing engine" << endl;

	engine.init();

	cout << "running engine" << endl;
	engine.run();
		//cout << "initializing JackAudio" << endl;
		//if (jack_audio.client != NULL) 
		//{
		//	if (jack_audio.activate() == 0) 
		//	{
		//		engine->run();
		//	}
		//}
	cout << "exiting.." << endl;
	return 0;
}

static void signal_handler(int sig)
{
	cout <<  endl << "exiting .." << endl;
	engine.reset_LP();

	//jack_audio.close();
	exit(0); //class destructors will be called if they are global
}
