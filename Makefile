.PHONY: build test

build:
	@echo "Building mora"
	@cmake -B build && cmake --build build

test: build
	@echo "Running tests"
	@ctest --test-dir build --output-on-failure
