#include <Windows.h>


class robotLock{
public:
	robotLock();
	~robotLock();
	void lock();
	void unlock();
	static unsigned int index;

private:
	HANDLE lockHandle;
	//TCHAR* name;

};