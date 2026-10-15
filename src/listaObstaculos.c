#include "listaObstaculos.h"
#include <stdlib.h>
#include <stdio.h>


Node* criar_lista(void) {
    Node *cabecalho = (Node*) malloc(sizeof(Node));
    if (cabecalho == NULL) {
        printf("Erro critico: falha ao alocar o no cabecalho da lista.\n");
        return NULL;
    }
    cabecalho->next = NULL;
    return cabecalho;
}


bool inserir_fim(Node *cabecalho, Obstaculo valor) {
    if (cabecalho == NULL) return false;

    Node *novo_no = (Node*) malloc(sizeof(Node));
    if (novo_no == NULL) {
        printf("Erro: falha ao alocar novo obstaculo.\n");
        return false;
    }

    novo_no->data = valor;
    novo_no->next = NULL;

    Node *atual = cabecalho;
    while (atual->next != NULL) {
        atual = atual->next;
    }

    atual->next = novo_no;
    return true;
}


void remover_no(Node *anterior) {
    if (anterior == NULL || anterior->next == NULL) return;

    Node *alvo = anterior->next;
    anterior->next = alvo->next;
    free(alvo);
}


void limpar_lista(Node *cabecalho) {
    if (cabecalho == NULL) return;

    Node *atual = cabecalho->next;
    while (atual != NULL) {
        Node *proximo = atual->next;
        free(atual);
        atual = proximo;
    }
    cabecalho->next = NULL;
}


void destruir_lista(Node *cabecalho) {
    if (cabecalho == NULL) return;

    limpar_lista(cabecalho);
    free(cabecalho);
}


int contar_lista(Node *cabecalho) {
    if (cabecalho == NULL) return 0;

    int total = 0;
    Node *atual = cabecalho->next;
    while (atual != NULL) {
        total++;
        atual = atual->next;
    }
    return total;
}
