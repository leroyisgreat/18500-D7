TARGET = gui.app

CXX    = g++
CFLAGS = -Wall -Iinclude `pkg-config gtkmm-3.0 --cflags --libs`

SRCDIR = src
OBJDIR = obj
INCDIR = include

_OBJ   = main.o gui.o viewfinder.o
OBJ    = $(patsubst %,$(OBJDIR)/%,$(_OBJ))
_INC   = gui.h viewfinder.h camera_state.h
INC    = $(patsubst %,$(INCDIR)/%,$(_INC))

all: $(OBJ)
	$(info Linking $(TARGET))
	$(info)
	$(CXX) -o bin/$(TARGET) $^ $(CFLAGS) 
	$(info Linking finished.)

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INC)
	$(info Compiling $@...)
	$(info)
	$(CXX) -c -o $@ $< $(CFLAGS) 
	$(info Compiling finished.)

.PHONY: clean

clean:
	$(info Cleaning...)
	$(info)
	rm -f bin/$(TARGET)
	rm -f obj/*.o
	$(info Cleaning finished.)
