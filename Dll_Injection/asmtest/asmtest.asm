.386 
.model flat, stdcall 
;.data 
;test_data		dd 3
;shellcode_addr  dd test_fun

.code 
test_fun PROC 
	mov eax, test_data
	add eax, 1		 
	mov test_data, eax
	ret
test_fun ENDP
test_data		dd 3
shellcode_addr  dd test_fun
shellcode_size	dd shellcode_size - test_fun


PUBLIC test_data
PUBLIC shellcode_size

END

 