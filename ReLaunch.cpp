#include "ReLaunch.h"
#include "JackMidi.h"
#include "Engine.h"
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include "AlsaMidi.h"
//#include <unistd.h>
//#include <vector>
using namespace std;

//AlsaMidi alsa_midi(&engine);
Engine engine;
MidiDriver* driver;

int main(int argc, char *argv[])
{
	//install a signal handler to properly quit 
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);

	int opt;
	//int longopt_index;

	//static struct option long_options[] =
	//{
	//	//{ "jack",           0, 0, 'j'},
	//	{ "alsa",           0, 0, 'a'},
	//	{ 0, 0, 0, 0}
	//};

//opt = getopt_long(argc, argv, "a", long_options, &longopt_index);
	opt = getopt(argc, argv, "a");

	cout << "opt: " << opt << endl;

	if (opt > 0)
	{
		switch (opt)
		{
			case 'a':
				cout << "a option" << endl;
				driver = new AlsaMidi(&engine);
				break;
			default:
				driver = new JackMidi(&engine);
				break;
		}
	}
	else
	{
		driver = new JackMidi(&engine);
	}


	cout << "initializing engine" << endl;

	engine.init();
	cout << "running engine" << endl;
	engine.run();
	return 0;
}

static void signal_handler(int sig)
{
	cout << endl << "exiting..." << endl;
	engine.reset_LP();
	//driver->exit();
	exit(0); //class destructors will be called if they are global
}
