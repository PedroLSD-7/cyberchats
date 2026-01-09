# BANCO VIRTUAL NUCOMP 
## Sistema bancário cliente-daemon desenvolvido em Assembly x86_64 e C, com comunicação via sinais Unix.

compilacao do programe via terminal ubuntu

### apos baixar a pasta com os arquivos no terminal abra a pasta

    cd cyberchats

### Primeiro passo para executar , mate todos os processos antigos caso tenha tentado executar da forma errada

    killall daemon 2>/dev/null
    killall cliente 2>/dev/null

### Limpe os arquivos de compilação caso tenha tentado executar da forma errada


    rm -f *.o daemon cliente daemon.pid daemon.log

### Compile o daemon

    nasm -f elf64 daemon.asm -o daemon.o
    ld -o daemon daemon.o

### Compile o cliente

    gcc -c banco.c -o banco.o
    gcc -c cliente.c -o cliente.o
    gcc -c menus.c -o menus.o
    gcc banco.o cliente.o menus.o -o cliente

gcc banco.o cliente.o menus.o -o cliente

 ### Execute 

    ./daemon &
___

    ./cliente


Teste o Sistema
text
1. Registre um novo usuário
   - Nome: "cliente1"
   - Senha: "senha123"
   - Saldo inicial: R$ 30.000,00

2. Faça login com as credenciais

3. Teste as operações:
   - Consultar saldo
   - Depositar (limite: R$ 10.000,00 por transação)
   - Sacar (limite: R$ 5.000,00 por transação)
   - Logout

TESTES RECOMENDADOS

 
1. Registre usuário "teste1"
2. Faça login
3. Deposite R$ 8.000,00
4. Saque R$ 3.000,00
5. Consulte saldo (deve mostrar R$ 35.000,00)
6. Faça logout
7. Login novamente (saldo deve persistir)

Teste 2: Limites de segurança
1. Tente depositar R$ 15.000,00 
2. Tente sacar R$ 6.000,00 
3. Tente login com senha errada 3x (deve bloquear)

Teste 3: Múltiplos usuários
1. Registre "usuario2"
2. Faça operações
3. Login com "teste1" (saldos devem ser independentes)

 ### FUNCIONALIDADES IMPLEMENTADAS

Sistema de registro com validação

Login com limite de tentativas

Consulta de saldo em tempo real

Depósito com limites por transação

Saque com verificação de saldo

Persistência de dados entre sessões

Logout seguro

### Recursos Técnicos
Comunicação interprocessos via sinais

Máquina de estados no daemon

Logging de todas as operações

Interface colorida no terminal

Validação robusta de entrada

Tratamento de erros gracefull

### REFERÊNCIAS TÉCNICAS

Assembly x86_64

Syscalls Linux: write, fork, exit, kill, signal

Modo de endereçamento: 64-bit

Convenção de chamada: System V AMD64 ABI

Sinais Unix

SIGUSR1/SIGUSR2: Comunicação personalizada

SIGTERM: Finalização limpa

SIGINT: Interrupção do usuário

SIGHUP: Reinicialização

### Estruturas de Dados

Usuário: nome, senha hash, saldo, timestamp

Estado: máquina de estados finitos

Log: timestamp + mensagem

## AUTORES
- Pedro Luis Silva Dias
- Thiago Henrique Garcia Da Paz
