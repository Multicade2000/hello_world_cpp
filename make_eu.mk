.PHONY: all cleansub

TARGET = eu/HelloWorldCpp
TYPE = ps-exe

SRCS = main.cpp \
game.cpp \
graph.cpp \
cdrom.cpp \
sound.cpp \
memcard.cpp \
controller.cpp \
player.cpp \
region_eu.cpp \
../third_party/common/syscalls/printf.s \
../third_party/common/crt0/crt0.s \

CPPFLAGS += -I../third_party/common
CPPFLAGS += -I../third_party/psyq/include
CPPFLAGS += -fno-exceptions -fno-rtti
CPPFLAGS += -fno-builtin-memcpy
CPPFLAGS += -fno-builtin-strcpy
LDFLAGS += -L../third_party/psyq/lib
LDFLAGS += -Wl,--start-group
LDFLAGS += -lapi
LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcd
LDFLAGS += -lcard
LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
LDFLAGS += -lgs
LDFLAGS += -lgte
LDFLAGS += -lgpu
LDFLAGS += -lgun
LDFLAGS += -lhmd
LDFLAGS += -lmath
LDFLAGS += -lmcrd
LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
LDFLAGS += -ltap
LDFLAGS += -Wl,--end-group

include ../third_party/common.mk

all:
	mkpsxiso -y eu/iso.xml

cleansub:
	rm -r eu/HelloWorldCpp.* eu/hello_world_cpp_eu.*