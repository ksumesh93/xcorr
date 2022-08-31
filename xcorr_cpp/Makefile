TARGET = xcorr.bin

CC=g++
CFLAGS= -Wall -ggdb -I.
LDLIBS= -lpthread

$(TARGET): $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(TARGET) *.o *~ 