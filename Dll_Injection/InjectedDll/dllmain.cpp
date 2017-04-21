// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	/*switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(0, "Hello", "", 0);
	case DLL_THREAD_ATTACH:
		MessageBoxA(0, "Hello", "", 0);
	case DLL_THREAD_DETACH:
		MessageBoxA(0, "Hello", "", 0);
	case DLL_PROCESS_DETACH:
		MessageBoxA(0, "Hello", "", 0);
		break;
	}*/
	//while (true){}
	MessageBoxA(0, "RUNN!", 0, 0);
	//switch (ul_reason_for_call)
	//{
	//case DLL_PROCESS_ATTACH:
	//	MessageBoxA(0, "Hello from Dll! PA", 0, 0);
	//case DLL_THREAD_ATTACH:
	//	MessageBoxA(0, "Hello from Dll! TA", 0, 0);
	//	
	//case DLL_THREAD_DETACH:
	//	MessageBoxA(0, "Hello from Dll! TD", 0, 0);
	//	
	//case DLL_PROCESS_DETACH:
	//	MessageBoxA(0, "Hello from Dll! PD", 0, 0);
	//	
	//	break;
	//}
	return TRUE;
}

