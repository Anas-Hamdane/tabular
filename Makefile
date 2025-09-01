TARGET     := tabular
STATIC     := lib$(TARGET).a
DYNAMIC    := lib$(TARGET).so

SRC        := ./src
INCLUDE    := ./include
BUILD      := ./build

CXX        := g++
AR         := ar
CXXFLAGS   := -std=c++11 -Wall -Wextra -g -fpic -I./include/

RSS        := $(SRC)/Color.cpp                \
							$(SRC)/Column.cpp               \
							$(SRC)/Config.cpp               \
							$(SRC)/String.cpp               \

OBJS := $(RSS:$(SRC)/%.cpp=$(BUILD)/%.o)

.PHONY: static shared all clean

all: static dynamic
static: $(STATIC)
dynamic: $(DYNAMIC)

$(STATIC): $(OBJS) | $(BUILD)
	$(AR) rcs $@ $(OBJS)

$(DYNAMIC): $(OBJS) | $(BUILD)
	$(CXX) -shared -o $@ $(OBJS)

$(BUILD)/%.o: $(SRC)/%.cpp $(BUILD)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
	rm -rf $(STATIC)
	rm -rf $(DYNAMIC)
