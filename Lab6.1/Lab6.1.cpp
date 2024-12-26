#include <iostream>
#include <functional>
#include <windows.h>
#include <omp.h>

#pragma comment(lib, "winmm.lib")

using namespace std;

using Operation = function<double(long)>;

double syncCalc(long count)
{
	double result = 0.0;
	for (long i = 0; i < count; ++i)
	{
		result += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
	}

	return result;
}

double parallelWrongCalc(long count)
{
	double result = 0.0;

	#pragma omp parallel for
	for (long i = 0; i < count; ++i)
	{
		result += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
	}

	return result;
}

double parallelCorrectCalc(long count)
{
	double result = 0.0;

	#pragma omp parallel for
	for (long i = 0; i < count; ++i)
	{
		double term = (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
		#pragma omp	atomic
		result += term;
	}

	return result;
}

double reductionCalc(long count)
{
	double result = 0.0;

	#pragma omp parallel for reduction(+:result)
	for (long i = 0; i < count; ++i)
	{
		double term = (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
		result += term;
	}

	return result;
}

int main(int argc, char* argv[])
{
	int functionNumber;
	if (argc != 2)
	{
		functionNumber = 1;
	}
	else
	{
		functionNumber = atoi(argv[1]);
	}

	Operation selectedOperation;

	switch (functionNumber)
	{
		case 1:
			selectedOperation = syncCalc;
			break;
		case 2:
			selectedOperation = parallelWrongCalc;
			break;
		case 3:
			selectedOperation = parallelCorrectCalc;
			break;
		case 4:
			selectedOperation = reductionCalc;
			break;
	}

	const long num_terms = 100000000;
	double pi = 0.0;

	auto start = timeGetTime();

	pi = selectedOperation(num_terms) * 4;

	cout << "Result: " << pi << endl;
	cout << "Time: " << (int)(timeGetTime() - start) << endl;

	return 0;
}