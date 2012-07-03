all:
	g++ -o launchSequencer jackAudio.cpp launchSequencer.cpp `pkg-config --cflags --libs jack`
