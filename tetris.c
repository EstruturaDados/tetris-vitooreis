#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5   // Tamanho máximo da fila de peças

// ----------------------
// Struct que representa uma peça do Tetris Stack
// ----------------------
typedef struct {
    char nome; // Tipo da peça: 'I', 'O', 'T', 'L'
    int id;    // Identificador único da peça
} Peca;

// ----------------------
// Struct que representa a fila circular de peças
// ----------------------
typedef struct {
    Peca elementos[TAM_FILA]; // Vetor com as peças
    int frente;               // Índice do primeiro elemento
    int tras;                 // Índice da próxima posição livre para inserir
    int quantidade;           // Quantidade de elementos atualmente na fila
} FilaPecas;

// ----------------------
// Protótipos das funções
// ----------------------
void inicializarFila(FilaPecas *fila);
int filaVazia(FilaPecas *fila);
int filaCheia(FilaPecas *fila);
void exibirFila(FilaPecas *fila);

Peca gerarPeca(int *proximoId);
int enfileirar(FilaPecas *fila, Peca novaPeca);
int desenfileirar(FilaPecas *fila, Peca *pecaRemovida);

void preencherFilaInicial(FilaPecas *fila, int *proximoId);
void exibirMenu();

// ----------------------
// Função principal
// ----------------------
int main() {
    FilaPecas fila;
    int opcao;
    int proximoId = 0; // Contador para gerar IDs únicos

    // Inicializa o gerador de números aleatórios para gerar tipos de peça
    srand((unsigned int)time(NULL));

    // Inicializa a fila
    inicializarFila(&fila);

    // Preenche a fila com um número fixo de peças iniciais (por exemplo, 5)
    preencherFilaInicial(&fila, &proximoId);

    // Loop principal do menu
    do {
        printf("\n====================================\n");
        printf("   TETRIS STACK - FILA DE PECAS\n");
        printf("====================================\n");

        printf("Confira a seguir seu estado:\n\n");
        printf("Fila de pecas:\n");
        exibirFila(&fila);

        exibirMenu();
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            // Jogar peça (dequeue)
            Peca pecaJogada;
            if (desenfileirar(&fila, &pecaJogada)) {
                printf("\nVoce jogou a peca [%c %d]\n", pecaJogada.nome, pecaJogada.id);
            } else {
                printf("\nNao ha pecas na fila para jogar (fila vazia).\n");
            }
        } else if (opcao == 2) {
            // Inserir nova peça (enqueue)
            if (filaCheia(&fila)) {
                printf("\nNao foi possivel inserir nova peca: fila cheia!\n");
            } else {
                Peca nova = gerarPeca(&proximoId);
                enfileirar(&fila, nova);
                printf("\nNova peca gerada e inserida na fila: [%c %d]\n", nova.nome, nova.id);
            }
        } else if (opcao == 0) {
            printf("\nSaindo do jogo... Ate logo!\n");
        } else {
            printf("\nOpcao invalida! Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}

// ----------------------
// Implementações das funções
// ----------------------

// Inicializa a fila: define índices e quantidade inicial
void inicializarFila(FilaPecas *fila) {
    fila->frente = 0;
    fila->tras = 0;
    fila->quantidade = 0;
}

// Retorna 1 se a fila estiver vazia, senão 0
int filaVazia(FilaPecas *fila) {
    return (fila->quantidade == 0);
}

// Retorna 1 se a fila estiver cheia, senão 0
int filaCheia(FilaPecas *fila) {
    return (fila->quantidade == TAM_FILA);
}

// Exibe o estado atual da fila
// Exemplo: [T 0] [O 1] [L 2] [I 3] [I 4]
void exibirFila(FilaPecas *fila) {
    if (filaVazia(fila)) {
        printf("(fila vazia)\n");
        return;
    }

    int i;
    int idx = fila->frente;

    for (i = 0; i < fila->quantidade; i++) {
        Peca p = fila->elementos[idx];
        printf("[%c %d] ", p.nome, p.id);
        idx = (idx + 1) % TAM_FILA; // Avança de forma circular
    }
    printf("\n");
}

// Gera automaticamente uma nova peça
// - nome: sorteado entre 'I', 'O', 'T', 'L'
// - id: número inteiro único (incrementando proximoId)
Peca gerarPeca(int *proximoId) {
    Peca nova;
    char tipos[] = {'I', 'O', 'T', 'L'};
    int indiceTipo = rand() % 4; // 0 a 3

    nova.nome = tipos[indiceTipo];
    nova.id = *proximoId;

    (*proximoId)++; // Incrementa o ID para a próxima peça

    return nova;
}

// Insere uma nova peça no final da fila (enqueue)
// Retorna 1 se inseriu, 0 se não conseguiu (fila cheia)
int enfileirar(FilaPecas *fila, Peca novaPeca) {
    if (filaCheia(fila)) {
        return 0;
    }

    fila->elementos[fila->tras] = novaPeca;
    fila->tras = (fila->tras + 1) % TAM_FILA; // Avança de forma circular
    fila->quantidade++;

    return 1;
}

// Remove uma peça da frente da fila (dequeue)
// Retorna 1 se removeu, 0 se a fila está vazia
int desenfileirar(FilaPecas *fila, Peca *pecaRemovida) {
    if (filaVazia(fila)) {
        return 0;
    }

    *pecaRemovida = fila->elementos[fila->frente];
    fila->frente = (fila->frente + 1) % TAM_FILA; // Avança de forma circular
    fila->quantidade--;

    return 1;
}

// Preenche a fila inicialmente com TAM_FILA peças
void preencherFilaInicial(FilaPecas *fila, int *proximoId) {
    int i;
    for (i = 0; i < TAM_FILA; i++) {
        Peca nova = gerarPeca(proximoId);
        enfileirar(fila, nova);
    }
}

// Exibe o menu de opções
void exibirMenu() {
    printf("\nOpcoes de acao:\n");
    printf("1 - Jogar peca (dequeue)\n");
    printf("2 - Inserir nova peca (enqueue)\n");
    printf("0 - Sair\n");
}
