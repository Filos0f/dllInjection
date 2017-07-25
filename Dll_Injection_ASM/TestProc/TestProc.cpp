// TestProc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <TlHelp32.h>

void ShowExeNameById(DWORD id)
{
	char exeName[512];
	DWORD cchExeName = 512;
	HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, id);

	if (QueryFullProcessImageNameA(process, 0, exeName, &cchExeName))
	{
		std::cout << exeName << "\n";
	}
	else
	{
		std::cerr << "GetModuleFileNameEx() failed: " <<
			GetLastError() << "\n";
	}
	CloseHandle(process);
}

DWORD FindProcessId(wchar_t* processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processSnapshot, &processInfo);
	if (!lstrcmp(processName, processInfo.szExeFile))
	{
		CloseHandle(processSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processSnapshot, &processInfo))
	{
		if (!lstrcmp(processName, processInfo.szExeFile))
		{
			CloseHandle(processSnapshot);
			ShowExeNameById(processInfo.th32ProcessID);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processSnapshot);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	/*char* dllPath = "C:\\Git_Projects2\\dllInjection\\Dll_Injection\\Debug\\InjectedDll.dll";
	HANDLE hCurrentProc = OpenProcess(PROCESS_ALL_ACCESS, 0, FindProcessId(L"TestProc.exe"));
	LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (loadLibAddr == NULL)
	{
		printf("Error: GetProcAddress. Error:%d\n", GetLastError());
	}
	LPVOID loadPath = VirtualAllocEx(hCurrentProc, 0, strlen(dllPath),
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (loadPath == NULL)
	{
		printf("Error: VirtualAllocEx. Error:%d\n", GetLastError());
	}

	if (!WriteProcessMemory(hCurrentProc, loadPath, dllPath, strlen(dllPath), NULL))
	{
		printf("Error: WriteProcessMemory. Error:%d\n", GetLastError());
	}

	HANDLE hThread1 = CreateRemoteThread(hCurrentProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loadPath, 0, 0);
	if (WaitForSingleObject(hThread1, INFINITE) == WAIT_FAILED)
	{
		printf("Error: WaitForSingleObject. Error:%d\n", GetLastError());
	}
*/
	//LoadLibraryA("C:\\Git_Projects2\\dllInjection\\Dll_Injection\\Debug\\InjectedDll.dll");



	while (true){
		printf("test proc");
		Sleep(10000);
	}
	return 0;
}

