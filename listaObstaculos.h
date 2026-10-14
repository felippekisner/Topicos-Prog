#ifndef LISTA_OBSTACULOS_H
#define LISTA_OBSTACULOS_H

#include "raylib.h"
#include <stdbool.h>

typedef struct Obstaculo {
    Rectangle rect;
    int faixa;
    int id; 
} Obstaculo;

typedef struct Node {
    Obstaculo data;
    struct Node *next;
} Node;

Node* criar_lista(void);

bool inserir_fim(Node *cabecalho, Obstaculo valor);

void remover_no(Node *anterior);

void limpar_lista(Node *cabecalho);

void destruir_lista(Node *cabecalho);

int contar_lista(Node *cabecalho);

#endif 
