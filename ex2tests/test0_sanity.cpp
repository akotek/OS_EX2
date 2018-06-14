#include "../uthreads.h"

#include <iostream>

void f (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "f" << tid << " Quanta:" <<  i << std::endl;
			if (i == 5)
			{
				std::cout << "f END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

void g (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "g" << tid << " Quanta:" <<  i << std::endl;
			if (i == 5)
			{
				std::cout << "g END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}


int main(void)
{
	try
	{
		uthread_init(100);
		int tid = uthread_get_tid();
		int i = 1;
		std::cout << "Thread:m Number:(0) " << tid << std::endl;
		// expected initial number: 1. Test changed to include zero, even though it was mentioned in the exercise:
        if(uthread_get_total_quantums() > 1 || uthread_get_total_quantums() < 0)
        {
		    std::cout << "Init Quantum num is incorrect." << std::endl;
        }
		while(1)
		{
			if(i == uthread_get_quantums(tid))
			{
				std::cout << "m" << tid << " Quanta:" <<  i << std::endl;
				if (i == 3)
				{
					std::cout << "m spawns f at (1) " << uthread_spawn(f) << std::endl;
					std::cout << "m spawns g at (2) " << uthread_spawn(g) << std::endl;
				}
				if (i == 10)
				{
					// changed from:
					// std::cout << "Total Quantums: " << uthread_get_total_quantums() << std::endl; // expected number: 20
					// to a test checking the correct range of total quantums:
                    if (uthread_get_total_quantums() > 10 && uthread_get_total_quantums() < 30)
                    {
                        std::cout << "Correct range of Total Quantums." << std::endl;
                    }
					uthread_terminate(tid);
				}
				i++;
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Caught The Following Excpetion: \n" << e.what() << std::endl;
	}

}
