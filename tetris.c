#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5    // Tamanho máximo da fila de peças futuras
#define TAM_PILHA 3   // Tamanho máximo da pilha de peças reservadas

// -----------------------------------------------------
// Struct que representa uma peça do Tetris Stack
// -----------------------------------------------------
typedef struct {
    char nome; // Tipo da peça: 'I', 'O', 'T', 'L'
    int id;    // Identificador único da peça
} Peca;

// -----------------------------------------------------
// Struct que representa a fila circular de peças futuras
// -----------------------------------------------------
typedef struct {
    Peca elementos[TAM_FILA];
    int frente;      // Índice do primeiro elemento
    int tras;        // Índice da próxima posição livre para inserir
    int quantidade;  // Quantidade de elementos na fila
} FilaPecas;

// -----------------------------------------------------
// Struct que representa a pilha de peças reservadas
// -----------------------------------------------------
typedef struct {
    Peca elementos[TAM_PILHA];
    int topo;        // Índice do topo da pilha (-1 indica pilha vazia)
    int quantidade;  // Quantidade de elementos na pilha
} PilhaPecas;

// -----------------------------------------------------
// Protótipos das funções
// -----------------------------------------------------

// Fila
void inicializarFila(FilaPecas *fila);
int filaVazia(FilaPecas *fila);
int filaCheia(FilaPecas *fila);
int enfileirar(FilaPecas *fila, Peca novaPeca);
int desenfileirar(FilaPecas *fila, Peca *pecaRemovida);
void exibirFila(FilaPecas *fila);

// Pilha
void inicializarPilha(PilhaPecas *pilha);
int pilhaVazia(PilhaPecas *pilha);
int pilhaCheia(PilhaPecas *pilha);
int empilhar(PilhaPecas *pilha, Peca novaPeca);
int desempilhar(PilhaPecas *pilha, Peca *pecaRemovida);
void exibirPilha(PilhaPecas *pilha);

// Geração de peças
Peca gerarPeca(int *proximoId);

// Inicialização e menu
void preencherFilaInicial(FilaPecas *fila, int *proximoId);
void exibirEstado(FilaPecas *fila, PilhaPecas *pilha);
void exibirMenu();

// Geração automática após ação
void gerarNovaPecaNaFilaSePossivel(FilaPecas *fila, int *proximoId);

// -----------------------------------------------------
// Função principal
// -----------------------------------------------------
int main() {
    FilaPecas fila;
    PilhaPecas pilha;
    int opcao;
    int proximoId = 0; // Contador para gerar IDs únicos

    srand((unsigned int)time(NULL));

    inicializarFila(&fila);
    inicializarPilha(&pilha);

    // Fila começa cheia com 5 peças
    preencherFilaInicial(&fila, &proximoId);

    do {
        printf("\n====================================\n");
        printf("   TETRIS STACK - NIVEL AVENTUREIRO\n");
        printf("====================================\n");

        exibirEstado(&fila, &pilha);
        exibirMenu();

        printf("Opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            // Jogar peça: remove da frente da fila
            Peca jogada;
            if (desenfileirar(&fila, &jogada)) {
                printf("\nVoce jogou a peca: [%c %d]\n", jogada.nome, jogada.id);
            } else {
                printf("\nNao ha pecas na fila para jogar (fila vazia).\n");
            }

            // A cada ação, tenta gerar nova peça
            gerarNovaPecaNaFilaSePossivel(&fila, &proximoId);

        } else if (opcao == 2) {
            // Reservar peça: move da frente da fila para o topo da pilha
            if (pilhaCheia(&pilha)) {
                printf("\nNao foi possivel reservar: pilha de reserva cheia!\n");
            } else if (filaVazia(&fila)) {
                printf("\nNao ha pecas na fila para reservar.\n");
            } else {
                Peca reservada;
                desenfileirar(&fila, &reservada);
                empilhar(&pilha, reservada);
                printf("\nPeca reservada com sucesso: [%c %d]\n", reservada.nome, reservada.id);
            }

            // A cada ação, tenta gerar nova peça
            gerarNovaPecaNaFilaSePossivel(&fila, &proximoId);

        } else if (opcao == 3) {
            // Usar peça reservada: remove do topo da pilha
            if (pilhaVazia(&pilha)) {
                printf("\nNao ha pecas reservadas para usar.\n");
            } else {
                Peca usada;
                desempilhar(&pilha, &usada);
                printf("\nVoce usou a peca reservada: [%c %d]\n", usada.nome, usada.id);
            }

            // A cada ação, tenta gerar nova peça
            gerarNovaPecaNaFilaSePossivel(&fila, &proximoId);

        } else if (opcao == 0) {
            printf("\nSaindo do jogo... Ate logo!\n");
        } else {
            printf("\nOpcao invalida! Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}

// -----------------------------------------------------
// Implementação das funções da Fila
// -----------------------------------------------------

void inicializarFila(FilaPecas *fila) {
    fila->frente = 0;
    fila->tras = 0;
    fila->quantidade = 0;
}

int filaVazia(FilaPecas *fila) {
    return (fila->quantidade == 0);
}

int filaCheia(FilaPecas *fila) {
    return (fila->quantidade == TAM_FILA);
}

int enfileirar(FilaPecas *fila, Peca novaPeca) {
    if (filaCheia(fila)) {
        return 0; // Falha: fila cheia
    }

    fila->elementos[fila->tras] = novaPeca;
    fila->tras = (fila->tras + 1) % TAM_FILA; // Movimento circular
    fila->quantidade++;

    return 1; // Sucesso
}

int desenfileirar(FilaPecas *fila, Peca *pecaRemovida) {
    if (filaVazia(fila)) {
        return 0; // Falha: fila vazia
    }

    *pecaRemovida = fila->elementos[fila->frente];
    fila->frente = (fila->frente + 1) % TAM_FILA; // Movimento circular
    fila->quantidade--;

    return 1; // Sucesso
}

void exibirFila(FilaPecas *fila) {
    if (filaVazia(fila)) {
        printf("(fila vazia)");
        return;
    }

    int i, idx;
    idx = fila->frente;
    for (i = 0; i < fila->quantidade; i++) {
        Peca p = fila->elementos[idx];
        printf("[%c %d] ", p.nome, p.id);
        idx = (idx + 1) % TAM_FILA;
    }
}

// -----------------------------------------------------
// Implementação das funções da Pilha
// -----------------------------------------------------

void inicializarPilha(PilhaPecas *pilha) {
    pilha->topo = -1;
    pilha->quantidade = 0;
}

int pilhaVazia(PilhaPecas *pilha) {
    return (pilha->quantidade == 0);
}

int pilhaCheia(PilhaPecas *pilha) {
    return (pilha->quantidade == TAM_PILHA);
}

int empilhar(PilhaPecas *pilha, Peca novaPeca) {
    if (pilhaCheia(pilha)) {
        return 0; // Falha: pilha cheia
    }

    pilha->topo++;
    pilha->elementos[pilha->topo] = novaPeca;
    pilha->quantidade++;

    return 1; // Sucesso
}

int desempilhar(PilhaPecas *pilha, Peca *pecaRemovida) {
    if (pilhaVazia(pilha)) {
        return 0; // Falha: pilha vazia
    }

    *pecaRemovida = pilha->elementos[pilha->topo];
    pilha->topo--;
    pilha->quantidade--;

    return 1; // Sucesso
}

void exibirPilha(PilhaPecas *pilha) {
    if (pilhaVazia(pilha)) {
        printf("(sem pecas reservadas)");
        return;
    }

    printf("(Topo -> Base): ");
    for (int i = pilha->topo; i >= 0; i--) {
        Peca p = pilha->elementos[i];
        printf("[%c %d] ", p.nome, p.id);
    }
}

// -----------------------------------------------------
// Geração de peças
// -----------------------------------------------------

Peca gerarPeca(int *proximoId) {
    Peca nova;
    char tipos[] = {'I', 'O', 'T', 'L'};
    int indiceTipo = rand() % 4; // 0 a 3

    nova.nome = tipos[indiceTipo];
    nova.id = *proximoId;

    (*proximoId)++; // Prepara próximo id

    return nova;
}

// Preenche a fila inicial com TAM_FILA peças
void preencherFilaInicial(FilaPecas *fila, int *proximoId) {
    for (int i = 0; i < TAM_FILA; i++) {
        Peca p = gerarPeca(proximoId);
        enfileirar(fila, p);
    }
}

// -----------------------------------------------------
// Exibição de estado e menu
// -----------------------------------------------------

void exibirEstado(FilaPecas *fila, PilhaPecas *pilha) {
    printf("\nEstado atual:\n\n");

    printf("Fila de pecas\t");
    exibirFila(fila);
    printf("\n");

    printf("Pilha de reserva\t");
    exibirPilha(pilha);
    printf("\n");
}

void exibirMenu() {
    printf("\nOpcoes de acao:\n");
    printf("1 - Jogar peca\n");
    printf("2 - Reservar peca\n");
    printf("3 - Usar peca reservada\n");
    printf("0 - Sair\n");
}

// -----------------------------------------------------
// Geração automática após cada ação
// -----------------------------------------------------

void gerarNovaPecaNaFilaSePossivel(FilaPecas *fila, int *proximoId) {
    if (filaCheia(fila)) {
        // Já está cheia, não há espaço para uma nova peça
        printf("\nFila ja esta cheia, nenhuma nova peca foi adicionada.\n");
        return;
    }

    Peca nova = gerarPeca(proximoId);
    enfileirar(fila, nova);
    printf("\nNova peca gerada e adicionada na fila: [%c %d]\n", nova.nome, nova.id);
}
