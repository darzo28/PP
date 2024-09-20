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
		cerr << "�������� ���������� ����������" << endl;
		cerr << "������: Lab1.exe <���������� �������>" << endl;
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
		cerr << "���������� ������� ������ ���� ����� ������������� ������" << endl;
		return nullopt;
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	const auto threadNumber = *(int*)lpParam;

	WaitForSingleObject(mutex, INFINITE);
	cout << "����� �" << threadNumber << " ��������� ���� ������" << endl;
	ReleaseMutex(mutex);

	ExitThread(0); // ������� ������������� ��� ���������� ������ � 0
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
		cerr << "������ �������� ��������: " << GetLastError() << endl;
		return 1;
	}

	// �������� �������
	for (auto i = 0; i < count; i++)
	{
		threadNumbers[i] = i + 1;
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadNumbers[i], CREATE_SUSPENDED, NULL);
		if (handles[i] == NULL) {
			cerr << "������ �������� ������: " << GetLastError() << endl;
			return 1;
		}
	}

	// ������ �������
	for (auto i = 0; i < count; i++)
	{
		ResumeThread(handles[i]);
	}

	// �������� ��������� ������ �������
	WaitForMultipleObjects(count, handles, true, INFINITE);

	for (int i = 0; i < count; i++) {
		CloseHandle(handles[i]);
	}

	CloseHandle(mutex);

	return 0;
}
