# Define variables
SRCDIR := src
SOURCES := ga_solve.cpp Cnf.cpp evolution/Evolution.cpp evolution/Individual.cpp
OBJDIR := .obj
OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))  # Create a list of object files in obj/
INCDIR := include

# Compiler settings
CXX := g++
CXXFLAGS := -O3 -std=c++17 -I$(INCDIR)  # Optimization flags

# Define the target executable
TARGET := ga_solve

# Rule for building the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Rule for compiling .cpp files to .o files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)    # Create the directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up
clean:
	rm -rf $(OBJDIR) $(TARGET)