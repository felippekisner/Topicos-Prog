#include "raylib.h"
#include "listaObstaculos.h" // << lista encadeada de obstaculos (baseada no material de sala)
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define LARGURA 420                                                      // Definir LARGURA
#define ALTURA 620                                                       // Definir ALTURA
#define NUMERO_FAIXAS 4                                                  // Numero de Faixas
#define PISTA_ESQUERDA 69                                                // Pista da esquerda
#define PISTA_DIREITA 338                                                // Pista da Direita
#define FAIXA_LARGURA ((PISTA_DIREITA - PISTA_ESQUERDA) / NUMERO_FAIXAS) // Definir LARGURA da Faixa
#define CARRO_W 38                                                       // Tamanho da ALTURA do carro
#define CARRO_H 77                                                       // Tamanho da LARGURA do carro
#define OBSTACULO_W 65
#define OBSTACULO_H 86

// #*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#
// Joguinho de Carro do Tesser
// #*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#

// Struct do carro do jogador (posição/tamanho + faixa atual)
typedef struct {
    Rectangle rect;
    int faixa;
} Jogador;


int centralizarFaixa(int f) { // Função pra definir o centro da faixa onde o carro estará
    return PISTA_ESQUERDA + f * FAIXA_LARGURA + FAIXA_LARGURA / 2;
}


void exibir_menu(int *estado, int *opcao, bool *fechar, Jogador *jogador, Node *obstaculos,
              int *pontos, float *spawnTimer, int *proximoId) {

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        *opcao = 1;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        *opcao = 0;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (*opcao == 0) {
            jogador->faixa = 1;
            jogador->rect.x = centralizarFaixa(jogador->faixa) - CARRO_W / 2.0f;
            jogador->rect.y = ALTURA - CARRO_H - 50.0f;
            limpar_lista(obstaculos); // esvazia a lista de obstaculos (equivalente a zerar o array)
            *pontos = 0;
            *spawnTimer = 0.0f;
            *proximoId = 1; // reinicia a numeracao dos nos a cada partida
            *estado = 1;
        } else {
            *fechar = true;
        }
    }

    const char *t = "Desvie dos Obstaculos";
    DrawText(t, LARGURA / 2 - MeasureText(t, 30) / 2, 160, 30, GOLD);

    const char *jogar = (*opcao == 0) ? " JOGAR " : "JOGAR";
    const char *fechar_ = (*opcao == 1) ? " FECHAR " : "FECHAR";
    int tj = (*opcao == 0) ? 22 : 18;
    int tf = (*opcao == 1) ? 22 : 18;
    DrawText(jogar, LARGURA / 2 - MeasureText(jogar, tj) / 2, 300, tj, (*opcao == 0) ? RAYWHITE : GRAY);
    DrawText(fechar_, LARGURA / 2 - MeasureText(fechar_, tf) / 2, 350, tf, (*opcao == 1) ? RAYWHITE : GRAY);

    const char *rodape = "Navegue com W/S e selecione com ENTER";
    DrawText(rodape, LARGURA / 2 - MeasureText(rodape, 14) / 2, 440, 14, LIGHTGRAY);
}


void jogar_partida(int *estado, int *pontos, float *spawnTimer, Jogador *jogador,
                  Node *obstaculos, float dt, Texture2D pista, Texture2D carro, Texture2D obstaculoTex,
                  int *proximoId) {

    if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && jogador->faixa > 0) {
        jogador->faixa--;
    }
    if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && jogador->faixa < NUMERO_FAIXAS - 1) {
        jogador->faixa++;
    }
    jogador->rect.x = centralizarFaixa(jogador->faixa) - CARRO_W / 2.0f;

    *spawnTimer += dt;
    if (*spawnTimer >= 0.7f) {
        int f = rand() % NUMERO_FAIXAS;
        bool livre = true;
        for (Node *no = obstaculos->next; no != NULL; no = no->next) {
            if (no->data.faixa == f && no->data.rect.y < 220.0f) {
                livre = false;
                break;
            }
        }

        if (livre) {
            *spawnTimer = 0.0f;
            Obstaculo novo = {
                .rect = (Rectangle){(float)centralizarFaixa(f) - OBSTACULO_W / 2.0f, -OBSTACULO_H, OBSTACULO_W, OBSTACULO_H},
                .faixa = f,
                .id = (*proximoId)++ 
            };
            inserir_fim(obstaculos, novo);
        }
    }

    
    Node *anterior = obstaculos;
    Node *atual = obstaculos->next;
    while (atual != NULL) {
        atual->data.rect.y += 260.0f * dt;

        if (atual->data.rect.y > ALTURA) {
            (*pontos)++;
            Node *proximo = atual->next;
            remover_no(anterior); 
            atual = proximo;
            continue;
        }

        if (CheckCollisionRecs(jogador->rect, atual->data.rect)) {
            *estado = 2;
        }

        anterior = atual;
        atual = atual->next;
    }
    

    // Desenho da pista e do carro do jogador
    DrawTexturePro(pista, (Rectangle){0, 0, (float)pista.width, (float)pista.height},
                   (Rectangle){0, 0, LARGURA, ALTURA}, (Vector2){0, 0}, 0.0f, WHITE);

    DrawTexturePro(carro, (Rectangle){0, 0, (float)carro.width, (float)carro.height},
                   jogador->rect, (Vector2){0, 0}, 0.0f, WHITE);

    for (Node *no = obstaculos->next; no != NULL; no = no->next) {
        DrawTexturePro(obstaculoTex, (Rectangle){0, 0, (float)obstaculoTex.width, (float)obstaculoTex.height},
                       no->data.rect, (Vector2){0, 0}, 0.0f, WHITE);
        // numero do no desenhado em cima do proprio obstaculo na pista
        DrawText(TextFormat("%d", no->data.id),
                 (int)(no->data.rect.x + no->data.rect.width / 2.0f - 6),
                 (int)(no->data.rect.y - 18), 16, YELLOW);
    }

    DrawText(TextFormat("Pontos: %d", *pontos), 10, 10, 22, RAYWHITE);

}


void exibir_fim(int *estado, int *opcao, int pontos, Jogador jogador, Texture2D pista, Texture2D carro) {

    if (IsKeyPressed(KEY_ENTER)) {
        *opcao = 0;
        *estado = 0;
    }


    DrawTexturePro(pista, (Rectangle){0, 0, (float)pista.width, (float)pista.height},
                   (Rectangle){0, 0, LARGURA, ALTURA}, (Vector2){0, 0}, 0.0f, WHITE);

    DrawTexturePro(carro, (Rectangle){0, 0, (float)carro.width, (float)carro.height},
                   jogador.rect, (Vector2){0, 0}, 0.0f, WHITE);

    const char *t = "FIM DE JOGO";
    DrawText(t, LARGURA / 2 - MeasureText(t, 36) / 2, 200, 36, RED);

    const char *p = TextFormat("Voce fez %d pontos", pontos);
    DrawText(p, LARGURA / 2 - MeasureText(p, 20) / 2, 260, 20, RAYWHITE);

    const char *e = "Pressione ENTER para voltar ao Menu";
    DrawText(e, LARGURA / 2 - MeasureText(e, 16) / 2, 320, 16, LIGHTGRAY);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Jogo de Desviar de obstáculos"); // Abertura da Janela
    SetTargetFPS(60);                                             // Definir os FPS do jogo
    SetExitKey(KEY_ESCAPE);                                       // Se quiser sair é so clicar no ESC

    Texture2D pista = LoadTexture("images/background-1.png"); // Carrega o fundo do jogo
    Texture2D carro = LoadTexture("images/Car_1_01.png");      // Carrega o carro
    Texture2D obstaculoTex = LoadTexture("images/Police.png"); // Carrega o carro da policia

    int estado = 0;      // Dizer se o jogo está no estado jogando ou menu
    int opcao = 0;       // Serve pra dizer qual a opcao que o usuario escolheu
    bool fechar = false; // variavel booleana que serve para caso queira fechar o jogo
    int pontos = 0;          // Pontuacao
    float spawnTimer = 0.0f; // tempo de spawn dos obstaculos
    int proximoId = 1;       // numeracao sequencial dos nos da lista de obstaculos

    Jogador jogador = {
        .faixa = 1,
        .rect = {(float)centralizarFaixa(1) - CARRO_W / 2.0f, ALTURA - CARRO_H - 50.0f, CARRO_W, CARRO_H}
    };

    Node *obstaculos = criar_lista(); // << cabecalho da lista encadeada de obstaculos

    while (!fechar && !WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(DARKGRAY);

        if (estado == 0) { // MENU
            exibir_menu(&estado, &opcao, &fechar, &jogador, obstaculos, &pontos, &spawnTimer, &proximoId);
        } else if (estado == 1) { // JOGANDO
            jogar_partida(&estado, &pontos, &spawnTimer, &jogador, obstaculos, dt, pista, carro, obstaculoTex, &proximoId);
        } else { // NAO JOGANDO
            exibir_fim(&estado, &opcao, pontos, jogador, pista, carro);
        }

        EndDrawing();
    }

    destruir_lista(obstaculos); // libera todos os nos + o cabecalho
    UnloadTexture(pista);
    UnloadTexture(carro);
    UnloadTexture(obstaculoTex);
    CloseWindow();
    return 0;
}
