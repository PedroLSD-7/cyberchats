; daemon.asm — versão correta com 2 FIFOs

section .data
    fifo_cmd  db "fifo_cmd",0
    fifo_resp db "fifo_resp",0

    msg_open  db "Cofre aberto.",10
    len_open  equ $-msg_open

    msg_empty db "Cofre vazio.",10
    len_empty equ $-msg_empty

    msg_clear db "Cofre esvaziado.",10
    len_clear equ $-msg_clear

    msg_exit  db "Encerrando daemon.",10
    len_exit  equ $-msg_exit

    msg_value db "Valor no cofre: ",0

section .bss
    buf resb 64
    valor resq 1
    fd_cmd resq 1
    fd_resp resq 1

section .text
    global _start

_start:
    ; abrir fifo_cmd (leitura)
    mov rax,2
    lea rdi,[fifo_cmd]
    mov rsi,0
    syscall
    mov [fd_cmd],rax

    ; abrir fifo_resp (escrita)
    mov rax,2
    lea rdi,[fifo_resp]
    mov rsi,1
    syscall
    mov [fd_resp],rax

    mov qword [valor],0

.loop:
    mov rax,0
    mov rdi,[fd_cmd]
    lea rsi,[buf]
    mov rdx,64
    syscall

    cmp byte [buf],'1'
    je .abrir
    cmp byte [buf],'2'
    je .inserir
    cmp byte [buf],'3'
    je .consultar
    cmp byte [buf],'4'
    je .esvaziar
    cmp byte [buf],'5'
    je .sair
    jmp .loop

.abrir:
    call w_open
    jmp .loop

.inserir:
    mov al,[buf+2]
    sub al,'0'
    movzx rax,al
    mov [valor],rax
    call w_valor
    jmp .loop

.consultar:
    mov rax,[valor]
    cmp rax,0
    je .vazio
    call w_valor
    jmp .loop

.vazio:
    call w_empty
    jmp .loop

.esvaziar:
    mov qword [valor],0
    call w_clear
    jmp .loop

.sair:
    call w_exit
    mov rax,60
    xor rdi,rdi
    syscall

; -------- writes --------

w_open:
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_open]
    mov rdx,len_open
    syscall
    ret

w_empty:
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_empty]
    mov rdx,len_empty
    syscall
    ret

w_clear:
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_clear]
    mov rdx,len_clear
    syscall
    ret

w_exit:
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_exit]
    mov rdx,len_exit
    syscall
    ret

w_valor:
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_value]
    mov rdx,16
    syscall

    mov rax,[valor]
    add rax,'0'
    mov [buf],al
    mov byte [buf+1],10

    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[buf]
    mov rdx,2
    syscall
    ret

