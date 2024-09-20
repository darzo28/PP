#include <windows.h>
#include <string>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

HANDLE mutex;

optional<int> ParseArgs(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Неверное количество аргументов" << endl;
		cerr << "Пример: Lab1.exe <количество потоков>" << endl;
		return nullopt;
	}

	try
	{
		auto count = stoi(argv[1]);
		if (count < 1)
		{
			throw out_of_range("Invalid input count");
		}
		return count;
	}
	catch (const logic_error&)
	{
		cerr << "Количество потоков должно быть целым положительным числом" << endl;
		return nullopt;
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	const auto threadNumber = *(int*)lpParam;

	WaitForSingleObject(mutex, INFINITE);
	cout << "Поток №" << threadNumber << " выполняет свою работу" << endl;
	ReleaseMutex(mutex);

	ExitThread(0); // функция устанавливает код завершения потока в 0
}

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1251);
	const auto args = ParseArgs(argc, argv);

	if (!args)
	{
		return 1;
	}

	const auto count = args.value();
	HANDLE* handles = new HANDLE[count];
	int* threadNumbers = new int[count];

	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL) {
		cerr << "Ошибка создания мьютекса: " << GetLastError() << endl;
		return 1;
	}

	// создание потоков
	for (auto i = 0; i < count; i++)
	{
		threadNumbers[i] = i + 1;
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadNumbers[i], CREATE_SUSPENDED, NULL);
		if (handles[i] == NULL) {
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

	for (int i = 0; i < count; i++) {
		CloseHandle(handles[i]);
	}

	CloseHandle(mutex);

	return 0;
}
