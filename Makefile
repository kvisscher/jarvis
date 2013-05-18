# Compiler options
CC= g++
CFLAGS = -Wall $(POCKETSPHINX_INCLUDES) -Ibuild -Iinclude -I/usr/include
LDFLAGS = -lespeak -lportaudio $(POCKETSPHINX_LIBS) -Iinclude -Ibuild -I/usr/include

# Space seperated list of files to compile
SOURCES = $(wildcard src/*.cc)
OBJECTS = $(SOURCES:.cpp=.o)

# The name of the executable
EXECUTABLE = jarvis

# Add pocketsphinx libraries and includes
CFLAGS  += `pkg-config --cflags-only-I pocketsphinx`
LDFLAGS += `pkg-config --libs --cflags pocketsphinx`

all: $(SOURCES) $(EXECUTABLE)

# Compiles all of the object files
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 
	
# $@ expands to the name of the output file (e.g. main.o)
# $< expands to the name of the file that should be compiled (e.g. main.cpp)
.cpp.o:
	@echo Compiling $< ..
	$(CC) $(CFLAGS) -o $@ -c $<
	@echo Done!
	
clean:
	rm $(OBJECTS) $(EXECUTABLE)
