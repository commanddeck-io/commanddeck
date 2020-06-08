SHELL := /bin/bash

upload:
	platformio run -t upload

all: upload
