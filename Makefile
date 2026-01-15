PKG_CONFIG_PATH := /opt/homebrew/lib/pkgconfig
CFLAGS := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags --libs libcjson)

shift: src/main.c parser/*.c formatter/*.c executor/*.c
	gcc src/main.c parser/*.c formatter/*.c executor/*.c -I include $(CFLAGS) -o shift

run: shift
	./shift

config_parser:
	gcc parser/config_parser.c $(CFLAGS) -o config_parser
