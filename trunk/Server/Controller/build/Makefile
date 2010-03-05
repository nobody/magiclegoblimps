CC = g++

LIBS = -lboost_system -lboost_thread -lmysqlcppconn
LIBDIRS = -L ../lib

INCLUDES = -I ../include 

DEBUG = -g3


OBJS = controller.o \
	   DbManager.o \
	   main.o \
	   RobotHandler.o \
	   Robot.o \
	   TcpServer.o \
	   AdminHandler.o


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
