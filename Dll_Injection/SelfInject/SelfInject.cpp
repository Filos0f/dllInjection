// DllInjector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

DWORD WINAPI testThreadProc(_In_ LPVOID lpParameter) {
	return (NULL == LoadLibrary("Imagehlp.dll"));
}

void test() {
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)testThreadProc, testThreadProc, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	DWORD code;
	GetExitCodeThread(hThread, &code);
	printf("Thread exited with code %d \n", code);
	CloseHandle(hThread);
}


void create_remote_thread(HANDLE hProcess, int* p) {
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)p, p, 0, NULL);
	if (!hThread) {
		printf("Create remote thread failed \n");
	}
	WaitForSingleObject(hThread, INFINITE);
	DWORD code;
	GetExitCodeThread(hThread, &code);
	printf("Thread exited with code %d \n", code);
	CloseHandle(hThread);
}


void inject(HANDLE hProcess)
{
	int i = 0xCC;
	int* p = (int*)VirtualAllocEx(hProcess, NULL, sizeof(int), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!p) {
		printf("VirtualAllocEx failed \n");
		return;
	}
	printf("VirtualAllocEx: %p \n", p);
	if (!WriteProcessMemory(hProcess, p, &i, sizeof(int), NULL)) {
		printf("VirtualAllocEx failed \n");
		return;
	}
	create_remote_thread(hProcess, p);
}


void create_child_proccess(int argc, char* argv[]) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//if (argc == 3)
	//{
	//	name = argv[2];
	//	//printf("Usage: %s [cmdline]\n", argv[0]);
	//	//return;

	//}

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		argv[0],        // Command line
						//argv[1],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE + 0 * CREATE_SUSPENDED,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// do inject
	inject(pi.hProcess);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


int main(int argc, char* argv[])
{
	if (argc != 1) {
		if (argc < 3) {
			printf("warning: injection into myself \n",
				"hint: DllInjection /run {program}.exe");
			create_child_proccess(argc, argv);
		}
		else {
			if (0 == strcmp("/run", argv[1])) {
				create_child_proccess(argc - 2, &argv[2]);
			}
		}
		printf("Hello, WORLD \n");
		test();
	}
	else {
		printf("Hello, COPY \n");
		Sleep(500000);
	}
	return 0;
}

