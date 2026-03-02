.model flat, cdecl

.data
    result dq 0

.code
exampleIntAsm PROC
    ; Set rax to 17 (64-bit register)
    mov rax, 17

    ; Store the result in the result variable
    mov [result], rax

    ret
exampleIntAsm ENDP
