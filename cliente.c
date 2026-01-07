#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd_cmd, fd_resp;
    char cmd[64], resp[128];
    int n;

    fd_cmd = open("fifo_cmd", O_WRONLY);
    fd_resp = open("fifo_resp", O_RDONLY);

    if (fd_cmd < 0 || fd_resp < 0) {
        perror("FIFO");
        return 1;
    }

    while (1) {
        printf("\n1 - Abrir cofre");
        printf("\n2 - Inserir valor");
        printf("\n3 - Consultar valor");
        printf("\n4 - Esvaziar cofre");
        printf("\n5 - Encerrar daemon");
        printf("\nOpcao: ");
        fflush(stdout);

        fgets(cmd, sizeof(cmd), stdin);

        if (cmd[0] == '2') {
            char v[16];
            printf("Digite o valor: ");
            fflush(stdout);
            fgets(v, sizeof(v), stdin);
            snprintf(cmd, sizeof(cmd), "2 %s", v);
        }

        write(fd_cmd, cmd, strlen(cmd));

        n = read(fd_resp, resp, sizeof(resp)-1);
        resp[n] = '\0';
        printf("%s", resp);

        if (cmd[0] == '5') break;
    }

    return 0;
}

