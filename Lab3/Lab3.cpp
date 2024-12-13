#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

using namespace std;

DWORD start = 0;
const string FILE_NAME = "out.txt";

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	const auto threadNumber = *(int*)lpParam;
	ofstream out;

	for (int i = 0; i < 25; i++)
	{
		out.open(FILE_NAME, ios::app);
		out << threadNumber << "|" << (int)(timeGetTime() - start) << endl;
		out.close();
	}

	ExitThread(0); // функция устанавливает код завершения потока в 0
}

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1251);

	string input;
	cin >> input;

	start = timeGetTime();

	const auto count = 2;
	HANDLE* handles = new HANDLE[count];
	int* threadNumbers = new int[count];

	// создание потоков
	for (auto i = 0; i < count; i++)
	{
		threadNumbers[i] = i + 1;
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadNumbers[i], CREATE_SUSPENDED, NULL);
		if (handles[i] == NULL)
		{
			cerr << "Ошибка создания потока: " << GetLastError() << endl;
			return 1;
		}
	}

	// запуск потоков
	for (auto i = 0; i < count; i++)
	{
		ResumeThread(handles[i]);
	}

	// ожидание окончания работы потоков
	WaitForMultipleObjects(count, handles, true, INFINITE);

	for (int i = 0; i < count; i++)
	{
		CloseHandle(handles[i]);
	}

	return 0;
}
