#include <iostream>
#include <windows.h>
#include <omp.h>

#pragma comment(lib, "winmm.lib")

using namespace std;

int main()
{
	const long num_terms = 100000000;
	double pi = 0.0;

	auto start = timeGetTime();

	for (long i = 0; i < num_terms; ++i)
	{
		if (i % 2 == 0)
		{
			pi += 1.0 / (2 * i + 1);
		}
		else
		{
			pi -= 1.0 / (2 * i + 1);
		}
	}

	/*#pragma omp parallel for
	for (long i = 0; i < num_terms; ++i)
	{
		if (i % 2 == 0)
		{
			pi += 1.0 / (2 * i + 1);
		}
		else
		{
			pi -= 1.0 / (2 * i + 1);
		}
	}

	#pragma omp parallel for
	for (long i = 0; i < num_terms; ++i)
	{
		double term = (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
		#pragma omp atomic
		pi += term;
	}

	#pragma omp parallel for reduction(+:pi)
	for (long i = 0; i < num_terms; ++i)
	{
		double term = (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
		pi += term;
	}*/

	pi *= 4;

	cout << "Result: " << pi << endl;
	cout << "Time: " << (int)(timeGetTime() - start) << endl;

	return 0;
}