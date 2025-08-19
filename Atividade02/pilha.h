#include <iostream>
using namespace std;

struct Pilha {
    int indiceTopo;  // Índice do elemento no topo
    int capacidade;  // Capacidade máxima da pilha
    int* elementos;  // Vetor para armazenar os elementos
};

// Cria a pilha com tamanho definido
void criarPilha(Pilha& pilha, int capacidade) {
    pilha.capacidade = capacidade;
    pilha.indiceTopo = -1; // -1 indica pilha vazia
    pilha.elementos = new int[capacidade];
}

// Adiciona elemento ao topo
bool empilhar(Pilha& pilha, int valor) {
    if (pilha.indiceTopo == pilha.capacidade - 1) {
        cout << "Erro: pilha cheia!" << endl;
        return false;
    }
    pilha.indiceTopo++;
    pilha.elementos[pilha.indiceTopo] = valor;
    return true;
}

// Remove e retorna elemento do topo
bool desempilhar(Pilha& pilha, int& valorRemovido) {
    if (pilha.indiceTopo == -1) {
        cout << "Erro: pilha vazia!" << endl;
        return false;
    }
    valorRemovido = pilha.elementos[pilha.indiceTopo];
    pilha.indiceTopo--;
    return true;
}

// Consulta o topo sem remover
bool topo(Pilha& pilha, int& valorTopo) {
    if (pilha.indiceTopo == -1) {
        cout << "Pilha vazia!" << endl;
        return false;
    }
    valorTopo = pilha.elementos[pilha.indiceTopo];
    return true;
}

// Libera memória
void destruirPilha(Pilha& pilha) {
    delete[] pilha.elementos;
    pilha.elementos = nullptr;
    pilha.indiceTopo = -1;
    pilha.capacidade = 0;
}

// Verifica se a pilha está vazia
bool vazia(const Pilha& pilha) {
    return pilha.indiceTopo == -1;
}