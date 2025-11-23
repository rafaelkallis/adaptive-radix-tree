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

benchmark-mem:
	@if [ -f ./$(BUILD_DIR)/bench-mem ]; then \
		valgrind --tool=massif --massif-out-file=massif.out ./$(BUILD_DIR)/bench-mem 100000 uniform < /dev/null && \
		ms_print massif.out && \
		echo "" && \
		echo "Running zipfian distribution..." && \
		valgrind --tool=massif --massif-out-file=massif.out ./$(BUILD_DIR)/bench-mem 100000 zipf < /dev/null && \
		ms_print massif.out; \
	else \
		echo "Please run 'make' or 'make release' first" && exit 1; \
	fi

clean:
	rm -rf ./$(BUILD_DIR)

.PHONY: test bench benchmark-mem
