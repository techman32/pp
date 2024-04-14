#include <iostream>
#include <Windows.h>
#include <string>
#include <tchar.h>
#include <optional>

using namespace std;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    int* param = (int*)(lpParam);
    printf("Thread %d\n", *param);
    ExitThread(0);
}

struct Args
{
    int threadsCount;
};

optional<Args> ParseArgs(int argc, _TCHAR* argv[]);

int _tmain(int argc, _TCHAR* argv[])
{
    auto args = ParseArgs(argc, argv);

    if (!args || args->threadsCount <= 0)
    {
        cout << "incorrect params" << endl;
        return -1;
    }

    HANDLE* handles = new HANDLE[args->threadsCount];

    for (int i = 0; i < args->threadsCount; i++)
    {
        int* newNumber = new int;
        *newNumber = i + 1;
        handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)newNumber, CREATE_SUSPENDED, NULL);
    }

    for (int i = 0; i < args->threadsCount; i++)
    {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(args->threadsCount, handles, TRUE, INFINITE);

    delete[] handles;
    return 0;
}

optional<Args> ParseArgs(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        return nullopt;
    }

    Args result;

    try
    {
        result.threadsCount = stoi(argv[1]);
    }
    catch (...)
    {
        return nullopt;
    }

    return result;
}