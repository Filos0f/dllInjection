include shellcode.inc

codeseg SEGMENT READ WRITE EXECUTE
shellcode_x64 proc FRAME
	sub     rsp, 28h
.ALLOCSTACK 28h
.ENDPROLOG


;;;;;;;;;;;; Find kernel32 base

    mov rax, qword ptr [rsp+28h] ; in rax - return address to somewhere in kernel32.dll
	and ax, 0f000h
	mov cx, 'ZM'
@@:
	sub rax, 01000h
	cmp cx, word ptr [rax]
	jne	@b
	
	mov qword ptr [kernel32_base], rax

;;;;;;;;;;;;; Find address of GetProcAddress

	xor rax, rax
	mov eax, dword ptr [getProcAddress_RVA]
	add rax, qword ptr [kernel32_base]
	mov qword ptr [getProcAddressAddr], rax

;;;;;;;;;;;; Find address of GetModuleHandle

	mov rcx, qword ptr [kernel32_base]
	lea rdx, qword ptr [getModuleHandleName]
	call qword ptr [getProcAddressAddr]
	mov qword ptr [getModuleHandleAddr], rax

;;;;;;;;;;;; Find address of LoadLibrary

	mov rcx, qword ptr [kernel32_base]
	lea rdx, qword ptr [loadLibraryName]
	call qword ptr [getProcAddressAddr]
	mov qword ptr [loadLibraryAddr], rax

;;;;;;;;;;;; Find address of VirtualProtect

	mov rcx, qword ptr [kernel32_base]
	lea rdx, qword ptr [virtualProtectName]
	call qword ptr [getProcAddressAddr]
	mov qword ptr [virtualProtectAddr], rax

;;;;;;;;;;;; Find address of OpenThread

	mov rcx, qword ptr [kernel32_base]
	lea rdx, qword ptr [openThreadName]
	call qword ptr [getProcAddressAddr]
	mov qword ptr [openThreadAddr], rax

;;;;;;;;;;;; Find address of ResumeThread

	mov rcx, qword ptr [kernel32_base]
	lea rdx, qword ptr [resumeThreadName]
	call qword ptr [getProcAddressAddr]
	mov qword ptr [resumeThreadAddr], rax

;;;;;;;;;;;; Find address of SuspendThread

	mov rcx, qword ptr [kernel32_base]
	lea rdx, qword ptr [suspendThreadName]
	call qword ptr [getProcAddressAddr]
	mov qword ptr [suspendThreadAddr], rax

;;;;;;;;;;; Find entry point of current procces

	xor rcx, rcx
	call qword ptr [getModuleHandleAddr]
	mov rdx, rax
	
	xor rcx, rcx
	mov ecx, dword ptr [rax.IMAGE_DOS_HEADER.e_lfanew]
	add rax, rcx

	xor rcx, rcx
	mov ecx, dword ptr [rax.IMAGE_NT_HEADERS.OptionalHeader.AddressOfEntryPoint]
	add rdx, rcx
	mov qword ptr [entryPointAddr], rdx

;;;;;;;;;;; Change memory protection on entry point

	mov rcx, [entryPointAddr]
	mov rdx, 4 ; bytes count
	mov r8, 040h
	sub esp, 8
	lea r9, [esp+8]
	call qword ptr [virtualProtectAddr]
	add esp, 8

;;;;;;;;;;; Patch entry point

	mov rax, [entryPointAddr]
	mov ecx, [rax]
	mov [oldEpBytes], ecx
	mov ecx, 0FDEB90h
	mov dword ptr [rax], ecx

;;;;;;;;;;; Resume thread

	xor rcx, rcx
	mov ecx, [threadId]
	mov r8, rcx
	mov rcx, 2
	xor rdx, rdx
	call qword ptr [openThreadAddr]
	mov dword ptr [threadHandle], eax
	mov	rcx, rax
	call qword ptr [resumeThreadAddr]


;;;;;;;;;; Load Library

	lea rcx, [dll_path]
	call qword ptr [loadLibraryAddr]
	or rax, 0
	sete byte ptr [result]

;;;;;;;;;; Suspend Thread
	
	;xor rcx, rcx
	;mov ecx, [threadHandle]
	;call qword ptr [suspendThreadAddr]

;;;;;;;;;; Restore entry point

	mov ecx, [oldEpBytes]
	mov rax, [entryPointAddr]
	mov [rax], ecx


;;;;;;;;;; Return result of calling LoadLibrary
	xor rax, rax
	mov	eax, [result]

	add     rsp, 28h
	ret
shellcode_x64 ENDP

kernel32_base			dq 0
getProcAddressAddr		dq 0
loadLibraryAddr			dq 0
getModuleHandleAddr		dq 0
virtualProtectAddr		dq 0
openThreadAddr			dq 0
resumeThreadAddr		dq 0
suspendThreadAddr		dq 0

entryPointAddr			dq 0

getModuleHandleName		db 'GetModuleHandleA', 0
loadLibraryName			db 'LoadLibraryA', 0
virtualProtectName		db 'VirtualProtect', 0
openThreadName			db 'OpenThread', 0
resumeThreadName		db 'ResumeThread', 0
suspendThreadName		db 'SuspendThread', 0

getProcAddress_RVA	dd 0
threadId			dd 0
threadHandle		dd 0
oldEpBytes			dd 0
dll_path			db 255 dup(0)
result				dd 0
shellcode_size		dd shellcode_size - shellcode_x64

codeseg ENDS


PUBLIC getProcAddress_RVA
PUBLIC threadId
PUBLIC dll_path
PUBLIC shellcode_size

END
