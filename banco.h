#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_USUARIOS 50
#define MAX_NOME 30
#define MAX_SENHA 20
#define MAX_DEPOSITO 10000.00
#define MAX_SAQUE 5000.00
#define MAX_DEPOSITO_DIARIO 20000.00

#define VERMELHO "\033[1;31m"
#define VERDE "\033[1;32m"
#define AMARELO "\033[1;33m"
#define AZUL "\033[1;34m"
#define CIANO "\033[1;36m"
#define RESET "\033[0m"

typedef struct {
    char nome[MAX_NOME];
    char senha[MAX_SENHA];
    float saldo;
    float deposito_diario;
    time_t ultimo_deposito;
} Usuario;

extern Usuario usuarios[MAX_USUARIOS];
extern int total_usuarios;
extern Usuario usuario_atual;
extern int logado;
extern pid_t daemon_pid;

// Funções básicas
void limpar_buffer();
void limpar_tela();
void pausar();
void mostrar_cabecalho();

// Usuários
void carregar_usuarios();
void salvar_usuarios();
int usuario_existe(const char *nome);
void resetar_deposito_diario(Usuario *user);

// Operações
void registrar_usuario();
void fazer_login();
void consultar_saldo();
void fazer_deposito();
void fazer_saque();

// Menus
void menu_principal();
void tela_inicial();

// Daemon
int encontrar_daemon();
void enviar_sinal(int sig);

#endif
