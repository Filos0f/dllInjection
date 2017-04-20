.386 
.model flat, stdcall 
;.data 
;test_data		dd 3
;shellcode_addr  dd test_fun

.code 
shellcode_start_addr dd 0
;test_d0 dd 88888888h
;test_d1 dd 88888888h
;test_d2 dd 88888888h
;test_d3 dd 88888888h
test_fun PROC 
	;mov eax, test_data
	add eax, 1	
	mov eax, ecx	 
	;mov test_data, eax
	ret
test_fun ENDP
test_d4 dd 88888888h
test_d5 dd 88888888h
test_d6 dd 88888888h
test_d7 dd 88888888h

shellcode_end_addr	dd shellcode_end_addr
shellcode_s dd shellcode_s - shellcode_start_addr


PUBLIC shellcode_start_addr
PUBLIC shellcode_end_addr 
PUBLIC shellcode_s
END

 