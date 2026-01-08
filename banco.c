#include "banco.h"

Usuario usuarios[MAX_USUARIOS];
int total_usuarios = 0;
Usuario usuario_atual;
int logado = 0;
pid_t daemon_pid = 0;

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_tela() {
    system("clear");
}

void pausar() {
    printf("\nPressione Enter para continuar");
    limpar_buffer();
}

void mostrar_cabecalho() {
    printf(CIANO "\n");
    printf("               BANCO VIRTUAL NUCOMP\n");
    printf("\n" RESET);
}

void carregar_usuarios() {
    FILE *f = fopen("usuarios.dat", "rb");
    if (!f) {
        total_usuarios = 0;
        return;
    }

    size_t lidos = fread(&total_usuarios, sizeof(int), 1, f);
    if (lidos != 1 || total_usuarios < 0 || total_usuarios > MAX_USUARIOS) {
        total_usuarios = 0;
        fclose(f);
        return;
    }

    for (int i = 0; i < total_usuarios; i++) {
        lidos = fread(&usuarios[i], sizeof(Usuario), 1, f);
        if (lidos != 1) {
            total_usuarios = i;
            break;
        }
    }

    fclose(f);
}

void salvar_usuarios() {
    FILE *f = fopen("usuarios.dat", "wb");
    if (!f) {
        printf(VERMELHO "ERRO: Nao foi possivel salvar usuarios\n" RESET);
        return;
    }

    fwrite(&total_usuarios, sizeof(int), 1, f);
    for (int i = 0; i < total_usuarios; i++) {
        fwrite(&usuarios[i], sizeof(Usuario), 1, f);
    }

    fclose(f);
}

int usuario_existe(const char *nome) {
    if (strlen(nome) == 0) return 0;

    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            return 1;
        }
    }
    return 0;
}

void resetar_deposito_diario(Usuario *user) {
    time_t agora = time(NULL);
    struct tm *info = localtime(&agora);
    struct tm *ultimo = localtime(&user->ultimo_deposito);

    if (info->tm_yday != ultimo->tm_yday || 
        info->tm_year != ultimo->tm_year) {
        user->deposito_diario = 0.0;
        user->ultimo_deposito = agora;
    }
}

void enviar_sinal(int sig) {
    if (daemon_pid == 0) {
        if (!encontrar_daemon()) {
            printf(VERMELHO "AVISO: Daemon nao encontrado\n" RESET);
            return;
        }
    }

    if (kill(daemon_pid, sig) == 0) {
        usleep(100000);
    }
}

void consultar_saldo() {
    limpar_tela();
    mostrar_cabecalho();
    printf(AZUL "\n               CONSULTA DE SALDO\n");
    printf("\n\n" RESET);

    enviar_sinal(SIGUSR2);

    printf(VERDE "Usuario: %s\n", usuario_atual.nome);
    printf("Saldo atual: R$ %.2f\n\n" RESET, usuario_atual.saldo);

    printf(AMARELO "Limites de operacao:\n" RESET);
    printf("- Deposito por transacao: R$ %.2f\n", MAX_DEPOSITO);
    printf("- Saque por transacao: R$ %.2f\n", MAX_SAQUE);
    printf("- Deposito diario: R$ %.2f\n", MAX_DEPOSITO_DIARIO);

    pausar();
}
