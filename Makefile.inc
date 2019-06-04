MAKE = make
CXX  = g++
ARRU = ar -ru
RM   = rm -f
CP   = CP -f
MV   = mv -f

CXXFLAGS    = -Wall -ggdb
CXXFLAGS_R  = -Wall -O3
LINKFLAGS   = -Wl,-rpath,./:../bin
LINKFLAGS_R = -Wl,-rpath,./:../bin

CXXFLAGS_SO  = -fPIC
LINKFLAGS_SO = -shared
CXXFLAGS_XML = -DXML_NULL #-DTIXML_USE_STL

COMMAND  = $(CXX)

CXXFLAGS_EXTERN =
LINKFLAGS_EXTERN =

INCLUDE =
LIBRARY =

TARGET   =
TARGET_R =

SRC_FILES   =

OBJ_FILES   = $(SRC_FILES:.cpp=.o)
OBJ_FILES_R = $(SRC_FILES:.cpp=.ro)
DEP_FILES   = $(SRC_FILES:.cpp=.d)
DEP_FILES_R = $(SRC_FILES:.cpp=.rd)

