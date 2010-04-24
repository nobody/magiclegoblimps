#include "robotLock.h"
unsigned int robotLock::index = 0;

robotLock::robotLock(){

	//create the new semaphore object
	lockHandle = CreateSemaphore(NULL, 1, 1, NULL);
}

robotLock::~robotLock(){
	CloseHandle(lockHandle);
}

void robotLock::lock(){
	DWORD err;

	//wait for the semaphore for an infinite number of miliseconds
	err = WaitForSingleObject(lockHandle, INFINITE);
}
void robotLock::unlock(){
	BOOL err;

	//release the semaphore, increase it by one, we don't need to know what it was
	err = ReleaseSemaphore(lockHandle, 1, NULL);
}


