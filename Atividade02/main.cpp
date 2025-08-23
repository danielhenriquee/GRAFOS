#include "pilha.h"
#include "fila.h"
#include <limits>
#include <iostream>
using namespace std;

const int MAX_VERTICES = 100;

// Códigos de escape ANSI para cores (para exibir a matriz)
constexpr const char* RED = "\033[31m";   // Vermelho
constexpr const char* GREEN = "\033[32m"; // Verde
constexpr const char* RESET = "\033[0m";  // Resetar cor

void clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nPressione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

// Exibe a matriz de adjacência
void mostrarGrafo(int G[][MAX_VERTICES], int numVertices) {
    for(int i = 0; i < numVertices; i++) {
        for(int j = 0; j < numVertices; j++) {
            if (G[i][j] == 0) {
                cout << RED << G[i][j] << RESET << " ";
            } else {
                cout << GREEN << G[i][j] << RESET << " ";
            }
        }
        cout << "\n";
    }
}

// Retorna "true" se o vértice já foi visitado. Se o índice for inválido, emite um aviso e retorna "true" para evitar acessos fora do vetor.
bool foiVisitado(int v, bool visitados[], int numVertices) {
    if (v < 0 || v >= numVertices) {
        cout << "Aviso: vértice inválido acessado!\n";
        return true; 
    }
    return visitados[v];
}

// Marca o vértice como visitado e imprime a ação.
void visitar(int v, bool visitados[], int numVertices) {
    if (v < 0 || v >= numVertices) {
        cout << "Vértice inválido!\n";
        return;
    }
    cout << "Visitando: " << v+1 << endl;
    visitados[v] = true;
}

// Função: DFS (busca em profundidade)
void DFS(int v, int G[][MAX_VERTICES], int numVertices, bool visitados[]) {
    Pilha S;
    criarPilha(S, MAX_VERTICES);

    visitar(v, visitados, numVertices); // Marca o vértice como visitado
    empilhar(S, v); // Empilha o vértice de origem

    while (true) {
        while (!vazia(S)) { // Enquanto houver vértices na pilha
            int atual;
            desempilhar(S, atual); // Desempilha o último vértice inserido na pilha;

            for (int i = numVertices - 1; i >= 0; i--) { 
                if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vértices adjacentes não visitados na pilha
                    visitar(i, visitados, numVertices); // Marca o vértice como visitado
                    empilhar(S, i);
                }
            }
        } 
 
        // Procura vértice não visitado (grafo desconexo)
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                visitar(i, visitados, numVertices); // Marca o vértice como visitado
                empilhar(S, i);
                aux = true;
                break;
            }
        }

        if (!aux) { // Se todos os vértices foram marcados, sai do DFS
            destruirPilha(S);
            break;
        }
    }
}

// Função: BFS (busca em largura)
void BFS(int v, int G[][MAX_VERTICES], int numVertices, bool visitados[]) {
    Fila Q;
    criarFila(Q, MAX_VERTICES);

    visitar(v, visitados, numVertices); // Marca o vértice como visitado
    enfileirar(Q, v); // Enfileira o vértice de origem

    while (true) {
        while (!vazia(Q)){ // Enquanto houver vértices na fila
            int atual;
            desenfileirar(Q, atual); // Desenfileira o primeiro vértice da fila

            for (int i = 0; i < numVertices; i++) {
                if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vértices adjacentes não visitados na fila
                    visitar(i, visitados, numVertices); // Marca o vértice como visitado
                    enfileirar(Q, i);
                }
            }
        }

        // Procura vértice não visitado (grafo desconexo)
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                visitar(i, visitados, numVertices);
                enfileirar(Q, i);
                aux = true;
                break;
            }
        }

        if (!aux) { // Se todos os vértices foram marcados, sai do BFS
            destruirFila(Q);
            break;
        }
    }
}

// Verifica se existe caminho entre dois vértices (u -> w) usando BFS
bool existeCaminho(int u, int w, int G[][MAX_VERTICES], int numVertices) {
    bool visitados[MAX_VERTICES] = {false}; // Marca os vértices visitados
    Fila Q;
    criarFila(Q, numVertices);
    enfileirar(Q, u); // Enfileira o vértice de origem

    
    while(!vazia(Q)) { // Enquanto houver vértices na fila (caminhos para explorar)
        int t;
        desenfileirar(Q, t); // Desenfileira o primeiro vértice da fila

        if(t == w) { // Se chegou ao destino
            destruirFila(Q);
            return true;
        }

        if(!visitados[t]) { // Se o vértice ainda não foi visitado, é processado
            visitados[t] = true; // Vértice marcado no vetor de visitados
            for(int i = 0; i < numVertices; i++) { // Adiciona os vértices adjacentes não visitados na fila
                if(G[t][i] == 1 && !visitados[i]) {
                    enfileirar(Q, i);
                }
            }
        }
    }

    destruirFila(Q);
    return false; // Não há caminho de u até w
}

// Retorna verdadeiro se o vértice está dentro dos limites válidos.
bool verticeExiste(int v, int numVertices) {
    return v >= 0 && v < numVertices;
}

// ========= FUNÇÕES AUXILIARES =========
void inicializarGrafo(int G[][MAX_VERTICES], int numVertices) {
    for(int i = 0; i < numVertices; i++){ 
        for(int j = 0; j < numVertices; j++){
            G[i][j] = 0;
        }
    }
}

void lerArestas(int G[][MAX_VERTICES], int numVertices, bool dirigido) {
    cout << "Digite as arestas (u v). Digite 0 0 para finalizar:\n";
    while(true) {
        int u, v;
        cin >> u >> v;
        if(u == 0 && v == 0) break;
        u--; v--; // Ajusta para índice base 0
        if(verticeExiste(u, numVertices) && verticeExiste(v, numVertices)) {
            G[u][v] = 1;
            if (!dirigido) G[v][u] = 1;
        } else {
            cout << "Aresta inválida! Digite novamente.\n";
        }
    }
}

int lerNumeroVertices() {
    int numVertices;
    while(true) {
        cout << "Digite o número de vértices (max " << MAX_VERTICES << "): ";
        cin >> numVertices;
        if(!cin.fail() && numVertices > 0 && numVertices <= MAX_VERTICES) {
            return numVertices;
        }
        cout << "Número inválido. Tente novamente.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

bool lerDirecionado() {
    char resposta;
    do {
        cout << "O grafo é direcionado? (s/n): ";
        cin >> resposta;
        if(resposta == 's') return true;
        if(resposta == 'n') return false;
        cout << "Resposta inválida!\n";
    } while(true);
}

int lerVertice(int numVertices, const string& prompt) {
    int v;
    do {
        cout << prompt;
        cin >> v;
        v--;
        if(verticeExiste(v, numVertices)) return v;
        cout << "Vértice inválido. Tente novamente.\n";
    } while(true);
}

int main() {
    int grafo[MAX_VERTICES][MAX_VERTICES] = {{0}};
    bool visitados[MAX_VERTICES];
    int numVertices = lerNumeroVertices();
    bool dirigido = lerDirecionado();
    inicializarGrafo(grafo, numVertices);
    lerArestas(grafo, numVertices, dirigido);
    
    int opcao;
    do {
        clear();
        cout << "\n===== MENU =====\n";
        cout << "1. DFS\n";
        cout << "2. BFS\n";
        cout << "3. Pesquisar vértice\n";
        cout << "4. Pesquisar caminho entre dois vértices\n";
        cout << "5. Modificar grafo\n";
        cout << "6. Refazer grafo\n";
        cout << "7. Mostrar grafo\n";
        cout << "8. Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        switch(opcao) {
            case 1: {
                fill(visitados, visitados+numVertices, false);
                int v = lerVertice(numVertices, "Vértice inicial (1 a " + to_string(numVertices) + "): ");
                cout << "\nDFS:\n";
                DFS(v, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 2: {
                fill(visitados, visitados+numVertices, false);
                int v = lerVertice(numVertices, "Vértice inicial (1 a " + to_string(numVertices) + "): ");
                cout << "\nBFS:\n";
                BFS(v, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 3: {
                int v = lerVertice(numVertices, "Digite o vértice para pesquisar (1 a " + to_string(numVertices) + "): ");
                cout << "O vértice " << v+1 << " " << (verticeExiste(v, numVertices) ? "existe" : "não existe") << " no grafo.\n";
                pause();
                break;
            }
            case 4: {
                int u = lerVertice(numVertices, "Digite vértice origem: ");
                int w = lerVertice(numVertices, "Digite vértice destino: ");
                cout << (existeCaminho(u, w, grafo, numVertices) ? "Existe" : "Não existe") << " caminho de " << u+1 << " até " << w+1 << ".\n";
                pause();
                break;
            }
            case 5: {
                clear();
                int subopcao;
                cout << "\n--- Modificar Grafo ---\n";
                cout << "1. Adicionar aresta\n";
                cout << "2. Remover aresta\n";
                cout << "3. Adicionar vértice\n";
                cout << "4. Remover vértice\n";
                cout << "Escolha: ";
                cin >> subopcao;

                if(subopcao == 1) {
                    int u = lerVertice(numVertices, "Digite o vértice de origem: ");
                    int v = lerVertice(numVertices, "Digite o vértice de destino: ");
                    grafo[u][v] = 1;
                    if(!dirigido) grafo[v][u] = 1;
                    cout << "Aresta adicionada.\n";
                } 
                else if(subopcao == 2) {
                    int u = lerVertice(numVertices, "Digite o vértice de origem: ");
                    int v = lerVertice(numVertices, "Digite o vértice de destino: ");
                    grafo[u][v] = 0;
                    if(!dirigido) grafo[v][u] = 0;
                    cout << "Aresta removida.\n";
                }
                else if(subopcao == 3) {
                    if(numVertices < MAX_VERTICES) {
                        numVertices++;
                        for(int i = 0; i < numVertices; i++){
                            grafo[i][numVertices-1] = 0;
                            grafo[numVertices-1][i] = 0;
                        }
                        cout << "Vértice " << numVertices << " adicionado.\n";
                    } else {
                        cout << "Limite máximo de vértices atingido!\n";
                    }
                }
                else if(subopcao == 4) {
                    int v = lerVertice(numVertices, "Digite o vértice a remover: ");
                    for(int i = v; i < numVertices-1; i++){
                        for(int j = 0; j < numVertices; j++){
                            grafo[i][j] = grafo[i+1][j];
                            grafo[j][i] = grafo[j][i+1];
                        }
                    }
                    numVertices--;
                    cout << "Vértice removido.\n";
                } else {
                    cout << "Vértice inválido!\n";
                }
                pause();
                break;
            }
            case 6: {
                clear();
                cout << "\n--- Refazer Grafo ---\n";
                numVertices = lerNumeroVertices();
                dirigido = lerDirecionado();
                inicializarGrafo(grafo, numVertices);
                lerArestas(grafo, numVertices, dirigido);
                cout << "Novo grafo criado!\n";
                pause();
                break;
            }
            case 7: {
                cout << "\nMatriz de adjacência:\n";
                mostrarGrafo(grafo, numVertices);
                pause();
                break;
            }
            case 8: {
                cout << "Encerrando programa.\n";
                break;
            }
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }

    } while(opcao != 8);
    return 0;
}

//cd Trabalho1_DFS_BFS
//g++ trabalho1.cpp -o trabalho1.exe
//./trabalho1.exe