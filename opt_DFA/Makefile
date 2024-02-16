# A line starting with a has is a comment, we'll build up
# the below example

TARGET = DFAopt
SRC_FILES = main.cpp merge_states.cpp NFAtoDFAfunc.cpp

# DO NOT EDIT BELOW HERE
CXX = g++
CFLAGS = -Wall -g -std=c++11
OBJECTS = $(SRC_FILES:.cpp=.o)

ifeq ($(shell echo "Windows"), "Windows")
	TARGET := $(TARGET).exe
	DEL = del
else
	DEL = rm
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	$(DEL) $(TARGET) $(OBJECTS)

# DEPENDENCIES
main.o: main.cpp merge_states.h NFAtoDFAfunc.h
merge_states.o: merge_states.cpp merge_states.h
NFAtoDFAfunc.o: NFAtoDFAfunc.cpp NFAtoDFAfunc.h
