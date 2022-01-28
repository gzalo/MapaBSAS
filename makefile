SOURCES=$(wildcard *.cpp)
OBJECTS=$(addprefix bin/,$(addsuffix .o,$(notdir $(basename $(SOURCES)))))
LDFLAGS= -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -static -static-libgcc -lopengl32 -lglu32 -lm -lgdi32 -lwinmm -ldinput8 -luser32 -lole32 -loleaut32 -limm32 -lshell32 -lversion -lsetupapi -lhid

G++FLAGS= -Wall -O2 -mno-ms-bitfields -std=c++11

all: mapabsas.exe

mapabsas.exe: $(OBJECTS) bin/gl3w.o
	$(CXX) $(OBJECTS) bin/gl3w.o -o $@ $(LDFLAGS)

bin/%.o: %.cpp
	$(CXX) -c $< -o $@ $(G++FLAGS)

bin/gl3w.o: GL/gl3w.c
	$(CXX) -c $< -o $@ $(G++FLAGS)

clean:
	@del bin\*.o
	@del mapabsas.exe
	
run:
	mapabsas
	
.PHONY: clean run	
