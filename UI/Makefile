TARGET = gui.app
MKDIR_P = mkdir -p

CXX    = g++
CFLAGS = -c -Wall -Iinclude `pkg-config --cflags gtkmm-3.0 opencv` -lpython2.7
LFLAGS = `pkg-config --libs gtkmm-3.0 opencv` -lraspicam_cv -lpython2.7 -lboost_system -lboost_filesystem

SRCDIR = src
OBJDIR = obj
INCDIR = include
EXEDIR = bin
DIRS   = $(EXEDIR) $(OBJDIR)

_OBJ   = main.o gui.o viewfinder.o exceptions.o
OBJ    = $(patsubst %,$(OBJDIR)/%,$(_OBJ))
_INC   = gui.hpp camera_mode.hpp viewfinder_mode.hpp viewfinder.hpp
INC    = $(patsubst %,$(INCDIR)/%,$(_INC))

$(shell mkdir -p $(DIRS))

all: $(OBJ)
	$(info Linking $(TARGET))
	$(info)
	$(CXX) -o $(EXEDIR)/$(TARGET) $^ $(LFLAGS)
	$(info Linking finished.)

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INC)
	$(info Compiling $@...)
	$(info)
	$(CXX) -o $@ $< $(CFLAGS) 
	$(info Compiling finished.)

.PHONY: clean

clean:
	$(info Cleaning...)
	$(info)
	rm -f $(EXEDIR)/$(TARGET)
	rmdir $(EXEDIR)/
	rm -f $(OBJDIR)/*.o
	rmdir $(OBJDIR)/
	$(info Cleaning finished.)
