#include <iostream>
using namespace std;

struct Fila {
    int cabeca;     // Índice do elemento da frente (próximo a ser desenfileirado)
    int cauda;      // Índice do último elemento (último enfileirado)
    int capacidade; // Capacidade máxima da fila
    int tamanho;    // Quantidade atual de elementos na fila
    int *elementos; // Vetor dinâmico que armazena os elementos
};

// Cria a fila com tamanho definido
void criarFila(Fila &fila, int capacidade) {
    fila.capacidade = capacidade;
    fila.cabeca = 0;
    fila.cauda = -1; // -1 = fila vazia
    fila.tamanho = 0;
    fila.elementos = new int[capacidade];
}

// Adiciona elemento no final da fila (cauda)
bool enfileirar(Fila &fila, int valor) {
    if (fila.tamanho == fila.capacidade) { // Verifica se a fila está cheia
        cout << "Erro: fila cheia!" << endl;
        return false;
    }
    fila.cauda = (fila.cauda + 1) % fila.capacidade; // Índice da cauda nova
    fila.elementos[fila.cauda] = valor; // Insere no final da fila
    fila.tamanho++;
    return true;
}

// Remove e retorna o elemento da frente (cabeca)
bool desenfileirar(Fila &fila, int &valorRemovido) {
    if (fila.tamanho == 0) { // Verifica se a fila está vazia
        cout << "Erro: fila vazia!" << endl;
        return false;
    }
    valorRemovido = fila.elementos[fila.cabeca]; // Retorna por referência do argumento o valor removido
    fila.cabeca = (fila.cabeca + 1) % fila.capacidade; // Atualiza nova cabeça da fila
    fila.tamanho--;

    if (fila.tamanho == 0) { // Se a fila está vazia, reseta atributos para valores padrão
        fila.cabeca = 0;
        fila.cauda = -1;
    }
    return true;
}

// Consulta a frente da fila
bool frente(const Fila &fila, int &valorFrente) {
    if (fila.tamanho == 0) { // Verifica se a fila está vazia
        cout << "Fila vazia!" << endl;
        return false;
    }
    valorFrente = fila.elementos[fila.cabeca]; // Retorna por referência do argumento o valor da frente da fila
    return true;
}

// Verifica se a fila está vazia
bool vazia(const Fila &fila) {
    return fila.tamanho == 0;
}

// Libera memória
void destruirFila(Fila &fila) {
    delete[] fila.elementos;
    fila.elementos = nullptr;
    fila.cabeca = 0;
    fila.cauda = -1;
    fila.capacidade = 0;
    fila.tamanho = 0;
}