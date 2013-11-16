CC=g++
CFLAGS= -c -Wall -Wimplicit 
LDFLAGS=
SOURCES=Urt_Updater_Cmd.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=UrTUpdater_Ded

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
