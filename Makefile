ifeq ($(shell uname -s),Linux)
	include Makefile_Linux
else
	include Makefile_Windows
endif
