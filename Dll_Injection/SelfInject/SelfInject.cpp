// SelfInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

extern "C" DWORD shellcode_start_addr;
extern "C" DWORD shellcode_end_addr;
extern "C" DWORD shellcode_s;
extern "C" DWORD load_lib_addr; 


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
	//HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)p, p, 0, NULL);
	//if (!hThread) {
	//	printf("Create remote thread failed \n");
	//}
	//WaitForSingleObject(hThread, INFINITE);
	//DWORD code;
	//GetExitCodeThread(hThread, &code);
	//printf("Thread exited with code %d \n", code);
	//CloseHandle(hThread);
}

void add_variables_to_asm() {
	LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (loadLibAddr == NULL)
	{
		printf("Error: GetProcAddress. Error:%d\n", GetLastError());
	}
}

//void inject_dll(DWORD kernel32_addr) {
//	LPVOID loadLibAddr = (LPVOID)GetProcAddress((HMODULE)kernel32_addr, "LoadLibraryA");
//	LPVOID test_loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
//
//	if (loadLibAddr != 0) {
//		printf("fail");
//	}
//}


bool inject_dll(HANDLE hprocess, HANDLE hThread, char* dllPath, DWORD kernel32_addr)
{
	HANDLE hTargetProcess = hprocess;
	LPVOID loadLibAddr = (LPVOID)GetProcAddress((HMODULE)kernel32_addr, "LoadLibraryA");
	//LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
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
	if (remoteThreadID == NULL)
	{
		printf("Error: the remote thread could not be created. Error:%d\n", GetLastError());

	}
	else
	{
		printf("Success: the remote thread was successfully created.\n");
	}

	ResumeThread(hThread);

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



void inject_shellcode(HANDLE hProcess, DWORD &kernel32_base_addr)
{

	LPDWORD addr_word = &shellcode_start_addr + 1;
	//SIZE_T shellcode_size = (&shellcode_end_addr - &shellcode_start_addr) - 1;
	SIZE_T shellcode_size = shellcode_s;

	LPVOID addr = (LPVOID)addr_word;
	shellcode_size = 1000;

	int i = 0xCC;
	int* p = (int*)VirtualAllocEx(hProcess, NULL, shellcode_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!p) {
		printf("VirtualAllocEx failed \n");
		return;
	}
	printf("VirtualAllocEx: %p \n", p);


	if (!WriteProcessMemory(hProcess, p, addr, shellcode_size, NULL)) {
		printf("VirtualAllocEx failed \n");
		return;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0x1000, (LPTHREAD_START_ROUTINE)p, NULL, 0, NULL);
	if (!hThread) {
		printf("Create remote thread failed \n");
	}
	WaitForSingleObject(hThread, INFINITE);
	DWORD code;
	GetExitCodeThread(hThread, &code);
	printf("Thread exited with code %d \n", code);
	CloseHandle(hThread);

	kernel32_base_addr = code;

	//create_remote_thread(hProcess, p);
}


int main()
{

	printf("sdfsd");

	LPTSTR exe_file_name = (LPTSTR)"C:\\Users\\Vadim\\Documents\\dllInjection\\Dll_Injection\\Debug\\TestProc.exe"; // TO DO path with spaces
																		 //char dll_name[] = "D:\\Innopolis\\Advanced System Programming\\projects\\dllInjection\\Dll_Injection\\Release\\InjectedDll.dll"; // TO DO path with spaces
	char dll_path[255] = "C:\\Users\\Vadim\\Documents\\dllInjection\\Dll_Injection\\Debug\\InjectedDll.dll"; // TO DO path with spaces

	// proccess where we would like to inject
	PROCESS_INFORMATION pi;
	create_child_proccess(exe_file_name, pi);
	 
	// inject
	//add_variables_to_asm();
	DWORD kernel32_base_addr;
	//LPVOID p_kernel32_addr;

	inject_shellcode(pi.hProcess, kernel32_base_addr);
	inject_dll(pi.hProcess, pi.hThread, dll_path, kernel32_base_addr);

	// close process and thread hendles
	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

