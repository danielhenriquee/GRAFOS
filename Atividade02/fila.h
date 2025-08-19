#include <iostream>
using namespace std;

struct Fila {
    int indiceFrente;  // Índice do elemento na frente (para desenfileirar)
    int indiceTras;    // Índice do último elemento (para enfileirar)
    int capacidade;    // Capacidade máxima da fila
    int tamanho;       // Número atual de elementos na fila
    int* elementos;    // Vetor para armazenar os elementos
};

// Cria a fila com tamanho definido
void criarFila(Fila& fila, int capacidade) {
    fila.capacidade = capacidade;
    fila.indiceFrente = 0;
    fila.indiceTras = -1; // -1 indica que não há elemento inserido ainda
    fila.tamanho = 0;
    fila.elementos = new int[capacidade];
}

// Adiciona elemento ao fim (enfileirar)
bool enfileirar(Fila& fila, int valor) {
    if (fila.tamanho == fila.capacidade) {
        cout << "Erro: fila cheia!" << endl;
        return false;
    }
    // posição circular para o novo traseiro
    fila.indiceTras = (fila.indiceTras + 1) % fila.capacidade;
    fila.elementos[fila.indiceTras] = valor;
    fila.tamanho++;
    return true;
}

// Remove e retorna elemento da frente (desenfileirar)
bool desenfileirar(Fila& fila, int& valorRemovido) {
    if (fila.tamanho == 0) {
        cout << "Erro: fila vazia!" << endl;
        return false;
    }
    valorRemovido = fila.elementos[fila.indiceFrente];
    fila.indiceFrente = (fila.indiceFrente + 1) % fila.capacidade;
    fila.tamanho--;
    // opcional: resetar índices quando ficar vazia (não necessário, mas deixa estado parecido com o inicial)
    if (fila.tamanho == 0) {
        fila.indiceFrente = 0;
        fila.indiceTras = -1;
    }
    return true;
}

// Consulta a frente sem remover
bool frente(Fila& fila, int& valorFrente) {
    if (fila.tamanho == 0) {
        cout << "Fila vazia!" << endl;
        return false;
    }
    valorFrente = fila.elementos[fila.indiceFrente];
    return true;
}

// Verifica se a fila está vazia
bool vazia(const Fila& fila) {
    return fila.tamanho == 0;
}

// Libera memória
void destruirFila(Fila& fila) {
    delete[] fila.elementos;
    fila.elementos = nullptr;
    fila.indiceFrente = 0;
    fila.indiceTras = -1;
    fila.capacidade = 0;
    fila.tamanho = 0;
}