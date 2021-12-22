all: opencltest

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2

opencltest: Makefile opencltest.c
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) opencltest.c -lOpenCL

clean:
	rm -f opencltest

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./opencltest

