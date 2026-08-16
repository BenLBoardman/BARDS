CPP_DIRS := cppsrc/geoUtil/*.cpp cppsrc/*.cpp cppsrc/algo/*.cpp -I cppsrc -I cppsrc/geoUtil


ifeq (run, $(firstword $(MAKECMDGOALS)))
  runargs := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  $(eval $(runargs):;@true)
endif


build:
	g++ $(CPP_DIRS) -std=c++23 -o bin/BARDS.exe

debug:
	g++ $(CPP_DIRS) -g -std=c++23 -o bin/BARDS.exe
	gdb ./bin/BARDS.exe $(runargs)

clean:
	rm -r bin/*


.PHONY: run

run:
	./bin/BARDS.exe $(runargs) $(if $(districts),districts=$(districts))