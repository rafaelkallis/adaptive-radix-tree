MAKEFLAGS += --silent

export BUILD_DIR ?= build

default: debug

release:
	mkdir -p ./$(BUILD_DIR) && cd ./$(BUILD_DIR) && cmake ../ -DCMAKE_BUILD_TYPE=Release && cmake --build .

debug:
	mkdir -p ./$(BUILD_DIR) && cd ./$(BUILD_DIR) && cmake ../ -DCMAKE_BUILD_TYPE=Debug && cmake --build .

test:
	@if [ -f ./$(BUILD_DIR)/test ]; then ./$(BUILD_DIR)/test ${ARGS}; else echo "Please run 'make' or 'make release' first" && exit 1; fi

bench:
	@if [ -f ./$(BUILD_DIR)/bench ]; then ./$(BUILD_DIR)/bench; else echo "Please run 'make' or 'make release' first" && exit 1; fi

bench-mem:
	@if [ ! -f ./$(BUILD_DIR)/bench-mem ]; then echo "Please run 'make' or 'make release' first" && exit 1; fi
	@echo "Running memory benchmark with uniform distribution..."
	@valgrind --tool=massif --massif-out-file=./$(BUILD_DIR)/massif.out.uniform ./$(BUILD_DIR)/bench-mem uniform
	@ms_print ./$(BUILD_DIR)/massif.out.uniform | head -50
	@echo ""
	@echo "Running memory benchmark with Zipfian distribution..."
	@valgrind --tool=massif --massif-out-file=./$(BUILD_DIR)/massif.out.zipf ./$(BUILD_DIR)/bench-mem zipf
	@ms_print ./$(BUILD_DIR)/massif.out.zipf | head -50
	@echo ""
	@echo "Full reports saved to:"
	@echo "  - ./$(BUILD_DIR)/massif.out.uniform"
	@echo "  - ./$(BUILD_DIR)/massif.out.zipf"
	@echo ""
	@echo "To view full reports, run:"
	@echo "  ms_print ./$(BUILD_DIR)/massif.out.uniform"
	@echo "  ms_print ./$(BUILD_DIR)/massif.out.zipf"

clean:
	rm -rf ./$(BUILD_DIR)

.PHONY: test bench bench-mem
