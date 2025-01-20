.PHONY : $(MAKECMDGOALS)

config_args :=
build_args :=
ex :=
args :=

build_type := Release


all: build

help:
	@echo "Usage"
	@echo "-----"
	@echo "    make <command>"
	@echo "    make <command> build_type={Release, Debug}"
	@echo
	@echo "Commands"
	@echo "--------"
	@echo "    help        : Shows this help message"
	@echo "    configure   : Generates a minimal configuration"
	@echo "    build       : **DEFAULT**, generates a minimal configuration and builds the library"
	@echo "    clean       : Removes all build files and targets"
	@echo "    docs        : Builds the documentation"
	@echo "    docs-serve  : Serves the documentation for realtime editing"
	@echo "    install     : Requires sudo, installs the library files and headers"
	@echo "    test        : Compiles and runs the tests"
	@echo "    run         : Runs specified executables"
	@echo "    |- run ex=<example> : Runs one of the examples"

clean:
	@echo -e "\x1b[32;20mCleaning working directory\x1b[0m\n"
	@rm -rf \
		./bin \
		./book \
		./docs/html \
		./docs/latex \
		./docs/rtf \
		./docs/man \
		./docs/xml \
		./docs/docbook \
		./docs/sqlite3 \
		./build \
		./lib \
		./pdb \
		./Testing \
		./logs \
		./pysrc/lion/_lion.*;

docs:
	@doxygen
	@moxygen docs/xml -o docs/src/reference/api_reference_%s.md --anchors --groups
	@cd docs; mdbook build

docs-serve:
	@doxygen
	@moxygen docs/xml -o docs/src/reference/api_reference_%s.md --anchors --groups
	@cd docs; mdbook serve --open

configure:
	@echo -e "\x1b[32;20mConfiguring\x1b[0m"
	@cmake \
		-S . \
		-B build \
		$(config_args) \
		-DCMAKE_BUILD_TYPE=$(build_type) \
		-DBUILD_SHARED_LIBS=ON;

build: configure
	@echo -e "\x1b[32;20mBuilding\x1b[0m"
	@cmake --build build --config $(build_type) $(build_args)

test:
	$(eval config_args += -DLION_BUILD_TESTS=ON)
	@$(MAKE) build config_args=$(config_args) build_args=$(build_args) build_type=$(build_type)
	@echo -e "\x1b[32;20mRunning tests\x1b[0m"
	@cd build; ctest -C $(build_type) --output-on-failure

run:
	$(eval config_args += -DLION_BUILD_EXAMPLES=ON)
	@$(MAKE) build config_args=$(config_args) build_args=$(build_args) build_type=$(build_type)
	@if [ -n "$(ex)" ]; then \
		echo -e "Running example \x1b[32;20m$(ex)\x1b[0m with args \x1b[32;20m$(args)\x1b[0m"; \
		if [ "$(build_type)" == "Release" ]; then \
			./bin/ex.$(ex) $(args); \
		else \
			./bin/debug/ex.$(ex) $(args); \
		fi \
	fi

install:
	@if [ $$EUID -ne 0 ]; then \
		echo -e "\x1b[31;20mInstallation requires admin privileges\x1b[0m"; \
	else \
		echo -e "\x1b[32;20mInstalling\x1b[0m"; \
		cmake --install build --prefix "/usr/"; \
	fi
