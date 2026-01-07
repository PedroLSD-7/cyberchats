# Execução – Cofre em Assembly + C (Linux)
## Descrição

Este projeto implementa um sistema de cofre utilizando:

Assembly (NASM x86_64) → lógica do cofre (daemon)

Linguagem C → interface de usuário

A comunicação ocorre via FIFO (named pipe).

## Requisitos do Sistema

 * Sistema operacional Linux (Ubuntu, Debian, WSL, etc.)

 * Arquitetura 64 bits

 * Ferramentas instaladas:

nasm

gcc

ld

make (opcional)

## Instalação dos Requisitos

Ubuntu / Debian / WSL:

sudo apt update

sudo apt install nasm gcc build-essential


## Verificação:

nasm -v

gcc --version

## Estrutura do Projeto

Após extrair o ZIP:

cyberchats/
├── daemon.asm    # Código Assembly (daemon do cofre)
├── cliente.c     # Interface em C

## Compilação do Projeto

Entre na pasta do projeto:

cd cyberchats

1 - Compilar o daemon (Assembly)

nasm -f elf64 daemon.asm

ld daemon.o -o daemon

2 - Compilar o cliente (C)

gcc cliente.c -o cliente

## Execução

1 - Iniciar o daemon (em background)

./daemon &

O terminal mostrará o PID do daemon.

2 - Executar o cliente

./cliente

## Menu do Sistema

1 - Abrir cofre

2 - Inserir valor (0 a 9)

3 - Consultar valor

4 - Esvaziar cofre

5 - Encerrar daemon

## Observações:

O cofre aceita valores de 0 a 9

Se nenhum valor for inserido, o cofre retorna 0

As operações funcionam em sequência, sem reiniciar o programa

## Encerrar o Sistema

Use a opção:

5 - Encerrar daemon

Ou, em último caso:

kill <PID>

## Limpeza (Opcional)

Para apagar arquivos gerados:

rm -f daemon daemon.o cliente cofre_fifo

## Problemas Comuns

 “Erro ao abrir FIFO”

*  Certifique-se de que o daemon está rodando antes do cliente

 “command not found”

*  Verifique se está na pasta correta:

pwd

## Conclusão

* Projeto portátil

* Funciona em qualquer Linux 64 bits

* Comunicação entre C e Assembly

* Execução contínua sem reinicialização


### autores
- Pedro Luis Silva Dias
- Thiago Henrique Garcia Da Paz
