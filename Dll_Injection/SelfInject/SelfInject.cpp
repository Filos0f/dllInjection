// SelfInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

extern "C" DWORD shellcode_size;
extern "C" DWORD shellcode_addr;
//extern "C" CHAR dll_path[255];
//extern "C" DWORD getProcAddress_RVA;
//extern "C" DWORD threadId;

//kernel32_base			dq 0
//loadLibraryAddr			dq 0
//dll_path			db 255 dup(0) - 
//shellcode_size		dd shellcode_size - shellcode_x64 - 



void create_child_proccess(LPTSTR file_name, PROCESS_INFORMATION &pi) {

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		file_name,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_SUSPENDED,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

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

void add_variables_to_asm() {
	LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (loadLibAddr == NULL)
	{
		printf("Error: GetProcAddress. Error:%d\n", GetLastError());
	}
}

void inject_shellcode(HANDLE hProcess)
{
	int i = 0xCC;
	int* p = (int*)VirtualAllocEx(hProcess, NULL, shellcode_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!p) {
		printf("VirtualAllocEx failed \n");
		return;
	}
	printf("VirtualAllocEx: %p \n", p);
	if (!WriteProcessMemory(hProcess, p, (LPCVOID)shellcode_addr, sizeof(int), NULL)) {
		printf("VirtualAllocEx failed \n");
		return;
	}
	create_remote_thread(hProcess, p);
}



int main()
{

	printf("sdfsd");

	LPTSTR exe_file_name = (LPTSTR)"D:\\Innopolis\\ASP\\projects\\dllInjection\\Dll_Injection\\Debug\\TestProc.exe"; // TO DO path with spaces
																		 //char dll_name[] = "D:\\Innopolis\\Advanced System Programming\\projects\\dllInjection\\Dll_Injection\\Release\\InjectedDll.dll"; // TO DO path with spaces
	char dll_path[255] = "D:\\Innopolis\\ASP\\projects\\dllInjection\\Dll_Injection\\Release\\InjectedDll.dll"; // TO DO path with spaces

	// proccess where we would like to inject
	PROCESS_INFORMATION pi;
	create_child_proccess(exe_file_name, pi);
	 
	// inject
	//add_variables_to_asm();
	inject_shellcode(pi.hProcess);

	// close process and thread hendles
	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

