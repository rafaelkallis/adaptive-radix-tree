
export BUILD_DIR ?= build

default: debug

release:
	mkdir -p ./$(BUILD_DIR) && cd ./$(BUILD_DIR) && cmake ../ -DCMAKE_BUILD_TYPE=Release && VERBOSE=1 cmake --build .

debug:
	mkdir -p ./$(BUILD_DIR) && cd ./$(BUILD_DIR) && cmake ../ -DCMAKE_BUILD_TYPE=Debug && VERBOSE=1 cmake --build .

test:
	@if [ -f ./$(BUILD_DIR)/test ]; then ./$(BUILD_DIR)/test ${ARGS}; else echo "Please run 'make' or 'make release' first" && exit 1; fi

bench:
	@if [ -f ./$(BUILD_DIR)/bench ]; then ./$(BUILD_DIR)/bench; else echo "Please run 'make' or 'make release' first" && exit 1; fi

clean:
	rm -rf ./$(BUILD_DIR)

.PHONY: test bench
