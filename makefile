SOURCES=$(wildcard *.cpp)
OBJECTS=$(addprefix bin/,$(addsuffix .o,$(notdir $(basename $(SOURCES)))))
LDFLAGS= -lmingw32 -lsdlmain -lsdl -lglew32 -lopengl32 -lglu32

G++FLAGS= -Wall -O2 -mno-ms-bitfields 

all: mapabsas.exe

mapabsas.exe: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

bin/%.o: %.cpp
	$(CXX) -c $< -o $@ $(G++FLAGS)

clean:
	@del bin\*.o
	@del mapabsas.exe
	
run:
	mapabsas
	
.PHONY: clean run	
