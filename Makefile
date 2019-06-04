sinclude ./Makefile.inc

COMMAND = $(CXX)

CXXFLAGS_EXTERN = -pthread
LINKFLAGS_EXTERN = -pthread -static

INCLUDE = -I. -I../
LIBRARY = -I. -L../libevwork -levwork -lev -lboost_thread-mt -lboost_system-mt

TARGET   = portmap
TARGET_R = portmap_r

SRC_FILES   = $(wildcard *.cpp)

sinclude ./Makefile.rules
