#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>

#include <Psapi.h>

#include "LoadLibraryR.h"

#define BREAK_WITH_ERROR( e ) { printf( "[-] %s. Error=%d", e, GetLastError() ); break; }

bool InjectDynamicLibrary2(DWORD processId, char* dllPath)
{
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD |
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_OPERATION |
		PROCESS_VM_WRITE |
		PROCESS_VM_READ,
		FALSE,
		processId);

	HANDLE hFile = CreateFileA(dllPath,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dllFileLength = GetFileSize(hFile,
		NULL);

	LPVOID remoteDllAddr = VirtualAllocEx(hProcess,
		NULL,
		dllFileLength,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);

	LPVOID lpBuffer = HeapAlloc(GetProcessHeap(),
		0,
		dllFileLength);

	DWORD dwBytesRead;

	ReadFile(hFile,
		lpBuffer,
		dllFileLength,
		&dwBytesRead,
		NULL);

	LPVOID lpRemoteLibraryBuffer = NULL;

	WriteProcessMemory(hProcess,
		lpRemoteLibraryBuffer,
		lpBuffer,
		dllFileLength,
		NULL);

	LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");

	DWORD dwLength = 0;
	HANDLE hModule = LoadRemoteLibraryR(hProcess, lpBuffer, dwLength, NULL);

	WaitForSingleObject(hModule, -1);

	HeapFree(GetProcessHeap(), 0, lpBuffer);
	CloseHandle(hProcess);

	return true;
}

bool InjectDynamicLibrary(DWORD processId, char* dllPath)
{
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
	if (hTargetProcess)
	{
		LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		if (loadLibAddr == NULL)
		{
			printf("Error: GetProcAddress. Error:%d\n", GetLastError());
		}
		LPVOID loadPath = VirtualAllocEx(hTargetProcess, 0, strlen(dllPath),
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (loadPath == NULL)
		{
			printf("Error: VirtualAllocEx. Error:%d\n", GetLastError());
			return false;
		}

		if (!WriteProcessMemory(hTargetProcess, loadPath, dllPath, strlen(dllPath), NULL))
		{
			printf("Error: WriteProcessMemory. Error:%d\n", GetLastError());
			return false;
		}

		HANDLE remoteThreadID = CreateRemoteThread(hTargetProcess, 0, 0,
			(LPTHREAD_START_ROUTINE)LoadLibraryA, loadPath, 0, 0);

		//HANDLE remoteThreadID = CreateRemoteThread(hTargetProcess, 0, 0,
		//	(LPTHREAD_START_ROUTINE)LoadLibraryA, loadPath, 0, 0);
		if (remoteThreadID == NULL) 
		{
			printf("Error: the remote thread could not be created. Error:%d\n", GetLastError());
		}
		else 
		{
			printf("Success: the remote thread was successfully created.\n");
		}
		if (WaitForSingleObject(remoteThreadID, INFINITE) == WAIT_FAILED)
		{
			printf("Error: WaitForSingleObject. Error:%d\n", GetLastError());
		}
		DWORD code;
		GetExitCodeThread(remoteThreadID, &code);
		printf("%d", code);

		VirtualFreeEx(hTargetProcess, loadPath, strlen(dllPath), MEM_RELEASE);
		CloseHandle(remoteThreadID);
		CloseHandle(hTargetProcess);
		return true;
	}
	return false;
}

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



int main()
{
	//FindProcessId(L"calc.exe")
	// Taskmgr.exe
	InjectDynamicLibrary(FindProcessId(L"chrome.exe"), "C:\\Git_Projects2\\dllInjection\\Dll_Injection\\Debug\\InjectedDll.dll");

	return 0;
}