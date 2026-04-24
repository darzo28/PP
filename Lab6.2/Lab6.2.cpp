#include <iostream>
#include <omp.h>

#pragma comment(lib, "winmm.lib")

int main()
{
	int x = 44;
	#pragma omp parallel for lastprivate(x)
	for (int i = 0; i <= 10; i++)
	{
		printf("x  %d\n", x);
		x = i;
		printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
	}
	printf("x is %d\n", x);
}
