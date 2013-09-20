BIN = bin
SRC = src
INC = include

INSTALLPATH = /usr/local/bin
HEADERPATH = /usr/local/include


vpath %.o    $(BIN)
vpath %.cpp  $(SRC) 
vpath %.h    $(INC) 

# DEBUG = -g

CXX = g++
CXXFLAGS = -std=c++11 -O3 -I$(INC) -fPIC $(DEBUG) 
LIBS = 

LDFLAGS = 
#-L/usr/local/opt/boost/lib

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = -L$(shell root-config --libdir) -lTree
ROOTLDFLAGS = $(shell root-config --ldflags)

CXXFLAGS += $(ROOTCFLAGS)
LIBS += $(ROOTLIBS)
LDFLAGS += $(ROOTLDFLAGS)

OBJ = main.o NeuralNet.o Architecture.o Layer.o Activation.o Dataset.o

HEADER = JetTagger.h

TARGET = TaggerFramework


$(TARGET): $(OBJ:%=$(BIN)/%)
	@echo "Linking the target $@"
	@$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

$(BIN)/%.o: %.cpp
	@echo compiling $<
	@mkdir -p $(BIN)
	@$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean test

CLEANLIST = *~ *.o *.o~

install:
	@echo "Installing $(TARGET) to $(INSTALLPATH)"
	@rm -rf $(INSTALLPATH)/$(TARGET)
	@ln $(TARGET) $(INSTALLPATH)/$(TARGET)

header:
	@echo "Installing $(HEADER) to $(HEADERPATH)"
	@cp $(INC)/$(HEADER) $(HEADERPATH)

clean:
	rm -rf $(CLEANLIST) $(CLEANLIST:%=$(BIN)/%)
	rm -rf $(BIN)

purge:
	rm -rf $(CLEANLIST) $(CLEANLIST:%=$(BIN)/%)
	rm -rf $(BIN)
	rm -rf $(TARGET) $(INSTALLPATH)/$(TARGET)

# ----- lightweight client example

APP_EXAMPLE = app-example

test: $(APP_EXAMPLE)

$(APP_EXAMPLE): $(APP_EXAMPLE).cxx JetTagger.h
	@echo "making lightweight example"
	@$(CXX) $< -o $@
	@echo "made $(APP_EXAMPLE), run to test!"