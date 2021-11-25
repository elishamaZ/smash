# Makefile for the smash program
CXX=g++
CXXFLAGS=-g -Wall -std=c++11
CCLINK = $(CXX)
OBJS = smash.o commands.o signals.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.cpp commands.h
smash.o: smash.cpp commands.h
signals.o: signals.cpp signals.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

