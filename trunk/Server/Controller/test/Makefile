CC = g++

LIBS = -lmysqlcppconn -lboost_system -lboost_thread
LIBDIRS = -L ../lib

INCLUDES = -I ../include 

DEBUG = -g3


all: AdminClient


AdminClient: AdminClient.cpp
	@echo "Building: $@"
	${CC} -o"$@" ${DEBUG} ${LIBDIRS} ${LIBS} ${INCLUDES} $@.cpp
	@echo "Done building: $@"

