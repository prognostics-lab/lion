.PHONY : $(shell egrep "^[A-Za-z0-9_-]+\:([^\=]|$$)" $(lastword $(MAKEFILE_LIST)) | sed -E 's/(.*):.*/\1/g')

all: build

help:
	@echo "Usage"
	@echo "-----"
	@echo "    make <command>"
	@echo
	@echo "Commands"
	@echo "--------"
	@echo "    build   : Default, generates minimal configuration and builds the library"
	@echo "    clean   : Removes all build files and targets"
	@echo "    install : Requires sudo, installs the library files and headers"

clean:
	@echo -e "\x1b[32;20mCleaning working directory\x1b[0m\n"
	@rm -rf \
		./bin \
		./build \
		./lib \
		./pdb \
		./Testing \
		./logs \
		./pysrc/lion/_lion.*;

configure:
	@echo -e "\x1b[32;20mConfiguring\x1b[0m"
	@cmake \
		-S . \
		-B build \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_SHARED_LIBS=ON;

build: configure
	@echo -e "\x1b[32;20mBuilding\x1b[0m"
	@cmake --build build --config Release;

install:
	@if [ $$EUID -ne 0 ]; then \
		echo -e "\x1b[31;20mInstallation requires admin privileges\x1b[0m"; \
	else \
		echo -e "\x1b[32;20mInstalling\x1b[0m"; \
		cmake --install build --prefix "/usr/"; \
	fi
