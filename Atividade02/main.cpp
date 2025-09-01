/*
cd Atividade02
g++ main.cpp -o grafos
./grafos
*/

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
bool foiVisitado(int v, int visitados[], int numVertices) {
    if (v < 0 || v >= numVertices) {
        cout << "Aviso: vértice inválido acessado!\n";
        return true; 
    }
    return visitados[v];
}

// DFS (busca em profundidade)
void DFS(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Pilha S;
    criarPilha(S, MAX_VERTICES);
    empilhar(S, v); // Empilha o vértice de origem
    cout << "\nDFS (profundidade) a partir do vertice " << v + 1 << "\n";

    while (true) {
        while (!vazia(S)) { // Enquanto houver vértices na pilha
            int atual;
            desempilhar(S, atual); // Desempilha o último vértice inserido na pilha;
            
            if (!foiVisitado(atual, visitados, numVertices)) { // Verifica se o vértice já foi visitado
                visitados[atual] = true; // Marca o vértice como visitado
                cout << "Visitando: " << atual + 1 << endl;
                for (int i = numVertices - 1; i >= 0; i--) { 
                    if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vértices adjacentes não visitados na pilha
                        empilhar(S, i);
                    }
                }
            }
        } 
 
        // Procura vértice não visitado (grafo desconexo)
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                empilhar(S, i);
                aux = 1;
                break;
            }
        }

        if (!aux) { // Se todos os vértices foram marcados, sai do DFS
            destruirPilha(S);
            break;
        }
    }
}

// BFS (busca em largura)
void BFS(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); // Enfileira o vértice de origem
    cout << "\nBFS (largura) a partir do vertice " << v + 1 << "\n";

    while (true) {
        while (!vazia(Q)){ // Enquanto houver vértices na fila
            int atual;
            desenfileirar(Q, atual); // Desenfileira o primeiro vértice da fila

            if (!foiVisitado(atual, visitados, numVertices)) { // Verifica se o vértice já foi visitado
                visitados[atual] = true; // Marca o vértice como visitado
                cout << "Visitando: " << atual + 1 << endl;
                for (int i = 0; i < numVertices; i++) { 
                    if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vértices adjacentes não visitados na pilha
                        enfileirar(Q, i);
                    }
                }
            }
        }

        // Procura vértice não visitado (grafo desconexo)
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                enfileirar(Q, i);
                aux = 1;
                break;
            }
        }

        if (!aux) { // Se todos os vértices foram marcados, sai do BFS
            destruirFila(Q);
            break;
        }
    }
}

// Fecho Transitivo Direto do vértice
void FTD(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); 

    cout << "\nFecho transitivo direto do vértice " << v + 1 << ":\n";
    visitados[v] = true; 

    while (!vazia(Q)) {
        int atual;
        desenfileirar(Q, atual);

        for (int i = 0; i < numVertices; i++) {
            if (G[atual][i] == 1 && !visitados[i]) {
                cout << "Alcançável: " << i + 1 << endl;
                visitados[i] = true;
                enfileirar(Q, i);
            }
        }
    }

    destruirFila(Q);
}

// Fecho Transitivo Inverso do vértice
void FTI(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); 

    cout << "\nFecho transitivo inverso do vértice " << v + 1 << ":\n";
    visitados[v] = true; 

    while (!vazia(Q)) {
        int atual;
        desenfileirar(Q, atual);

        for (int i = 0; i < numVertices; i++) {
            // Aqui a diferença: olhamos arestas que chegam em "atual"
            if (G[i][atual] == 1 && !visitados[i]) {
                cout << "Pode alcançar " << v + 1 << ": " << i + 1 << endl;
                visitados[i] = true;
                enfileirar(Q, i);
            }
        }
    }

    destruirFila(Q);
}

// Retorna verdadeiro se o vértice está dentro dos limites válidos.
bool verticeExiste(int v, int numVertices) {
    return v >= 0 && v < numVertices;
}

// Verifica a conectividade do grafo
void conectividade(int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    // Testa FTD do vértice 0
    fill(visitados, visitados + numVertices, false);
    FTD(0, G, numVertices, visitados);

    bool todosFTD = true; // Assume que passou por todos os vértices do grafo
    for (int i = 0; i < numVertices; i++) { // Verifica
        if (!visitados[i]) {
            todosFTD = false; // Se não passou por algum -> false
            break;
        }
    }

    // Testa FTI do vértice 0
    fill(visitados, visitados + numVertices, false);
    FTI(0, G, numVertices, visitados);

    bool todosFTI = true; // Assume que passou por todos os vértices do grafo
    for (int i = 0; i < numVertices; i++) {
        if (!visitados[i]) {
            todosFTI = false; // Se não passou por algum -> false
            break;
        }
    }

    if (todosFTD && todosFTI) { // Se é fortemente conexo
        cout << "\nO grafo e fortemente conexo!\n";
    } else {
        subgrafosFCM();
    }
}

// Encontra os subgrafos Fortemente Conexos Máximos
void subgrafosFCM(int G[][MAX_VERTICES], int numVertices, int visitados[]) {
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
        if(u == 0 || v == 0) break;
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

int lerVertice(int numVertices, string prompt) {
    int v;
    do {
        cout << prompt;
        cin >> v;
        v--;
        if(verticeExiste(v, numVertices)) 
            return v;
        cout << "Vértice inválido. Tente novamente.\n";
    } while(true);
}

int main() {
    int grafo[MAX_VERTICES][MAX_VERTICES] = {{0}};
    int visitados[MAX_VERTICES];
    int numVertices = lerNumeroVertices();
    bool dirigido = lerDirecionado();
    inicializarGrafo(grafo, numVertices);
    lerArestas(grafo, numVertices, dirigido);
    
    int opcao;
    do {
        clear();
        cout << "\nMatriz de adjacência:\n";
        mostrarGrafo(grafo, numVertices);
        cout << "\n===== MENU =====\n";
        cout << "1. DFS\n";
        cout << "2. BFS\n";
        cout << "3. Pesquisar vértice\n";
        cout << "4. Fecho transitivo direto de um vértice\n";
        cout << "5. Fecho transitivo inverso de um vértice\n";
        cout << "6. Conexividade\n";
        cout << "7. Modificar grafo\n";
        cout << "8. Refazer grafo\n";
        cout << "9. Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        switch(opcao) {
            case 1: { // DFS
                fill(visitados, visitados+numVertices, false);
                cout << "\nDigite o vertice inicial para o DFS (1 a " << numVertices <<"): ";
                int v;
                cin >> v;
                while (v < 1 || v > numVertices) {
                    cout << "Valor invalido. Tente novamente: ";
                    cin >> v;
                }
                DFS(v-1, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 2: { // BFS
                fill(visitados, visitados+numVertices, false);
                cout << "\nDigite o vertice inicial para o BFS (1 a " << numVertices <<"): ";
                int v;
                cin >> v;
                while (v < 1 || v > numVertices) {
                    cout << "Valor invalido. Tente novamente: ";
                    cin >> v;
                }
                BFS(v-1, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 3: { // PESQUISAR VÉRTICE
                int v;
                cout << "Digite o vértice que deseja pesquisar: ";
                cin >> v;
                if (v >= 1 && v <= numVertices)
                    cout << "O vertice " << v << " existe no grafo.\n";
                else
                    cout << "O vertice " << v << " nao existe no grafo.\n";
                pause();
                break;
            }
            case 4: { // FECHO TRANSITIVO DIRETO
                fill(visitados, visitados+numVertices, false);
                cout << "\nDigite o vertice inicial para o FTD (1 a " << numVertices <<"): ";
                int v;
                cin >> v;
                while (v < 1 || v > numVertices) {
                    cout << "Valor invalido. Tente novamente: ";
                    cin >> v;
                }
                FTD(v-1, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 5: { // FECHO TRANSITIVO INVERSO
                fill(visitados, visitados+numVertices, false);
                cout << "\nDigite o vertice inicial para o FTI (1 a " << numVertices <<"): ";
                int v;
                cin >> v;
                while (v < 1 || v > numVertices) {
                    cout << "Valor invalido. Tente novamente: ";
                    cin >> v;
                }
                FTI(v, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 6: { // CONEXIVIDADE
                FTI(grafo, numVertices, visitados);
                pause();
                break;
            }
            case 7: { // MODIFICAR GRAFO
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
            case 8: { // REFAZER GRAFO
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
            case 9: { // SAIR
                cout << "Encerrando programa.\n";
                break;
            }
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }

    } while(opcao != 9);
    return 0;
}