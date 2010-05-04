CC = g++

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
    LIBDIRS = -L ../lib/64
    LIBS = -lboost_system64 -lboost_thread64 -lmysqlcppconn64
else
    LIBDIRS = -L ../lib/32
    LIBS = -lboost_system32 -lboost_thread32 -lmysqlcppconn32
endif

INCLUDES = -I ../include 

DEBUG = -g3


OBJS = controller.o \
	   DbManager.o \
	   main.o \
	   RobotHandler.o \
	   Robot.o \
	   TcpServer.o \
	   AdminHandler.o \
	   VideoHandler.o \
	   DataFile.o \
	   protocol.o \
	   Qos.o \
	   Point.o \
	   Assignment.o \
	   hungarian.o



all: ServerController

-include *.d


ServerController: ${OBJS}
	@echo "Building: $@"
	${CC} -o"$@" ${LIBDIRS} ${LIBS} ${OBJS}
	@echo "Done building: $@"

%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ ${INCLUDES} ${DEBUG} -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

clean:
	${RM} ${OBJS} *.d ServerController
