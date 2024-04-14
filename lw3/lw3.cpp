#pragma comment (lib, "winmm.lib")
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

vector<string> ou;
DWORD startTime = 0;

struct Thread
{
	size_t threadNum;
};

void WriteToFile(std::string filename, std::string data)
{
	std::ofstream file;
	file.open(filename, std::ios_base::app);
	if (file.is_open())
	{
		file << data << std::endl;
		file.close();
	}
	else
	{
		std::cout << "Ошибка при открытии файла " << filename << std::endl;
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpvoid)
{
	Thread* data = (Thread*)lpvoid;

	int a = 0;

	for (size_t k = 0; k < 20; k++)
	{
		for (size_t i = 0; i < 1000; i++)
		{
			for (size_t j = 0; j < 1000; j++)
			{
				a++;
				a--;
			}
		}
		auto endTime = timeGetTime();
		string s = to_string(data->threadNum) + "|" + to_string(endTime - startTime);
		WriteToFile("output.txt", s);
	}

	ExitThread(0);
}

int main(int argc, CHAR* argv[])
{
	char a;
	cin >> a;
	startTime = timeGetTime();
	ofstream file;
	file.open("output.txt");
	file.close();

	HANDLE* handles = new HANDLE[2];
	DWORD startTime = timeGetTime();
	for (size_t i = 0; i < 2; i++)
	{
		Thread* data = new Thread();
		data->threadNum = i + 1;

		handles[i] = CreateThread(NULL, 0, &ThreadProc, data, CREATE_SUSPENDED, NULL);
	}

	SetThreadPriority(handles[1], THREAD_PRIORITY_HIGHEST);

	for (size_t i = 0; i < 2; i++)
	{
		if (handles[i] != 0)
			ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(2, handles, TRUE, INFINITE);

	return 0;
}