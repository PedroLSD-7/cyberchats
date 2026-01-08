section .data
    pid_file    db 'daemon.pid', 0
    log_file    db 'daemon.log', 0

    ; Mensagens de log
    msg_start   db 'DAEMON INICIADO - SISTEMA BANCARIO', 10
    msg_start_len equ $ - msg_start

    msg_registro db 'REGISTRO: Novo usuario', 10
    msg_registro_len equ $ - msg_registro

    msg_login   db 'LOGIN: Sessao iniciada', 10
    msg_login_len equ $ - msg_login

    msg_logout  db 'LOGOUT: Sessao encerrada', 10
    msg_logout_len equ $ - msg_logout

    msg_deposito db 'DEPOSITO: Operacao bancaria', 10
    msg_deposito_len equ $ - msg_deposito

    msg_saque   db 'SAQUE: Operacao bancaria', 10
    msg_saque_len equ $ - msg_saque

    msg_consulta db 'CONSULTA: Saldo verificado', 10
    msg_consulta_len equ $ - msg_consulta

    msg_erro    db 'ERRO: Operacao invalida', 10
    msg_erro_len equ $ - msg_erro

    msg_limite  db 'ERRO: Limite excedido', 10
    msg_limite_len equ $ - msg_limite

    msg_fim     db 'DAEMON FINALIZADO', 10
    msg_fim_len equ $ - msg_fim

    ; Estados do sistema
    estado db 0  ; 0=inativo, 1=registro, 2=login, 3=operando

section .bss
    fd      resd 1
    buffer  resb 16

section .text
    global _start

; Escreve no log
; rsi = mensagem, rdx = tamanho
write_log:
    push rax
    push rdi
    push rsi
    push rdx

    mov rax, 2          ; sys_open
    mov rdi, log_file
    mov rsi, 0x441      ; O_WRONLY|O_CREAT|O_APPEND
    mov rdx, 0644o
    syscall

    cmp rax, 0
    jl .fim_log

    mov [fd], rax

    ; Escreve mensagem
    mov rax, 1          ; sys_write
    mov rdi, [fd]
    syscall

    ; Fecha arquivo
    mov rax, 3          ; sys_close
    mov rdi, [fd]
    syscall

.fim_log:
    pop rdx
    pop rsi
    pop rdi
    pop rax
    ret

; Converte inteiro para string
; rax = numero, rdi = buffer
int_to_str:
    push rbx
    push rcx
    push rdx

    mov rbx, 10
    xor rcx, rcx

    test rax, rax
    jnz .converter
    mov byte [rdi], '0'
    mov byte [rdi+1], 0
    jmp .fim_conv

.converter:
    xor rdx, rdx
    div rbx
    add dl, '0'
    push rdx
    inc rcx
    test rax, rax
    jnz .converter

    mov rbx, rdi
.pop_loop:
    pop rax
    mov [rbx], al
    inc rbx
    loop .pop_loop
    mov byte [rbx], 0

.fim_conv:
    pop rdx
    pop rcx
    pop rbx
    ret

; Salva PID
save_pid:
    push rax
    push rdi
    push rsi
    push rdx

    ; Obtem PID
    mov rax, 39         ; sys_getpid
    syscall

    ; Converte para string
    mov rdi, buffer
    call int_to_str

    ; Abre arquivo PID
    mov rax, 2
    mov rdi, pid_file
    mov rsi, 0x241      ; O_WRONLY|O_CREAT|O_TRUNC
    mov rdx, 0644o
    syscall

    cmp rax, 0
    jl .fim_pid

    mov [fd], rax

    ; Calcula tamanho da string
    mov rsi, buffer
    xor rdx, rdx
.calcula:
    cmp byte [rsi+rdx], 0
    je .escreve
    inc rdx
    jmp .calcula

.escreve:
    ; Escreve PID
    mov rax, 1
    mov rdi, [fd]
    mov rsi, buffer
    syscall

    ; Fecha
    mov rax, 3
    mov rdi, [fd]
    syscall

.fim_pid:
    pop rdx
    pop rsi
    pop rdi
    pop rax
    ret

; Handler SIGUSR1 - Registro/Login
handler_usr1:
    push rsi
    push rdx

    ; Verifica estado atual
    mov al, [estado]
    cmp al, 0
    je .registro
    cmp al, 2
    je .login
    cmp al, 3
    je .logout

    ; Estado invalido
    mov rsi, msg_erro
    mov rdx, msg_erro_len
    call write_log
    jmp .fim

.registro:
    mov byte [estado], 1
    mov rsi, msg_registro
    mov rdx, msg_registro_len
    call write_log
    jmp .fim

.login:
    mov byte [estado], 3
    mov rsi, msg_login
    mov rdx, msg_login_len
    call write_log
    jmp .fim

.logout:
    mov byte [estado], 2
    mov rsi, msg_logout
    mov rdx, msg_logout_len
    call write_log

.fim:
    pop rdx
    pop rsi
    ret

; Handler SIGUSR2 - Operacoes bancarias
handler_usr2:
    push rsi
    push rdx

    ; Verifica se esta logado
    mov al, [estado]
    cmp al, 3
    jne .erro_estado

    ; Loga operacao (na pratica distinguiria deposito/saque/consulta)
    ; Por simplicidade, loga como operacao generica
    mov rsi, msg_deposito
    mov rdx, msg_deposito_len
    call write_log
    jmp .fim

.erro_estado:
    mov rsi, msg_erro
    mov rdx, msg_erro_len
    call write_log

.fim:
    pop rdx
    pop rsi
    ret

; Handler SIGTERM
handler_term:
    mov rsi, msg_fim
    mov rdx, msg_fim_len
    call write_log

    ; Remove arquivo PID
    mov rax, 87         ; sys_unlink
    mov rdi, pid_file
    syscall

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall

; Handler SIGINT
handler_int:
    mov byte [estado], 0
    ret

; Handler SIGHUP
handler_hup:
    mov byte [estado], 0
    ret

_start:
    ; Inicializa estado
    mov byte [estado], 1  ; Comeca aguardando registro

    ; Fork para virar daemon
    mov rax, 57         ; sys_fork
    syscall

    cmp rax, 0
    jg .pai_sair

    ; Processo filho (daemon)
    mov rax, 112        ; sys_setsid
    syscall

    call save_pid

    ; Log inicial
    mov rsi, msg_start
    mov rdx, msg_start_len
    call write_log

    ; Loop principal
.loop:
    mov rax, 34         ; sys_pause
    syscall
    jmp .loop

.pai_sair:
    mov rax, 60
    xor rdi, rdi
    syscall
