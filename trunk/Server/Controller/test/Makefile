CC = g++

LIBS = -lmysqlcppconn -lboost_system -lboost_thread
LIBDIRS = -L ../lib

INCLUDES = -I ../include 

DEBUG = -g3


all: AdminClient VideoClient connection.class testdata RobotController


TESTDATA_DEPS = ../build/DataFile.o ../build/Robot.o ../build/protocol.o

RobotController: RobotController.cpp ../build/protocol.o
	@echo "Building: $@"
	${CC} -o"$@" ${DEBUG} ${LIBDIRS} ${LIBS} ${INCLUDES} -I ../ $@.cpp ../build/protocol.o

testdata: testdata.cpp ${TESTDATA_DEPS}
	@echo "Building: $@"
	${CC} -o"$@" ${DEBUG} ${LIBDIRS} ${LIBS} ${INCLUDES} -I ../ $@.cpp ${TESTDATA_DEPS}

AdminClient: AdminClient.cpp
	@echo "Building: $@"
	${CC} -o"$@" ${DEBUG} ${LIBDIRS} ${LIBS} ${INCLUDES} $@.cpp
	@echo "Done building: $@"

VideoClient: VideoClient.cpp
	@echo "Building: $@"
	${CC} -o"$@" ${DEBUG} ${LIBDIRS} ${LIBS} ${INCLUDES} $@.cpp
	@echo "Done building: $@"

connection.class: connection.java
	javac $<


../build/%.o: ../%.cpp
	@cd ../build && ${MAKE} ${@:../build/%.o=%.o}

clean:
	${RM} AdminClient VideoClient testdata connection.class

.PHONY: clean
