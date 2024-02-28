#include "parallel_scheduler.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>


void exampleFunction(int id) 
{
	std::cout << "Function " << id << " is executed by thread " << pthread_self() << std::endl;
	sleep(1);
}

int main() 
{

	const size_t poolCapacity = 3;
	parallel_scheduler scheduler(poolCapacity);

	for (int i = 0; i < 10; ++i) 
	{
		scheduler.run([i] { exampleFunction(i); });
	}
	sleep(5);

	return 0;
}

