CXXFLAGS = -std=c++17 -Og -Wall -municode
OBJDIR = ./obj/
SRCDIR = ./src/
OBJ := $(OBJDIR)base64.o $(OBJDIR)window.o $(OBJDIR)main.o

decode.exe: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -lComCtl32 -o $@

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

test: decode.exe
	decode.exe

clean:
	@cd $(OBJDIR) && @del /Q *.o 2>nul
	
.PHONY: clean test