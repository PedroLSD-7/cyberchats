#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    // Descritores dos FIFOs de comando e resposta
    int fd_cmd, fd_resp;

    // Buffers para enviar comandos e receber respostas
    char cmd[64], resp[128];

    // Variável para armazenar a quantidade de bytes lidos
    int n;

    // Abre o FIFO de comandos para escrita (envia comandos ao daemon)
    fd_cmd = open("fifo_cmd", O_WRONLY);

    // Abre o FIFO de respostas para leitura (recebe respostas do daemon)
    fd_resp = open("fifo_resp", O_RDONLY);

    // Verifica se houve erro ao abrir os FIFOs
    if (fd_cmd < 0 || fd_resp < 0) {
        perror("FIFO");
        return 1;
    }

    // Loop principal do programa (menu interativo)
    while (1) {
        // Exibe o menu de opções
        printf("\n1 - Abrir cofre");
        printf("\n2 - Inserir valor");
        printf("\n3 - Consultar valor");
        printf("\n4 - Esvaziar cofre");
        printf("\n5 - Encerrar daemon");
        printf("\nOpcao: ");
        fflush(stdout);

        // Lê a opção escolhida pelo usuário
        fgets(cmd, sizeof(cmd), stdin);

        // Se a opção for inserir valor, pede o valor ao usuário
        if (cmd[0] == '2') {
            char v[16];

            printf("Digite o valor: ");
            fflush(stdout);

            // Lê o valor digitado
            fgets(v, sizeof(v), stdin);

            // Monta o comando com opção + valor
            snprintf(cmd, sizeof(cmd), "2 %s", v);
        }

        // Envia o comando para o daemon pelo FIFO
        write(fd_cmd, cmd, strlen(cmd));

        // Lê a resposta enviada pelo daemon
        n = read(fd_resp, resp, sizeof(resp)-1);

        // Finaliza a string da resposta
        resp[n] = '\0';

        // Exibe a resposta do daemon
        printf("%s", resp);

        // Se a opção for encerrar, sai do loop
        if (cmd[0] == '5') break;
    }

    // Encerra o programa cliente
    return 0;
}