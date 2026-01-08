#include "banco.h"

int encontrar_daemon() {
    FILE *f = fopen("daemon.pid", "r");
    if (f) {
        if (fscanf(f, "%d", &daemon_pid) == 1) {
            fclose(f);
            if (kill(daemon_pid, 0) == 0) {
                return 1;
            }
        }
        fclose(f);
    }

    FILE *ps = popen("ps aux | grep './daemon' | grep -v grep | head -1", "r");
    if (ps) {
        char linha[256];
        if (fgets(linha, sizeof(linha), ps)) {
            char *token = strtok(linha, " ");
            token = strtok(NULL, " ");
            if (token) {
                daemon_pid = atoi(token);
                pclose(ps);
                return 1;
            }
        }
        pclose(ps);
    }

    return 0;
}

void registrar_usuario() {
    limpar_tela();
    mostrar_cabecalho();
    printf(AZUL "\n          CADASTRO DE NOVO USUARIO\n");
    printf("\n\n" RESET);

    printf("Usuarios cadastrados: %d / %d\n\n", total_usuarios, MAX_USUARIOS);

    if (total_usuarios >= MAX_USUARIOS) {
        printf(VERMELHO "Limite maximo de usuarios atingido\n" RESET);
        pausar();
        return;
    }

    char nome[MAX_NOME], senha[MAX_SENHA], confirma[MAX_SENHA];

    printf("Nome de usuario: ");
    if (fgets(nome, MAX_NOME, stdin) == NULL) {
        printf(VERMELHO "\nErro na leitura do nome\n" RESET);
        pausar();
        return;
    }
    nome[strcspn(nome, "\n")] = 0;

    if (strlen(nome) < 3) {
        printf(VERMELHO "\nNome deve ter pelo menos 3 caracteres\n" RESET);
        pausar();
        return;
    }

    if (usuario_existe(nome)) {
        printf(VERMELHO "\nUsuario ja existe\n" RESET);
        pausar();
        return;
    }

    printf("Senha: ");
    if (fgets(senha, MAX_SENHA, stdin) == NULL) {
        printf(VERMELHO "\nErro na leitura da senha\n" RESET);
        pausar();
        return;
    }
    senha[strcspn(senha, "\n")] = 0;

    if (strlen(senha) < 6) {
        printf(VERMELHO "\nSenha deve ter pelo menos 6 caracteres\n" RESET);
        pausar();
        return;
    }

    printf("Confirmar senha: ");
    if (fgets(confirma, MAX_SENHA, stdin) == NULL) {
        printf(VERMELHO "\nErro na confirmacao da senha\n" RESET);
        pausar();
        return;
    }
    confirma[strcspn(confirma, "\n")] = 0;

    if (strcmp(senha, confirma) != 0) {
        printf(VERMELHO "\nSenhas nao conferem\n" RESET);
        pausar();
        return;
    }

    // SALDO DE R$ 30.000,00
    strcpy(usuarios[total_usuarios].nome, nome);
    strcpy(usuarios[total_usuarios].senha, senha);
    usuarios[total_usuarios].saldo = 30000.00;
    usuarios[total_usuarios].deposito_diario = 0.0;
    usuarios[total_usuarios].ultimo_deposito = time(NULL);
    total_usuarios++;

    salvar_usuarios();
    enviar_sinal(SIGUSR1);

    printf(VERDE "\nUsuario registrado com sucesso\n" RESET);
    printf(AMARELO "Saldo inicial: R$ 30.000,00\n" RESET);
    pausar();
}

void fazer_login() {
    limpar_tela();
    mostrar_cabecalho();
    printf(AZUL "\n                   LOGIN\n");
    printf("\n\n" RESET);

    if (total_usuarios == 0) {
        printf(VERMELHO "Nenhum usuario cadastrado!\n" RESET);
        printf("Registre-se primeiro.\n");
        pausar();
        return;
    }

    char nome[MAX_NOME], senha[MAX_SENHA];
    int tentativas = 3;

    while (tentativas > 0) {
        printf("Usuario: ");
        if (fgets(nome, MAX_NOME, stdin) == NULL) {
            printf(VERMELHO "\nErro na leitura\n" RESET);
            pausar();
            return;
        }
        nome[strcspn(nome, "\n")] = 0;

        if (strlen(nome) == 0) {
            printf(VERMELHO "\nNome de usuario nao pode ser vazio\n" RESET);
            tentativas--;
            if (tentativas > 0) {
                printf(AMARELO "Tentativas restantes: %d\n" RESET, tentativas);
                sleep(1);
                limpar_tela();
                mostrar_cabecalho();
                printf(AZUL "\n                   LOGIN\n");
                printf("\n\n" RESET);
            }
            continue;
        }

        printf("Senha: ");
        if (fgets(senha, MAX_SENHA, stdin) == NULL) {
            printf(VERMELHO "\nErro na leitura\n" RESET);
            pausar();
            return;
        }
        senha[strcspn(senha, "\n")] = 0;

        if (strlen(senha) == 0) {
            printf(VERMELHO "\nSenha nao pode ser vazia\n" RESET);
            tentativas--;
            if (tentativas > 0) {
                printf(AMARELO "Tentativas restantes: %d\n" RESET, tentativas);
                sleep(1);
                limpar_tela();
                mostrar_cabecalho();
                printf(AZUL "\n                   LOGIN\n");
                printf("\n\n" RESET);
            }
            continue;
        }

        int encontrado = 0;
        for (int i = 0; i < total_usuarios; i++) {
            if (strcmp(usuarios[i].nome, nome) == 0 &&
                strcmp(usuarios[i].senha, senha) == 0) {

                usuario_atual = usuarios[i];
                logado = 1;
                encontrado = 1;

                resetar_deposito_diario(&usuario_atual);
                enviar_sinal(SIGUSR1);

                printf(VERDE "\nLogin realizado com sucesso\n" RESET);
                printf(AMARELO "Bem-vindo, %s\n" RESET, nome);
                pausar();
                return;
            }
        }

        if (!encontrado) {
            tentativas--;
            printf(VERMELHO "\nCredenciais incorretas\n" RESET);
            if (tentativas > 0) {
                printf(AMARELO "Tentativas restantes: %d\n" RESET, tentativas);
                sleep(2);
                limpar_tela();
                mostrar_cabecalho();
                printf(AZUL "\n                   LOGIN\n");
                printf("\n\n" RESET);
            }
        }
    }

    printf(VERMELHO "\nAcesso bloqueado por seguranca\n" RESET);
    pausar();
}

void fazer_deposito() {
    limpar_tela();
    mostrar_cabecalho();
    printf(AZUL "\n                    DEPOSITO\n");
    printf("\n\n" RESET);

    resetar_deposito_diario(&usuario_atual);

    float valor;
    printf("Valor para deposito: R$ ");
    if (scanf("%f", &valor) != 1) {
        printf(VERMELHO "\nValor invalido\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();

    if (valor <= 0) {
        printf(VERMELHO "\nValor deve ser positivo\n" RESET);
    } else if (valor > MAX_DEPOSITO) {
        printf(VERMELHO "\nLimite de deposito excedido\n" RESET);
        printf("Limite por transacao: R$ %.2f\n", MAX_DEPOSITO);
    } else if (usuario_atual.deposito_diario + valor > MAX_DEPOSITO_DIARIO) {
        printf(VERMELHO "\nLimite diario de deposito excedido\n" RESET);
        printf("Depositado hoje: R$ %.2f\n", usuario_atual.deposito_diario);
        printf("Limite diario: R$ %.2f\n", MAX_DEPOSITO_DIARIO);
    } else {
        enviar_sinal(SIGUSR2);

        usuario_atual.saldo += valor;
        usuario_atual.deposito_diario += valor;
        usuario_atual.ultimo_deposito = time(NULL);

        for (int i = 0; i < total_usuarios; i++) {
            if (strcmp(usuarios[i].nome, usuario_atual.nome) == 0) {
                usuarios[i] = usuario_atual;
                break;
            }
        }

        salvar_usuarios();

        printf(VERDE "\nDeposito realizado com sucesso\n" RESET);
        printf(VERDE "Valor depositado: R$ %.2f\n", valor);
        printf(VERDE "Novo saldo: R$ %.2f\n", usuario_atual.saldo);
        printf(AMARELO "Depositado hoje: R$ %.2f\n" RESET, usuario_atual.deposito_diario);
    }

    pausar();
}

void fazer_saque() {
    limpar_tela();
    mostrar_cabecalho();
    printf(AZUL "\n                     SAQUE\n");
    printf("\n\n" RESET);

    float valor;
    printf("Valor para saque: R$ ");
    if (scanf("%f", &valor) != 1) {
        printf(VERMELHO "\nValor invalido\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();

    if (valor <= 0) {
        printf(VERMELHO "\nValor deve ser positivo\n" RESET);
    } else if (valor > MAX_SAQUE) {
        printf(VERMELHO "\nLimite de saque excedido\n" RESET);
        printf("Limite por transacao: R$ %.2f\n", MAX_SAQUE);
    } else if (valor > usuario_atual.saldo) {
        printf(VERMELHO "\nSaldo insuficiente\n" RESET);
        printf("Saldo disponivel: R$ %.2f\n", usuario_atual.saldo);
    } else {
        enviar_sinal(SIGUSR2);

        usuario_atual.saldo -= valor;

        for (int i = 0; i < total_usuarios; i++) {
            if (strcmp(usuarios[i].nome, usuario_atual.nome) == 0) {
                usuarios[i] = usuario_atual;
                break;
            }
        }

        salvar_usuarios();

        printf(VERDE "\nSaque realizado com sucesso\n" RESET);
        printf(VERDE "Valor sacado: R$ %.2f\n", valor);
        printf(VERDE "Novo saldo: R$ %.2f\n", usuario_atual.saldo);
    }

    pausar();
}

