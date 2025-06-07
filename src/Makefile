LOCAL_PATH := $(shell pwd)

_OBJS = main.o chunk.o debug.o
OBJS = $(patsubst %,$(OUT_DIR)/%,$(_OBJS))

$(OUT_DIR)/cxxlox: $(OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(OUT_DIR)/%.o: %.cpp
	$(CXX) -MMD -c $< -o $@ $(CXXFLAGS)

-include $(OUT_DIR)/*.d

.PHONY: clean
clean:
	rm -f $(OBJS) $(OUT_DIR)/*.d $(OUT_DIR)/cxxlox
