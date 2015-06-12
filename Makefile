# For the moment I just use this simple Makefile.
# If you don't want to use libasan, just remove -lasan and -fsanitize=address

all:
	g++ -o test_turtle \
	    tests/turtle.c noc_turtle.c \
	    -Wall \
	    -O0 -g \
	    -I ./ -lglfw3 -lGLEW -lGL -lm -lXrandr -lXcursor -lXinerama -lXi
