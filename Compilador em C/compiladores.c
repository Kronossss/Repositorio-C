#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define MAX_TOKEN_LEN 256

typedef struct {
    char type[MAX_TOKEN_LEN];
    char value[MAX_TOKEN_LEN];
} Token;

typedef struct {
    char id[MAX_TOKEN_LEN];
    Token *token;
} EntradaEscopo;

void definirValorParaToken(char *tk_id, char *valor, EntradaEscopo *escopos, int num_escopos) {
    for (int i = num_escopos - 1; i >= 0; i--) {
        if (strcmp(tk_id, escopos[i].id) == 0) {
            strcpy(escopos[i].token->value, valor);
            return;
        }
    }
}

Token* obterTokenPorId(char *tk_id, EntradaEscopo *escopos, int num_escopos) {
    for (int i = num_escopos - 1; i >= 0; i--) {
        if (strcmp(tk_id, escopos[i].id) == 0) {
            return escopos[i].token;
        }
    }
    return NULL;
}

void processarLinha(char *linha, EntradaEscopo *escopos, int *num_escopos, char list_tk_id_block[][MAX_TOKEN_LEN], int *num_tk_id_block) {
    regex_t regex;
    int reti;

    // Expressões regulares
    char re_block_id[] = "_[a-zA-Z0-9]+_";
    char re_id_invalid[] = "[_0-9]+[a-zA-Z0-9_]*";
    char assign_variable[] = "[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*[a-zA-Z][a-zA-Z0-9_]*";

    reti = regcomp(&regex, re_block_id, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Não foi possível compilar a expressão regular\n");
        exit(EXIT_FAILURE);
    }

    if (regexec(&regex, linha, 0, NULL, 0) == 0) {
        char *block_id = strrchr(linha, ' ');
        if (block_id != NULL && sscanf(block_id, " %255s", list_tk_id_block[*num_tk_id_block]) == 1) {
            (*num_tk_id_block)++;
            escopos[*num_escopos].token = NULL;
            (*num_escopos)++;
        } else {
            printf("Erro Léxico: Identificador do bloco inválido.\n");
        }
    } else if (strstr(linha, "FIM") != NULL) {
        if (*num_tk_id_block > 0 && *num_escopos > 0) {
            char current_block[MAX_TOKEN_LEN];
            sscanf(linha, "FIM %255s", current_block);
            if (strcmp(current_block, list_tk_id_block[*num_tk_id_block - 1]) == 0) {
                (*num_escopos)--;
                (*num_tk_id_block)--;
            } else {
                printf("Erro Sintático: Identificador não corresponde ao bloco atual\n");
            }
        } else {
            printf("Erro Sintático: Bloco não iniciado\n");
        }
    } else {
        // Expressões regulares adicionais
        char espace[] = "\\s+";
        char re_number[] = "[+-]?\\d+(\\.\\d+)?";
        char re_string[] = "\"([^\"]*)\"";
        char assign_number[] = "[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*";
        char assign_string[] = "[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*";
        char assign_variable[] = "[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*[a-zA-Z][a-zA-Z0-9_]*";
        char dec_number_with_assign[] = "NUMERO\\s*=";
        char dec_string_with_assign[] = "CADEIA\\s*=";
        char dec_number_without_assign[] = "NUMERO\\s+[a-zA-Z_][a-zA-Z0-9_]*";
        char dec_string_without_assign[] = "CADEIA\\s+[a-zA-Z_][a-zA-Z0-9_]*";
        char print_values[] = "PRINT\\s+[a-zA-Z_][a-zA-Z0-9_]*";

        reti = regcomp(&regex, dec_number_with_assign, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular declaração de número com atribuição
            // ...
        }

        reti = regcomp(&regex, dec_string_with_assign, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular declaração de string com atribuição
            // ...
        }

        reti = regcomp(&regex, dec_number_without_assign, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular declaração de número sem atribuição
            // ...
        }

        reti = regcomp(&regex, dec_string_without_assign, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular declaração de string sem atribuição
            // ...
        }

        reti = regcomp(&regex, assign_variable, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular atribuição de variável
            // ...
        }

        reti = regcomp(&regex, assign_number, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular atribuição de número
            // ...
        }

        reti = regcomp(&regex, assign_string, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular atribuição de string
            // ...
        }

        reti = regcomp(&regex, print_values, REG_EXTENDED);
        if (reti == 0 && regexec(&regex, linha, 0, NULL, 0) == 0) {
            // Manipular comando PRINT
            // ...
        }
    }

    regfree(&regex);
}

void analisadorSemantico(char *caminho_arquivo) {
    EntradaEscopo escopos[MAX_TOKEN_LEN];
    int num_escopos = 0;
    char list_tk_id_block[MAX_TOKEN_LEN][MAX_TOKEN_LEN];
    int num_tk_id_block = 0;
    char linha[MAX_TOKEN_LEN];

    FILE *fd = fopen(caminho_arquivo, "r");
    if (fd == NULL) {
        printf("Arquivo não encontrado\n");
        return;
    }

    while (fgets(linha, MAX_TOKEN_LEN, fd) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0'; // Remover caracteres de nova linha

        processarLinha(linha, escopos, &num_escopos, list_tk_id_block, &num_tk_id_block);
    }

    fclose(fd);
}

int main() {
    analisadorSemantico("data/file.txt");
    return 0;
}
