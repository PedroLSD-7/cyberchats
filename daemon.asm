
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
    buf resb 64        ; buffer para comandos e respostas
    valor resq 1      ; valor armazenado no cofre
    fd_cmd resq 1     ; fd do FIFO de comandos
    fd_resp resq 1    ; fd do FIFO de respostas

section .text
    global _start

_start:
    ; Inicializa o daemon abrindo os FIFOs e zerando o cofre

    mov rax,2
    lea rdi,[fifo_cmd]
    mov rsi,0
    syscall
    mov [fd_cmd],rax

    mov rax,2
    lea rdi,[fifo_resp]
    mov rsi,1
    syscall
    mov [fd_resp],rax

    mov qword [valor],0

.loop:
    ; Loop principal: lê comandos e executa ações

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
    ; Abre o cofre (apenas envia mensagem)
    call w_open
    jmp .loop

.inserir:
    ; Insere um valor numérico no cofre
    mov al,[buf+2]
    sub al,'0'
    movzx rax,al
    mov [valor],rax
    call w_valor
    jmp .loop

.consultar:
    ; Consulta o valor do cofre
    mov rax,[valor]
    cmp rax,0
    je .vazio
    call w_valor
    jmp .loop

.vazio:
    ; Informa que o cofre está vazio
    call w_empty
    jmp .loop

.esvaziar:
    ; Esvazia o cofre
    mov qword [valor],0
    call w_clear
    jmp .loop

.sair:
    ; Encerra o daemon
    call w_exit
    mov rax,60
    xor rdi,rdi
    syscall

; funções de escrita no FIFO 

w_open:
    ; Envia mensagem de cofre aberto
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_open]
    mov rdx,len_open
    syscall
    ret

w_empty:
    ; Envia mensagem de cofre vazio
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_empty]
    mov rdx,len_empty
    syscall
    ret

w_clear:
    ; Envia mensagem de cofre esvaziado
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_clear]
    mov rdx,len_clear
    syscall
    ret

w_exit:
    ; Envia mensagem de encerramento
    mov rax,1
    mov rdi,[fd_resp]
    lea rsi,[msg_exit]
    mov rdx,len_exit
    syscall
    ret

w_valor:
    ; Envia o valor armazenado no cofre
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
