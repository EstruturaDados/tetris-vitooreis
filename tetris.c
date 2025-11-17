#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5    // Capacidade fixa da fila de peças futuras
#define TAM_PILHA 3   // Capacidade fixa da pilha de peças reservadas

// -----------------------------------------------------
// Struct que representa uma peça do Tetris Stack
// -----------------------------------------------------
typedef struct {
    char nome; // Tipo da peça: 'I', 'O', 'T', 'L'
    int id;    // Identificador único da peça
} Peca;

// -----------------------------------------------------
// Struct da fila circular de peças futuras
// -----------------------------------------------------
typedef struct {
    Peca elementos[TAM_FILA];
    int frente;      // Índice do primeiro elemento
    int tras;        // Índice da próxima posição livre
    int quantidade;  // Quantidade de elementos na fila
} FilaPecas;

// -----------------------------------------------------
// Struct da pilha de peças reservadas
// -----------------------------------------------------
typedef struct {
    Peca elementos[TAM_PILHA];
    int topo;        // Índice do topo (-1 indica pilha vazia)
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

// Inicialização e exibição
void preencherFilaInicial(FilaPecas *fila, int *proximoId);
void exibirEstado(FilaPecas *fila, PilhaPecas *pilha);
void exibirMenu();

// Ações especiais
void trocarPecaAtual(FilaPecas *fila, PilhaPecas *pilha);
void trocaMultiplaTres(FilaPecas *fila, PilhaPecas *pilha);

// -----------------------------------------------------
// Função principal
// -----------------------------------------------------
int main() {
    FilaPecas fila;
    PilhaPecas pilha;
    int opcao;
    int proximoId = 0; // contador para ids únicos

    srand((unsigned int)time(NULL));

    inicializarFila(&fila);
    inicializarPilha(&pilha);

    // A fila começa cheia com 5 peças
    preencherFilaInicial(&fila, &proximoId);

    do {
        printf("\n====================================\n");
        printf("       TETRIS STACK - NIVEL MESTRE\n");
        printf("====================================\n");

        exibirEstado(&fila, &pilha);
        exibirMenu();

        printf("Opcao escolhida: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            // 1 - Jogar peça da frente da fila (dequeue)
            if (filaVazia(&fila)) {
                printf("\nNao ha pecas na fila para jogar.\n");
            } else {
                Peca jogada;
                desenfileirar(&fila, &jogada);
                printf("\nAcao: Voce jogou a peca [%c %d]\n", jogada.nome, jogada.id);

                // A cada remoção da fila, gera nova peça para manter a fila cheia (quando possível)
                if (!filaCheia(&fila)) {
                    Peca nova = gerarPeca(&proximoId);
                    enfileirar(&fila, nova);
                    printf("Nova peca gerada e adicionada na fila: [%c %d]\n", nova.nome, nova.id);
                }
            }

        } else if (opcao == 2) {
            // 2 - Enviar peça da fila para a pilha de reserva
            if (pilhaCheia(&pilha)) {
                printf("\nNao foi possivel reservar: pilha de reserva cheia!\n");
            } else if (filaVazia(&fila)) {
                printf("\nNao ha pecas na fila para reservar.\n");
            } else {
                Peca reservada;
                desenfileirar(&fila, &reservada);
                empilhar(&pilha, reservada);
                printf("\nAcao: Peca da fila enviada para a pilha de reserva: [%c %d]\n",
                       reservada.nome, reservada.id);

                // A cada envio à pilha (remoção da fila), gera nova peça na fila
                if (!filaCheia(&fila)) {
                    Peca nova = gerarPeca(&proximoId);
                    enfileirar(&fila, nova);
                    printf("Nova peca gerada e adicionada na fila: [%c %d]\n", nova.nome, nova.id);
                }
            }

        } else if (opcao == 3) {
            // 3 - Usar peça da pilha de reserva (pop)
            if (pilhaVazia(&pilha)) {
                printf("\nNao ha pecas reservadas para usar.\n");
            } else {
                Peca usada;
                desempilhar(&pilha, &usada);
                printf("\nAcao: Voce usou a peca reservada [%c %d]\n", usada.nome, usada.id);
                // Observação: aqui NAO é gerada nova peça, pois a fila nao foi alterada.
            }

        } else if (opcao == 4) {
            // 4 - Trocar peça da frente da fila com o topo da pilha
            trocarPecaAtual(&fila, &pilha);

        } else if (opcao == 5) {
            // 5 - Trocar os 3 primeiros da fila com as 3 peças da pilha
            trocaMultiplaTres(&fila, &pilha);

        } else if (opcao == 0) {
            printf("\nEncerrando o gerenciador de pecas... Até logo!\n");
        } else {
            printf("\nOpcao invalida! Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}

// -----------------------------------------------------
// Implementação - Fila
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
        return 0; // falha: fila cheia
    }

    fila->elementos[fila->tras] = novaPeca;
    fila->tras = (fila->tras + 1) % TAM_FILA; // movimento circular
    fila->quantidade++;

    return 1; // sucesso
}

int desenfileirar(FilaPecas *fila, Peca *pecaRemovida) {
    if (filaVazia(fila)) {
        return 0; // falha: fila vazia
    }

    *pecaRemovida = fila->elementos[fila->frente];
    fila->frente = (fila->frente + 1) % TAM_FILA; // movimento circular
    fila->quantidade--;

    return 1; // sucesso
}

void exibirFila(FilaPecas *fila) {
    if (filaVazia(fila)) {
        printf("(fila vazia)");
        return;
    }

    int idx = fila->frente;
    for (int i = 0; i < fila->quantidade; i++) {
        Peca p = fila->elementos[idx];
        printf("[%c %d] ", p.nome, p.id);
        idx = (idx + 1) % TAM_FILA;
    }
}

// -----------------------------------------------------
// Implementação - Pilha
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
        return 0; // falha: pilha cheia
    }

    pilha->topo++;
    pilha->elementos[pilha->topo] = novaPeca;
    pilha->quantidade++;

    return 1; // sucesso
}

int desempilhar(PilhaPecas *pilha, Peca *pecaRemovida) {
    if (pilhaVazia(pilha)) {
        return 0; // falha: pilha vazia
    }

    *pecaRemovida = pilha->elementos[pilha->topo];
    pilha->topo--;
    pilha->quantidade--;

    return 1; // sucesso
}

void exibirPilha(PilhaPecas *pilha) {
    if (pilhaVazia(pilha)) {
        printf("(sem pecas reservadas)");
        return;
    }

    printf("(Topo -> base): ");
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
    int indiceTipo = rand() % 4; // sorteia entre 0 e 3

    nova.nome = tipos[indiceTipo];
    nova.id = *proximoId;

    (*proximoId)++; // prepara o próximo ID

    return nova;
}

// Preenche a fila inicialmente com TAM_FILA peças
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
    printf("\nOpcoes disponiveis:\n");
    printf("1 - Jogar peca da frente da fila\n");
    printf("2 - Enviar peca da fila para a pilha de reserva\n");
    printf("3 - Usar peca da pilha de reserva\n");
    printf("4 - Trocar peca da frente da fila com o topo da pilha\n");
    printf("5 - Trocar os 3 primeiros da fila com as 3 pecas da pilha\n");
    printf("0 - Sair\n");
}

// -----------------------------------------------------
// Ações especiais de troca
// -----------------------------------------------------

// Troca simples: frente da fila <-> topo da pilha
void trocarPecaAtual(FilaPecas *fila, PilhaPecas *pilha) {
    if (filaVazia(fila)) {
        printf("\nNao ha pecas na fila para trocar.\n");
        return;
    }
    if (pilhaVazia(pilha)) {
        printf("\nNao ha pecas na pilha de reserva para trocar.\n");
        return;
    }

    // Posição da frente da fila e topo da pilha
    int idxFila = fila->frente;
    int idxPilha = pilha->topo;

    Peca temp = fila->elementos[idxFila];
    fila->elementos[idxFila] = pilha->elementos[idxPilha];
    pilha->elementos[idxPilha] = temp;

    printf("\nAcao: troca entre a peca da frente da fila e o topo da pilha realizada.\n");
}

// Troca múltipla: 3 primeiros da fila <-> 3 da pilha
void trocaMultiplaTres(FilaPecas *fila, PilhaPecas *pilha) {
    if (fila->quantidade < 3) {
        printf("\nNao ha 3 pecas na fila para realizar a troca multipla.\n");
        return;
    }
    if (pilha->quantidade < 3) {
        printf("\nNao ha 3 pecas na pilha para realizar a troca multipla.\n");
        return;
    }

    // Vetores temporários para guardar os elementos
    Peca tempFila[3];
    Peca tempPilha[3];

    // 1) Copia os 3 primeiros da fila
    for (int i = 0; i < 3; i++) {
        int idx = (fila->frente + i) % TAM_FILA;
        tempFila[i] = fila->elementos[idx];
    }

    // 2) Copia as 3 peças da pilha (topo -> baixo)
    for (int i = 0; i < 3; i++) {
        int idx = pilha->topo - i;
        tempPilha[i] = pilha->elementos[idx];
    }

    // 3) Na fila: substitui os 3 primeiros pelas peças da pilha
    //    (mesma ordem: topo, depois as de baixo)
    for (int i = 0; i < 3; i++) {
        int idx = (fila->frente + i) % TAM_FILA;
        fila->elementos[idx] = tempPilha[i];
    }

    // 4) Na pilha: substitui topo, topo-1, topo-2
    //    pelas peças originais da fila, porém invertendo a ordem
    //    para preservar (Topo -> base) como no exemplo do enunciado.
    // Exemplo alvo:
    //   Fila: [O8][L7][T6]...
    //   Pilha (Topo->base): [T2][L1][I0]
    pilha->elementos[pilha->topo]     = tempFila[2]; // topo recebe o 3º da fila
    pilha->elementos[pilha->topo - 1] = tempFila[1]; // proximo recebe o 2º
    pilha->elementos[pilha->topo - 2] = tempFila[0]; // último recebe o 1º

    printf("\nAcao: troca realizada entre os 3 primeiros da fila e as 3 pecas da pilha.\n");
}
