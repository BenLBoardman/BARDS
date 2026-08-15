build:
	g++ cppsrc/*.cpp cppsrc/algo/*.cpp -std=c++23 -o bin/BARDS.exe

debug:
	g++ cppsrc/*.cpp cppsrc/algo/*.cpp -g -std=c++23 -o bin/BARDS.exe
	gdb ./bin/BARDS.exe

clean:
	rm -r bin/*

run:
	./bin/BARDS.exe