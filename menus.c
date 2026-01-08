#include "banco.h"

void menu_principal() {
    int opcao;

    do {
        limpar_tela();
        mostrar_cabecalho();

        printf(AZUL "Usuario: %s\n", usuario_atual.nome);
        printf("Saldo: R$ %.2f\n\n" RESET, usuario_atual.saldo);

        printf("1. Consultar saldo\n");
        printf("2. Realizar deposito\n");
        printf("3. Realizar saque\n");
        printf("4. Ver limites\n");
        printf("5. Sair\n\n");

        printf("Opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf(VERMELHO "\nOpcao invalida\n" RESET);
            limpar_buffer();
            pausar();
            continue;
        }
        limpar_buffer();

        switch(opcao) {
            case 1:
                consultar_saldo();
                break;
            case 2:
                fazer_deposito();
                break;
            case 3:
                fazer_saque();
                break;
            case 4:
                limpar_tela();
                mostrar_cabecalho();
                printf(AZUL "\n                   LIMITES\n");
                printf("\n\n" RESET);
                printf("Deposito por transacao: R$ %.2f\n", MAX_DEPOSITO);
                printf("Saque por transacao: R$ %.2f\n", MAX_SAQUE);
                printf("Deposito diario total: R$ %.2f\n", MAX_DEPOSITO_DIARIO);
                printf("Saldo minimo: R$ 0,00\n");
                printf("Saldo maximo: ilimitado\n");
                pausar();
                break;
            case 5:
                enviar_sinal(SIGUSR1);
                logado = 0;
                printf(VERDE "\nLogout realizado com sucesso\n" RESET);
                sleep(1);
                return;
            default:
                printf(VERMELHO "\nOpcao invalida\n" RESET);
                pausar();
        }
    } while (1);
}

void tela_inicial() {
    int opcao;

    do {
        limpar_tela();
        mostrar_cabecalho();

        if (encontrar_daemon()) {
            printf(VERDE "Daemon ativo - PID: %d\n\n" RESET, daemon_pid);
        } else {
            printf(AMARELO "Daemon nao encontrado - Operacao local\n\n" RESET);
        }

        printf("Usuarios cadastrados: %d\n\n", total_usuarios);

        printf("1. Registrar novo usuario\n");
        printf("2. Fazer login\n");
        printf("3. Sobre o sistema\n");
        printf("4. Sair\n\n");

        printf("Opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf(VERMELHO "\nOpcao invalida\n" RESET);
            limpar_buffer();
            sleep(1);
            continue;
        }
        limpar_buffer();

        switch(opcao) {
            case 1:
                registrar_usuario();
                break;
            case 2:
                fazer_login();
                break;
            case 3:
                limpar_tela();
                mostrar_cabecalho();
                printf(AZUL "\n                   SOBRE\n");
                printf("\n\n" RESET);
                printf("Sistema Bancario NuComp\n");
                printf("Versao: 1.0\n");
                printf("Arquitetura: Cliente-Daemon\n");
                printf("Sinais usados: SIGUSR1 (Registro/Login), SIGUSR2 (Operacoes)\n");
                printf("Limites de seguranca implementados\n");
                pausar();
                break;
            case 4:
                printf("\nObrigado por usar o Banco NuComp\n");
                exit(0);
            default:
                printf(VERMELHO "Opcao invalida\n" RESET);
                sleep(1);
        }
    } while (!logado);
}

int main() {
    carregar_usuarios();

    if (!encontrar_daemon()) {
        printf(AMARELO "\nAVISO: Daemon nao encontrado\n" RESET);
        printf("Execute em outro terminal: ./daemon\n");
        printf("Deseja continuar? (s/n): ");

        char resposta;
        if (scanf(" %c", &resposta) != 1) {
            printf(VERMELHO "Entrada invalida\n" RESET);
            return 1;
        }
        limpar_buffer();

        if (resposta != 's' && resposta != 'S') {
            return 1;
        }
    }

    tela_inicial();

    if (logado) {
        menu_principal();
    }

    return 0;
}
