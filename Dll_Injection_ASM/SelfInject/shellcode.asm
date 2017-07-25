.386 
.model flat, stdcall 
;.data 
;test_data		dd 3
;shellcode_addr  dd test_fun

.stack 100h

.code 
shellcode_start_addr dd 0
;test_d0 dd 88888888h
;test_d1 dd 88888888h
;test_d2 dd 88888888h
;test_d3 dd 88888888h
test_fun PROC 
	;;mov eax, test_data
	;add eax, 1	
	;mov eax, ecx	 
	;;mov test_data, eax

	;ASSUME FS:NOTHING
	;xor ebx, ebx            ;//clear ebx
	;mov ebx, fs:[ 30h ]    ;//get a pointer to the PEB
	;mov ebx, [ ebx + 0Ch ] ;//get PEB->Ldr
	;mov ebx, [ ebx + 14h ] ;//get PEB->Ldr.InMemoryOrderModuleList.Flink(1st entry) **here**       
	;mov ebx, [ ebx ]        ;//get the next entry(2nd entry)
	;mov ebx, [ ebx ]        ;//get the next entry(3rd entry)
	;mov ebx, [ ebx + 10h ] ;//get the 3rd entries base address(kernel32.dll)
	;ASSUME FS:ERROR




	ASSUME FS:NOTHING
	cld ;// clear the direction flag for the loop
	xor edx, edx ;// zero edx
	;;mov edx, [fs:edx+30h] ;// get a pointer to the PEB
	mov edx, fs:[ 30h ]    ;//get a pointer to the PEB
	mov edx, [edx+0Ch] ;// get PEB->Ldr
	mov edx, [edx+14h] ;// get the first module from the InMemoryOrder module list
next_mod:
	mov esi, [edx+28h] ;// get pointer to modules name (unicode string)
	push 24 ;// push down the length we want to check
	pop ecx ;// set ecx to this length for the loop
	xor edi, edi ;// clear edi which will store the hash of the module name
loop_modname:
	xor eax, eax ;// clear eax
	lodsb ;// read in the next byte of the name
	cmp al, 'a' ;// some versions of Windows use lower case module names
	jl not_lowercase
	sub al, 20h ;// if so normalise to uppercase
not_lowercase:
	ror edi, 13 ;// rotate right our hash value
	add edi, eax ;// add the next byte of the name to the hash
	loop loop_modname ;// loop until we have read enough
	cmp edi, 6A4ABC5Bh ;// compare the hash with that of KERNEL32.DLL
	mov ebx, [edx+10h] ;// get this modules base address
	mov edx, [edx] ;// get the next module
	jne next_mod ;// if it doesn't match, process the next module

	mov eax, ebx
	ASSUME FS:ERROR






	ret
test_fun ENDP
test_d4 dd 88888888h
test_d5 dd 88888888h
test_d6 dd 88888888h
test_d7 dd 88888888h

shellcode_end_addr	dd shellcode_end_addr
shellcode_s dw shellcode_s - shellcode_start_addr
load_lib_addr dd 0

PUBLIC shellcode_start_addr
PUBLIC shellcode_end_addr 
PUBLIC shellcode_s
PUBLIC load_lib_addr 
END

 