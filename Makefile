CPP_DIRS := cppsrc/geoUtil/*.cpp cppsrc/*.cpp cppsrc/algo/*.cpp -I cppsrc -I cppsrc/geoUtil


build:
	g++ $(CPP_DIRS) -std=c++23 -o bin/BARDS.exe

debug:
	g++ $(CPP_DIRS) -g -std=c++23 -o bin/BARDS.exe
	gdb ./bin/BARDS.exe

clean:
	rm -r bin/*

run:
	./bin/BARDS.exe