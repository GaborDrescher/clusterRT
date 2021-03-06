EXE := rt
CC  := mpicxx
CXX := mpicxx

CXXFLAGS := -Wall -Wextra -O3 -march=native -mtune=native -mfpmath=sse -ffast-math -fopenmp `sdl-config --cflags --libs` -lGL -lglut -lGLU -lrt
#CXXFLAGS := -Wall -Wextra -O0 -g `sdl-config --cflags --libs` -lGL -lglut -lGLU -lrt

#intel compiler flags
#CXXFLAGS := -O3 -ipo -xHOST -fomit-frame-pointer -no-prec-div -openmp -lrt 

SOURCES := $(shell find . -maxdepth 1 -name "*.cpp")
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))

HEADERS := $(shell find . -name "*.h")
ADDDEP  := $(HEADERS) Makefile

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): $(ADDDEP)

%o: %cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXE)
