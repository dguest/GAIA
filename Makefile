BIN = bin
SRC = src
INC = include

INSTALLPATH = /usr/local/bin


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
ROOTLIBS = $(shell root-config --libs)

CXXFLAGS += $(ROOTCFLAGS)
LIBS += $(ROOTLIBS)

OBJ = VarUtils.o main.o NeuralNet.o Architecture.o Layer.o Activation.o Dataset.o

TARGET = TaggerFramework


$(TARGET): $(OBJ:%=$(BIN)/%)
	@echo "Linking the target $@"
	@$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS)
	
$(BIN)/%.o: %.cpp
	@echo compiling $<
	@mkdir -p $(BIN)
	@$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean

CLEANLIST = *~ *.o *.o~

install:
	@echo "Installing $(TARGET) to $(INSTALLPATH)"
	@rm -rf $(INSTALLPATH)/$(TARGET)
	@ln $(TARGET) $(INSTALLPATH)/$(TARGET)

clean:
	rm -rf $(CLEANLIST) $(CLEANLIST:%=$(BIN)/%)
	rm -rf $(BIN)

purge:
	rm -rf $(CLEANLIST) $(CLEANLIST:%=$(BIN)/%)
	rm -rf $(BIN)
	rm -rf $(TARGET) $(INSTALLPATH)/$(TARGET)

