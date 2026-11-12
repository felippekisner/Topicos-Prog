#include "raylib.h"
#include "listaObstaculos.h" // << lista encadeada de obstaculos (baseada no material de sala)
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define LARGURA 520                                                      // Definir LARGURA
#define ALTURA 620                                                       // Definir ALTURA
#define NUMERO_FAIXAS 4                                                  // Numero de Faixas
#define PISTA_ESQUERDA 110                                                // Pista da esquerda
#define PISTA_DIREITA 415                                               // Pista da Direita
#define FAIXA_LARGURA ((PISTA_DIREITA - PISTA_ESQUERDA) / NUMERO_FAIXAS) // Definir LARGURA da Faixa
#define CARRO_W 48                                                       // Tamanho da ALTURA do carro
#define CARRO_H 74                                                       // Tamanho da LARGURA do carro
#define OBSTACULO_W 65
#define OBSTACULO_H 76
#define NUMERO_VIDAS 3
#define MAX_VIDAS 5             // limite de vidas (a vida extra nao passa disso)
#define TEMPO_PODER 22.0f       // de quanto em quanto tempo (s) aparece um poder
#define TEMPO_ESCUDO 5.0f       // quanto tempo (s) o escudo dura

// Tipos de objeto que descem a pista
#define POLICIA 0
#define AGUA 1
#define FOGUETE 2  // obstaculo: foguete 
#define ESCUDO 3   // poder: escudo
#define VIDA 4     // poder: vida extra


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
              int *pontos, float *spawnTimer, int *proximoId, int *vida,
              float *invencivel, float *spawn_poder, float *escudo) {

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
            *vida = NUMERO_VIDAS; // volta as vidas
            *invencivel = 0.0f;
            *spawn_poder = 0.0f;
            *escudo = 0.0f;
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

    const char *d1 = "CUIDADO com o foguete: e rapido e tira vida!";
    const char *d2 = "Bolinha azul (E): ESCUDO por 5 segundos";
    const char *d3 = "Bolinha vermelha (+1): VIDA EXTRA";
    const char *d4 = "Tecla M ou o icone do alto-falante: liga/desliga o som";
    DrawText(d1, LARGURA / 2 - MeasureText(d1, 14) / 2, 480, 14, RED);
    DrawText(d2, LARGURA / 2 - MeasureText(d2, 14) / 2, 505, 14, SKYBLUE);
    DrawText(d3, LARGURA / 2 - MeasureText(d3, 14) / 2, 530, 14, PINK);
    DrawText(d4, LARGURA / 2 - MeasureText(d4, 12) / 2, 570, 12, LIGHTGRAY);
}

void jogar_partida(int *estado, int *pontos, float *spawnTimer, Jogador *jogador,
                  Node *obstaculos, float dt, Texture2D pista, Texture2D carro, Texture2D obstaculoTex,
                  int *proximoId, int *vida, Texture2D agua_pista, Texture2D foguete_tex,
                  Sound *som_batida, Sound *torcida_triste, Sound *torcida,
                  float *invencivel, float *spawn_poder, float *escudo) {

    if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && jogador->faixa > 0) {
        jogador->faixa--;
    }
    if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && jogador->faixa < NUMERO_FAIXAS - 1) {
        jogador->faixa++;
    }
    jogador->rect.x = centralizarFaixa(jogador->faixa) - CARRO_W / 2.0f;

    // Diminui os tempos da invencibilidade (que vem depois de levar dano) e do escudo
    if (*invencivel > 0) *invencivel -= dt;
    if (*escudo > 0) *escudo -= dt;

    // A pista vai ficando mais rapida conforme os pontos (ate um limite)
    float velocidade = 300.0f + *pontos * 2.0f;
    if (velocidade > 450.0f) velocidade = 450.0f;

    // Spawn dos obstaculos (policia, agua ou foguete)
    *spawnTimer += dt;
    if (*spawnTimer >= 0.6f) {
        int f = rand() % NUMERO_FAIXAS;
        int r = rand() % 100;
        int tipo = POLICIA;
        if (r < 25) tipo = AGUA;          // 25% agua
        else if (r < 45) tipo = FOGUETE;  // 20% foguete

        // a agua aparece no meio da pista, entao a faixa precisa estar livre mais pra baixo
        float limite = (tipo == AGUA) ? 320.0f : 220.0f;
        bool livre = true;
        for (Node *no = obstaculos->next; no != NULL; no = no->next) {
            if (no->data.faixa == f && no->data.rect.y < limite) {
                livre = false;
                break;
            }
        }

        if (livre) {
            *spawnTimer = 0.0f;
            float w = OBSTACULO_W;
            float h = OBSTACULO_H;
            float y = -OBSTACULO_H;             // policia nasce acima da tela e desce
            if (tipo == AGUA) {
                w = 76;
                h = 30;
                y = 80 + rand() % 161;          // agua surge "do nada" no meio da pista (y de 80 a 240)
            } else if (tipo == FOGUETE) {
                w = 36;
                h = 70;
                y = -70;
            }
            Obstaculo novo = {
                .rect = (Rectangle){(float)centralizarFaixa(f) - w / 2.0f, y, w, h},
                .faixa = f,
                .id = (*proximoId)++,
                .tipo = tipo,
                .tempo = 0.0f
            };
            inserir_fim(obstaculos, novo);
        }
    }

    // Spawn dos poderes (demora bastante pra aparecer um) - sorteia entre escudo e vida
    *spawn_poder += dt;
    if (*spawn_poder >= TEMPO_PODER) {
        *spawn_poder = 0.0f;
        int f = rand() % NUMERO_FAIXAS;
        Obstaculo poder = {
            .rect = (Rectangle){(float)centralizarFaixa(f) - 22, -44, 44, 44},
            .faixa = f,
            .id = 0,
            .tipo = ESCUDO + rand() % 2,   // 3 = escudo, 4 = vida
            .tempo = 0.0f
        };
        inserir_fim(obstaculos, poder);
    }


    Node *anterior = obstaculos;
    Node *atual = obstaculos->next;
    while (atual != NULL) {
        float v = velocidade;
        if (atual->data.tipo == FOGUETE) v = velocidade * 2.2f; // o foguete desce mais rapido
        atual->data.rect.y += v * dt;
        atual->data.tempo += dt;

        if (atual->data.rect.y > ALTURA) {
            if (atual->data.tipo < ESCUDO) (*pontos)++; // poder que passou nao da ponto
            Node *proximo = atual->next;
            remover_no(anterior);
            atual = proximo;
            continue;
        }

        
        bool colide = CheckCollisionRecs(jogador->rect, atual->data.rect);
        if (atual->data.tipo == AGUA && atual->data.tempo < 0.5f) colide = false;

        if (colide) {
            if (atual->data.tipo >= ESCUDO) {
                // pegou um poder
                PlaySound(*torcida);
                if (atual->data.tipo == ESCUDO) {
                    *escudo = TEMPO_ESCUDO;
                } else if (*vida < MAX_VIDAS) {
                    (*vida)++;
                }
                Node *proximo = atual->next;
                remover_no(anterior);
                atual = proximo;
                continue;
            } else if (*invencivel <= 0) {
                PlaySound(*som_batida);
                if (*escudo <= 0) {
                    // sem escudo: perde uma vida
                    PlaySound(*torcida_triste);
                    (*vida)--;
                    *invencivel = 1.0f; // fica invencivel um tempinho
                    if (*vida > 0) {
                        *estado = 1;
                    } else {
                        *estado = 2;
                    }
                }
                Node *proximo = atual->next; // o obstaculo que bateu some
                remover_no(anterior);
                atual = proximo;
                continue;
            }
        }

        anterior = atual;
        atual = atual->next;
    }


    // Desenho da pista e do carro do jogador
    DrawTexturePro(pista, (Rectangle){0, 0, (float)pista.width, (float)pista.height},
                   (Rectangle){0, 0, LARGURA, ALTURA}, (Vector2){0, 0}, 0.0f, WHITE);

    // o carro pisca enquanto esta invencivel por causa do dano
    if (*invencivel <= 0 || ((int)(*invencivel * 10)) % 2 == 0) {
        DrawTexturePro(carro, (Rectangle){0, 0, (float)carro.width, (float)carro.height},
                       jogador->rect, (Vector2){0, 0}, 0.0f, WHITE);
    }

    // bolha azul em volta do carro quando tem escudo (pisca quando esta acabando)
    if (*escudo > 0 && (*escudo > 1.5f || ((int)(*escudo * 10)) % 2 == 0)) {
        DrawCircle((int)(jogador->rect.x + CARRO_W / 2), (int)(jogador->rect.y + CARRO_H / 2), 50, Fade(SKYBLUE, 0.3f));
    }

    for (Node *no = obstaculos->next; no != NULL; no = no->next) {
        Rectangle r = no->data.rect;
        int cx = (int)(r.x + r.width / 2);
        int cy = (int)(r.y + r.height / 2);

        if (no->data.tipo == POLICIA) {
            DrawTexturePro(obstaculoTex, (Rectangle){0, 0, (float)obstaculoTex.width, (float)obstaculoTex.height},
                           r, (Vector2){0, 0}, 0.0f, WHITE);
        } else if (no->data.tipo == AGUA) {
            // a agua aparece aos poucos (fica transparente no comeco)
            float alpha = no->data.tempo / 0.5f;
            if (alpha > 1.0f) alpha = 1.0f;
            // so a parte da poca da imagem (o resto da imagem e transparente)
            DrawTexturePro(agua_pista, (Rectangle){10, 91, 236, 74},
                           r, (Vector2){0, 0}, 0.0f, Fade(WHITE, alpha));
        } else if (no->data.tipo == FOGUETE) {
            // so a parte do foguete da imagem, girado 180 graus (cai de ponta cabeca, apontando pro carro)
            DrawTexturePro(foguete_tex, (Rectangle){135, 21, 242, 470},
                           (Rectangle){(float)cx, (float)cy, r.width, r.height},
                           (Vector2){r.width / 2, r.height / 2}, 180.0f, WHITE);
        } else if (no->data.tipo == ESCUDO) {
            DrawCircle(cx, cy, 22, SKYBLUE);
            DrawText("E", cx - MeasureText("E", 26) / 2, cy - 13, 26, WHITE);
        } else {
            DrawCircle(cx, cy, 22, RED);
            DrawText("+1", cx - MeasureText("+1", 22) / 2, cy - 11, 22, WHITE);
        }

        if (no->data.tipo < ESCUDO) {
            // numero do no desenhado em cima do proprio obstaculo na pista
            DrawText(TextFormat("%d", no->data.id),
                     (int)(r.x + r.width / 2.0f - 6),
                     (int)(r.y - 18), 16, YELLOW);
        }
    }

    DrawText(TextFormat("Pontos: %d", *pontos), 10, 10, 22, RAYWHITE);
    DrawText(TextFormat("Vidas: %d", *vida), 10, 40, 22, RAYWHITE);
    if (*escudo > 0) {
        DrawText(TextFormat("ESCUDO: %.1f", *escudo), 10, 70, 22, SKYBLUE);
    }

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
    srand(time(NULL));                                            // Sorteio diferente a cada jogo
    InitWindow(LARGURA, ALTURA, "Jogo de Desviar de obstáculos"); // Abertura da Janela
    SetTargetFPS(60);                                             // Definir os FPS do jogo
    SetExitKey(KEY_ESCAPE);                                       // Se quiser sair é so clicar no ESC
    // 1. Inicializa o dispositivo de áudio
    InitAudioDevice();
    // 2. Carrega o arquivo de som (substitua pelo caminho do seu arquivo)
    Sound som_batida = LoadSound("resources/som_batida.mp3");
    Music musica_fundo = LoadMusicStream("resources/musica_fundo.ogg");
    Sound torcida = LoadSound("resources/torcida.mp3");
    Sound torcida_triste = LoadSound("resources/torcida_triste.mp3");
    Texture2D pista = LoadTexture("images/background-1.png"); // Carrega o fundo do jogo
    Texture2D carro = LoadTexture("images/Car_1_01.png");      // Carrega o carro
    Texture2D obstaculoTex = LoadTexture("images/Police.png"); // Carrega o carro da policia
    Texture2D agua_pista = LoadTexture("images/agua.png"); //Carrega o obstáculo da água
    Texture2D foguete_tex = LoadTexture("images/turbo.png"); // Carrega o foguete (obstaculo)

    int estado = 0;      // Dizer se o jogo está no estado jogando ou menu
    int opcao = 0;       // Serve pra dizer qual a opcao que o usuario escolheu
    bool fechar = false; // variavel booleana que serve para caso queira fechar o jogo
    int pontos = 0;          // Pontuacao
    float spawnTimer = 0.0f; // tempo de spawn dos obstaculos
    float spawn_poder = 0.0f; // tempo de spawn do poder
    float invencivel = 0.0f;     // tempo que ainda resta de invencibilidade
    float escudo = 0.0f;     // tempo que ainda resta de escudo
    bool mudo = false;       // true = som desligado
    Rectangle botaoSom = {LARGURA - 50, 10, 40, 40}; // botao do alto-falante (canto superior direito)
    int proximoId = 1;       // numeracao sequencial dos nos da lista de obstaculos
    int vida = NUMERO_VIDAS;


    Jogador jogador = {
        .faixa = 1,
        .rect = {(float)centralizarFaixa(1) - CARRO_W / 2.0f, ALTURA - CARRO_H - 50.0f, CARRO_W, CARRO_H}
    };

    Node *obstaculos = criar_lista(); // << cabecalho da lista encadeada de obstaculos
    PlayMusicStream(musica_fundo);
    PlaySound(torcida);
    while (!fechar && !WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateMusicStream(musica_fundo);
        BeginDrawing();
        ClearBackground(DARKGRAY);

        if (estado == 0) { // MENU
            exibir_menu(&estado, &opcao, &fechar, &jogador, obstaculos, &pontos, &spawnTimer, &proximoId, &vida, &invencivel, &spawn_poder, &escudo);
        } else if (estado == 1) { // JOGANDO
            jogar_partida(&estado, &pontos, &spawnTimer, &jogador, obstaculos, dt, pista, carro, obstaculoTex, &proximoId, &vida, agua_pista, foguete_tex, &som_batida, &torcida_triste, &torcida, &invencivel, &spawn_poder, &escudo);
        } else { // NAO JOGANDO
            exibir_fim(&estado, &opcao, pontos, jogador, pista, carro);
        }

        // Botao de som: clicar no alto-falante ou apertar M liga/desliga todo o som
        if (IsKeyPressed(KEY_M) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botaoSom))) {
            mudo = !mudo;
            SetMasterVolume(mudo ? 0.0f : 1.0f);
        }
        DrawCircle(LARGURA - 30, 30, 20, Fade(BLACK, 0.5f));
        DrawRectangle(LARGURA - 41, 26, 6, 8, WHITE);   // corpo do alto-falante
        DrawTriangle((Vector2){LARGURA - 35, 30}, (Vector2){LARGURA - 25, 40}, (Vector2){LARGURA - 25, 20}, WHITE);
        if (mudo) {
            DrawLineEx((Vector2){LARGURA - 22, 24}, (Vector2){LARGURA - 12, 36}, 3, RED); // X vermelho = mudo
            DrawLineEx((Vector2){LARGURA - 22, 36}, (Vector2){LARGURA - 12, 24}, 3, RED);
        } else {
            DrawLine(LARGURA - 21, 25, LARGURA - 21, 35, WHITE); // ondas de som
            DrawLine(LARGURA - 17, 22, LARGURA - 17, 38, WHITE);
        }

        EndDrawing();
    }

    destruir_lista(obstaculos); // libera todos os nos + o cabecalho
    UnloadTexture(pista);
    UnloadTexture(carro);
    UnloadTexture(obstaculoTex);
    UnloadTexture(agua_pista);
    UnloadTexture(foguete_tex);
    UnloadSound(som_batida);
    UnloadMusicStream(musica_fundo);
    UnloadSound(torcida);
    UnloadSound(torcida_triste);
    CloseWindow();
    return 0;
}
