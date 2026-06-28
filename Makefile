build:
	g++ cppsrc/*.cpp cppsrc/algo/*.cpp -o bin/BARDS.exe

debug:
	g++ cppsrc/*.cpp cppsrc/algo/*.cpp -g -o bin/BARDS.exe
	gdb ./bin/BARDS.exe

clean:
	rm -r bin/*

run:
	./bin/BARDS.exe