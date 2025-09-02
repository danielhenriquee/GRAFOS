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
// Exibe a matriz de adjacência com indices e bordas
void mostrarGrafo(int G[][MAX_VERTICES], int numVertices) {
    // Cabeçalho com índices das colunas
    cout << "     ";
    for(int j = 0; j < numVertices; j++) {
        cout << j+1 << " ";
    }
    cout << "\n   +";
    for(int j = 0; j < numVertices; j++) {
        cout << "--";
    }
    cout << "-+\n";

    // Linhas da matriz com índices
    for(int i = 0; i < numVertices; i++) {
        cout << (i+1 < 10 ? " " : "") << i+1 << " | "; // índice da linha
        for(int j = 0; j < numVertices; j++) {
            if (G[i][j] == 0) {
                cout << RED << G[i][j] << RESET << " ";
            } else {
                cout << GREEN << G[i][j] << RESET << " ";
            }
        }
        cout << "|\n";
    }

    // Rodapé da matriz
    cout << "   +";
    for(int j = 0; j < numVertices; j++) {
        cout << "--";
    }
    cout << "-+\n";
}

// Retorna "true" se o vertice ja foi visitado. Se o índice for invalido, emite um aviso e retorna "true" para evitar acessos fora do vetor.
bool foiVisitado(int v, int visitados[], int numVertices) {
    if (v < 0 || v >= numVertices) {
        cout << "Aviso: vertice invalido acessado!\n";
        return true; 
    }
    return visitados[v];
}

// DFS (busca em profundidade)
void DFS(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Pilha S;
    criarPilha(S, MAX_VERTICES);
    empilhar(S, v); // Empilha o vertice de origem
    cout << "\nDFS (profundidade) a partir do vertice " << v + 1 << "\n";

    while (true) {
        while (!vazia(S)) { // Enquanto houver vertices na pilha
            int atual;
            desempilhar(S, atual); // Desempilha o último vertice inserido na pilha;
            
            if (!foiVisitado(atual, visitados, numVertices)) { // Verifica se o vertice ja foi visitado
                visitados[atual] = true; // Marca o vertice como visitado
                cout << "Visitando: " << atual + 1 << endl;
                for (int i = numVertices - 1; i >= 0; i--) { 
                    if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vertices adjacentes nao visitados na pilha
                        empilhar(S, i);
                    }
                }
            }
        } 
 
        // Procura vertice nao visitado (grafo desconexo)
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                empilhar(S, i);
                aux = 1;
                break;
            }
        }

        if (!aux) { // Se todos os vertices foram marcados, sai do DFS
            destruirPilha(S);
            break;
        }
    }
}

// BFS (busca em largura)
void BFS(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); // Enfileira o vertice de origem
    cout << "\nBFS (largura) a partir do vertice " << v + 1 << "\n";

    while (true) {
        while (!vazia(Q)){ // Enquanto houver vertices na fila
            int atual;
            desenfileirar(Q, atual); // Desenfileira o primeiro vertice da fila

            if (!foiVisitado(atual, visitados, numVertices)) { // Verifica se o vertice ja foi visitado
                visitados[atual] = true; // Marca o vertice como visitado
                cout << "Visitando: " << atual + 1 << endl;
                for (int i = 0; i < numVertices; i++) { 
                    if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vertices adjacentes nao visitados na pilha
                        enfileirar(Q, i);
                    }
                }
            }
        }

        // Procura vertice nao visitado (grafo desconexo)
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                enfileirar(Q, i);
                aux = 1;
                break;
            }
        }

        if (!aux) { // Se todos os vertices foram marcados, sai do BFS
            destruirFila(Q);
            break;
        }
    }
}

// Fecho Transitivo Direto do vertice
void FTD(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    fill(visitados, visitados+numVertices, -1); // -1 = nao visitado
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); 
    visitados[v] = 0;

    while (!vazia(Q)) {
        int atual;
        desenfileirar(Q, atual);

        for (int i = 0; i < numVertices; i++) {
            if (G[atual][i] == 1 && visitados[i] == -1) { // Não visitados
                visitados[i] = visitados[atual] + 1;
                enfileirar(Q, i);
            }
        }
    }
    destruirFila(Q);
}

// Chama a funçao que processa, esta apenas imprime
void FTD_show(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    FTD(v, G, numVertices, visitados);

    cout << "\nFecho transitivo direto do vertice " << v + 1 << ":\n";
    for (int i = 0; i < numVertices; i++) {
        if (i != v && visitados[i] > 0) {
            cout << "Alcançavel: " << i+1 << " (distancia: " << visitados[i] << " passos)" << endl;
        }
    }
}

// Fecho Transitivo Inverso do vertice
void FTI(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    fill(visitados, visitados+numVertices, -1); // -1 = nao visitado
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); 
    visitados[v] = 0; 

    while (!vazia(Q)) {
        int atual;
        desenfileirar(Q, atual);

        for (int i = 0; i < numVertices; i++) {
            if (G[i][atual] == 1 && visitados[i] == -1) { // Nao visitado
                visitados[i] = visitados[atual] + 1;
                enfileirar(Q, i);
            }
        }
    }
    destruirFila(Q);
}

// Chama a funçao que processa, esta apenas imprime
void FTI_show(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    FTI(v, G, numVertices, visitados);

    cout << "\nFecho transitivo inverso do vertice " << v + 1 << ":\n";
    for (int i = 0; i < numVertices; i++) {
        if (i != v && visitados[i] > 0) {
            cout << "O vértice " << i+1 << " alcança (distancia: " << visitados[i] << " passos)" << endl;
        }
    }
}

// Retorna verdadeiro se o vertice esta dentro dos limites validos.
bool verticeExiste(int v, int numVertices) {
    return v >= 0 && v < numVertices;
}

// Encontra os subgrafos Fortemente Conexos Maximos
void subgrafosFCM(int G[][MAX_VERTICES], int numVertices) {
    bool comSubgrafos[MAX_VERTICES] = {false}; // Marca os vertices que ja possuem subgrafos

    cout << "\nSubgrafos Fortemente Conexos Maximos:\n";

    for (int v = 0; v < numVertices; v++) { // Iteraçao por todos os vertices
        if (!comSubgrafos[v]) {
            int ftd[MAX_VERTICES]; // Marca os vertices alcançaveis por v
            int fti[MAX_VERTICES]; // Marca os vertices que alcançam v
            fill(ftd, ftd + numVertices, -1);
            fill(fti, fti + numVertices, -1);

            // Processa os fechos
            FTD(v, G, numVertices, ftd);
            FTI(v, G, numVertices, fti);

            // Interseçao FTD ∩ FTI
            cout << "{ ";
            for (int i = 0; i < numVertices; i++) {
                if (ftd[i] >= 0 && fti[i] >= 0) {
                    cout << i+1 << " ";
                    comSubgrafos[i] = true;
                }
            }
            cout << "}\n";
        }
    }
}

// Verifica a conectividade do grafo
void conectividade(int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    if (numVertices == 0) { 
        cout << "O grafo esta vazio.\n"; 
        return;
    }

    // Testa FTD do vertice 0
    fill(visitados, visitados + numVertices, false);
    FTD(0, G, numVertices, visitados);
    bool todosFTD = true; // Assume que passou por todos os vertices do grafo
    for (int i = 0; i < numVertices; i++) { // Verifica
        if (!visitados[i]) {
            todosFTD = false; // Se nao passou por algum -> false
            break;
        }
    }

    // Testa FTI do vertice 0
    fill(visitados, visitados + numVertices, false);
    FTI(0, G, numVertices, visitados);
    bool todosFTI = true; // Assume que passou por todos os vertices do grafo
    for (int i = 0; i < numVertices; i++) {
        if (!visitados[i]) {
            todosFTI = false; // Se nao passou por algum -> false
            break;
        }
    }

    if (todosFTD && todosFTI) { // Se e fortemente conexo
        cout << "\nO grafo e fortemente conexo!\n";
    } else {
        subgrafosFCM(G, numVertices);
    }
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
    cout << "Digite as arestas (u v). Digite um dos vertices como 0 para finalizar:\n";
    while(true) {
        int u, v;
        cin >> u >> v;
        if(u == 0 || v == 0) break;
        u--; v--; // Ajusta para índice base 0
        if(verticeExiste(u, numVertices) && verticeExiste(v, numVertices)) {
            G[u][v] = 1;
            if (!dirigido) G[v][u] = 1;
        } else {
            cout << "Aresta invalida! Digite novamente.\n";
        }
    }
}

int lerNumeroVertices() {
    int numVertices;
    while(true) {
        cout << "Digite o número de vertices (max " << MAX_VERTICES <<"): ";
        cin >> numVertices;
        if(!cin.fail() && numVertices > 0 && numVertices <= MAX_VERTICES) {
            return numVertices;
        }
        cout << "Número invalido. Tente novamente.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

bool lerDirecionado() {
    char resposta;
    do {
        cout << "O grafo e direcionado? (s/n): ";
        cin >> resposta;
        if(resposta == 's') return true;
        if(resposta == 'n') return false;
        cout << "Resposta invalida!\n";
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
        cout << "Vertice invalido. Tente novamente.\n";
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
        cout << "3. Pesquisar vertice\n";
        cout << "4. Fecho transitivo direto de um vertice\n";
        cout << "5. Fecho transitivo inverso de um vertice\n";
        cout << "6. Conectividade\n";
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
            case 3: { // PESQUISAR VeRTICE
                int v;
                cout << "Digite o vertice de origem (1 ate " << numVertices <<"): ";
                cin >> v;
                if (v >= 1 && v <= numVertices)
                    cout << "O vertice " << v << " existe no grafo.\n";
                else
                    cout << "O vertice " << v << " nao existe no grafo.\n";
                pause();
                break;
            }
            case 4: { // FECHO TRANSITIVO DIRETO
                cout << "\nDigite o vertice inicial para o FTD (1 a " << numVertices <<"): ";
                int v;
                cin >> v;
                while (v < 1 || v > numVertices) {
                    cout << "Valor invalido. Tente novamente: ";
                    cin >> v;
                }
                FTD_show(v-1, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 5: { // FECHO TRANSITIVO INVERSO
                cout << "\nDigite o vertice inicial para o FTI (1 a " << numVertices <<"): ";
                int v;
                cin >> v;
                while (v < 1 || v > numVertices) {
                    cout << "Valor invalido. Tente novamente: ";
                    cin >> v;
                }
                FTI_show(v-1, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 6: { // CONECTIVIDADE
                conectividade(grafo, numVertices, visitados);
                pause();
                break;
            }
            case 7: { // MODIFICAR GRAFO
                int subopcao;
                cout << "\n--- Modificar Grafo ---\n";
                cout << "1. Adicionar aresta\n";
                cout << "2. Remover aresta\n";
                cout << "3. Adicionar vertice\n";
                cout << "4. Remover vertice\n";
                cout << "Escolha: ";
                cin >> subopcao;

                if(subopcao == 1) {
                    lerArestas(grafo, numVertices, dirigido);
                    fill(visitados, visitados + MAX_VERTICES, false);
                } else if(subopcao == 2) {
                    cout << "Digite as arestas a remover (u v). Digite 0 em qualquer vértice para finalizar:\n";
                    while(true) {
                        int u, v;
                        cin >> u >> v;
                        if(u == 0 || v == 0) break;
                        u--; v--; // Ajusta para índice base 0
                        if(verticeExiste(u, numVertices) && verticeExiste(v, numVertices)) {
                            grafo[u][v] = 0;
                            if(!dirigido) grafo[v][u] = 0;
                        } else {
                            cout << "Aresta inválida! Digite novamente.\n";
                        }
                    }
                    fill(visitados, visitados + MAX_VERTICES, false);
                } else if(subopcao == 3) { // Adicionar vertices
                    int qtd;
                    cout << "Quantos vertices deseja adicionar? ";
                    cin >> qtd;

                    if(qtd <= 0) {
                        cout << "Quantidade invalida!\n";
                    } else if(numVertices + qtd > MAX_VERTICES) {
                        cout << "Nao e possível adicionar. Limite maximo de " << MAX_VERTICES << " vertices atingido!\n";
                        cout << "Você pode adicionar ate " << (MAX_VERTICES - numVertices) << " vertices.\n";
                    } else {
                        for(int k = 0; k < qtd; k++) {
                            numVertices++;
                            for(int i = 0; i < numVertices; i++){
                                grafo[i][numVertices-1] = 0;
                                grafo[numVertices-1][i] = 0;
                            }
                            cout << "Vertice " << numVertices << " adicionado.\n";
                        }
                    }
                    fill(visitados, visitados + MAX_VERTICES, false);
                } else if(subopcao == 4) {
                    int v = lerVertice(numVertices, "Digite o vertice a remover: ");
                    for(int i = v; i < numVertices-1; i++){
                        for(int j = 0; j < numVertices; j++){
                            grafo[i][j] = grafo[i+1][j];
                            grafo[j][i] = grafo[j][i+1];
                        }
                    }
                    numVertices--;
                    cout << "Vertice removido.\n";
                    fill(visitados, visitados + MAX_VERTICES, false);
                } else {
                    cout << "Opcao invalida!\n";
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
                cout << "Opçao invalida! Tente novamente.\n";
        }

    } while(opcao != 9);
    return 0;
}