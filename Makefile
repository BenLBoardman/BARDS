build:
	g++ cppsrc/*.cpp -o bin/BARDS.exe

debug:
	g++ cppsrc/*.cpp -g -o bin/BARDS.exe
	gdb ./bin/BARDS.exe

clean:
	rm -r bin/*

run:
	./bin/BARDS.exe