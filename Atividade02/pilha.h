#include <iostream>
using namespace std;

struct Pilha {
    int indiceTopo; // Índice do elemento no topo
    int capacidade; // Capacidade máxima da pilha
    int *elementos; // Vetor dinâmico que armazena os elementos
};

// Cria a pilha com tamanho definido
void criarPilha(Pilha &pilha, int capacidade) {
    pilha.capacidade = capacidade;
    pilha.indiceTopo = -1; // -1 = fila vazia
    pilha.elementos = new int[capacidade];
}

// Adiciona elemento no topo da pilha
bool empilhar(Pilha &pilha, int valor) {
    if (pilha.indiceTopo == pilha.capacidade - 1) { // Verifica se a pilha está cheia
        cout << "Erro: pilha cheia!" << endl;
        return false;
    }
    pilha.indiceTopo++;
    pilha.elementos[pilha.indiceTopo] = valor;
    return true;
}

// Remove e retorna elemento do topo da pilha
bool desempilhar(Pilha &pilha, int &valorRemovido) {
    if (pilha.indiceTopo == -1) { // Verifica se a pilha está vazia
        cout << "Erro: pilha vazia!" << endl;
        return false;
    }
    valorRemovido = pilha.elementos[pilha.indiceTopo]; // Retorna por referência do argumento o valor removido
    pilha.indiceTopo--;
    return true;
}

// Consulta o topo da pilha
bool topo(const Pilha &pilha, int &valorTopo) {
    if (pilha.indiceTopo == -1) { // Verifica se a pilha está vazia
        cout << "Pilha vazia!" << endl;
        return false;
    }
    valorTopo = pilha.elementos[pilha.indiceTopo]; // Retorna por referência do argumento o valor do topo da pilha
    return true;
}

// Verifica se a pilha está vazia
bool vazia(const Pilha &pilha) {
    return pilha.indiceTopo == -1;
}

// Libera memória
void destruirPilha(Pilha &pilha) {
    delete[] pilha.elementos;
    pilha.elementos = nullptr;
    pilha.indiceTopo = -1;
    pilha.capacidade = 0;
}