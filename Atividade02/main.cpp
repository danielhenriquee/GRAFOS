
#include "pilha.h"
#include "fila.h"
#include <limits>
#include <iostream>
using namespace std;

const int MAX_VERTICES = 10;

// Códigos de escape ANSI para cores (para exibir a matriz)
constexpr const char* RED = "\033[31m";   // Vermelho
constexpr const char* GREEN = "\033[32m"; // Verde
constexpr const char* RESET = "\033[0m";  // Resetar cor


// Exibe a matriz de adjacência com cores.
// 0 (sem aresta) = vermelho
// 1 (com aresta) = verde
void mostrarGrafo(int G[][MAX_VERTICES], int numVertices){
    for(int i = 0; i < numVertices; i++){
        for(int j = 0; j < numVertices; j++){
            if (G[i][j] == 0) {
                // Vermelho para aresta inexistente
                cout << RED << G[i][j] << RESET << " ";
            } else {
                // Verde para aresta existente
                cout << GREEN << G[i][j] << RESET << " ";
            }
        }
        cout << "\n";
    }
}


// Retorna "true" se o vértice já foi visitado. Se o índice for inválido, emite um aviso e retorna "true" para evitar acessos fora do vetor.
bool foiVisitado(int v, bool visitados[], int numVertices){
    if (v < 0 || v >= numVertices) {
        cout << "Aviso: vértice inválido acessado!\n";
        return true; 
    }
    return visitados[v];
}


// Marca o vértice como visitado e imprime a ação.
void visitar(int v, bool visitados[], int numVertices){
    if (v < 0 || v >= numVertices) {
        cout << "Vértice inválido!\n";
        return;
    }
    cout << "Visitando: " << v+1 << endl;
    visitados[v] = true;
}

// Função: DFS (busca em profundidade)
// Estratégia LIFO (último que entra é o primeiro que sai).
void DFS(int v, int G[][MAX_VERTICES], int numVertices, bool visitados[]){
    Pilha S;
    criarPilha(S, MAX_VERTICES);

    // Começamos empilhando o vértice inicial
    empilhar(S, v);

    // Enquanto houver vértices na pilha
    while(!vazia(S)){
        int atual;
        desempilhar(S, atual); // Pega o último vértice inserido

        // Se ainda não foi visitado, processamos ele
        if(!foiVisitado(atual, visitados, numVertices)){
            visitar(atual, visitados, numVertices);

            // Adiciona vizinhos do vértice atual na pilha
            // Se houver aresta (atual -> e) e o vizinho não foi visitado
            for(int e = numVertices - 1; e >= 0; e--){
                if(G[atual][e] == 1 && !foiVisitado(e, visitados, numVertices)){
                    empilhar(S, e);
                }
            }
        } 
    } 
    destruirPilha(S); // Libera memória da pilha
}

// Função: BFS (busca em largura)
// Estratégia FIFO (primeiro que entra é o primeiro que sai).
void BFS(int v, int G[][MAX_VERTICES], int numVertices, bool visitados[]){
    Fila Q;
    criarFila(Q, MAX_VERTICES);

    // Coloca o vértice inicial na fila
    enfileirar(Q, v);

    // Enquanto houver vértices na fila
    while(!vazia(Q)){
        int t;
        desenfileirar(Q, t); // Retira o primeiro da fila

        // Se ainda não visitado, processamos
        if(!foiVisitado(t, visitados, numVertices)){
            visitar(t, visitados, numVertices);

            // Adiciona todos os vizinhos não visitados na fila
            for(int i=0; i<numVertices; i++){
                if(G[t][i]==1 && !foiVisitado(i, visitados, numVertices))
                    enfileirar(Q, i);
            }
        }
    }
    destruirFila(Q); // Libera memória da fila
}


// Verifica se existe caminho entre dois vértices (u -> w) usando BFS.
bool existeCaminho(int u, int w, int G[][MAX_VERTICES], int numVertices){
    bool visitados[MAX_VERTICES] = {false}; // Marca quem já foi visitado
    Fila Q;
    criarFila(Q, numVertices);

    // Inicia a busca com o vértice u
    enfileirar(Q, u);

    // Enquanto houver vértices para explorar
    while(!vazia(Q)){
        int t;
        desenfileirar(Q, t);

        // Se chegamos ao destino, já podemos encerrar
        if(t == w){
            destruirFila(Q);
            return true;
        }

        // Se ainda não foi visitado, processamos
        if(!visitados[t]){
            visitados[t] = true;

            // Adiciona todos os vizinhos não visitados
            for(int i = 0; i < numVertices; i++){
                if(G[t][i] == 1 && !visitados[i]){
                    enfileirar(Q, i);
                }
            }
        }
    }

    destruirFila(Q);
    return false; // Não chegamos ao destino
}

// Retorna verdadeiro se o vértice está dentro dos limites válidos.
bool verticeExiste(int v, int numVertices) {
    return v >= 0 && v < numVertices;
}

int main() {
    int grafo[MAX_VERTICES][MAX_VERTICES] = {{0}}; // Matriz de adjacência
    int numVertices;
    bool visitados[MAX_VERTICES]; // Vetor para marcar vértices visitados

    // Entrada: número de vértices 
    do {
        cout << "Digite o número de vértices (max " << MAX_VERTICES << "): ";
        cin >> numVertices;

        if(cin.fail() || numVertices <= 0 || numVertices > MAX_VERTICES){
            cout << "Número inválido. Tente novamente." << endl;
            cin.clear(); // Limpa estado de erro
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta lixo no buffer
        } else break;
    } while(true);

    // Inicializa a matriz com 0
    for(int i=0; i<numVertices; i++)
        for(int j=0; j<numVertices; j++)
            grafo[i][j] = 0;

    // Entrada: grafo direcionado ou não 
    bool dirigido;
    char resposta;
    do{
        cout << "O grafo é direcionado? (s/n): ";
        cin >> resposta;
        if(resposta == 's'){
            dirigido = true;
        } else if(resposta == 'n'){
            dirigido = false;
        } else {
            cout<<"Resposta inválida"<<endl;
        }
    }while(resposta != 's' && resposta != 'n');
    
    // Entrada: número de arestas 
    int numArestas;
    cout << "Digite o número de arestas: ";
    cin >> numArestas;

    //  Entrada: leitura das arestas
    cout << "Digite as arestas (u v):" << endl;
    for(int k = 0; k < numArestas; k++){
        int u, v;
        cin >> u >> v;
        u--; v--; // Ajusta para índice base 0

        // Verifica validade antes de inserir
        if(verticeExiste(u, numVertices) && verticeExiste(v, numVertices)){
            grafo[u][v] = 1;
            if(!dirigido) // Se não for direcionado, adiciona na outra direção também
                grafo[v][u] = 1;
        } else {
            cout << "Aresta inválida! Ignorada.\n";
            k--; // Força o usuário a digitar de novo
        }
    }

    // Menu de operações 
    int opcao;
    do {
        cout << "\n MENU \n";
        cout << "1. DFS\n";
        cout << "2. BFS\n";
        cout << "3. Pesquisar caminho entre dois vértices\n";
        cout << "4. Mostrar grafo\n";
        cout << "5. Pesquisar vértice\n";
        cout << "6. Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        switch(opcao){
            case 1: {
                // Executa DFS
                int v;
                cout << "Vértice inicial (1 a " << numVertices << "): ";
                cin >> v;
                v--; 
                fill(visitados, visitados+numVertices, false); // Zera visitados
                cout << "\n DFS \n";
                DFS(v, grafo, numVertices, visitados);

                // Caso o grafo seja desconexo, percorremos os outros vértices
                for(int i = 0; i < numVertices; i++){
                    if(!visitados[i])
                        DFS(i, grafo, numVertices, visitados);
                }
                break;
            }
            case 2: {
                // Executa BFS
                int v;
                cout << "Vértice inicial (1 a " << numVertices << "): ";
                cin >> v;
                v--; 
                fill(visitados, visitados+numVertices, false); // Zera visitados
                cout << "\n BFS \n";
                BFS(v, grafo, numVertices, visitados);

                // Caso o grafo seja desconexo
                for(int i = 0; i < numVertices; i++){
                    if(!visitados[i])
                        BFS(i, grafo, numVertices, visitados);
                }
                break;
            }
            case 3: {
                // Pesquisa caminho entre dois vértices
                int u, w;
                cout << "Digite os vértices origem e destino: ";
                cin >> u >> w;
                u--; w--;
                if(existeCaminho(u, w, grafo, numVertices))
                    cout << "Existe caminho de " << u+1 << " até " << w+1 << ".\n";
                else
                    cout << "Não existe caminho de " << u+1 << " até " << w+1 << ".\n";
                break;
            }
            case 4: {
                // Exibe a matriz do grafo
                cout << "\n MATRIZ DO GRAFO \n";
                mostrarGrafo(grafo, numVertices);
                break;
            }
            case 5: {
                // Verifica se um vértice existe
                int v;
                cout << "Digite o vértice para pesquisar (1 a " << numVertices << "): ";
                cin >> v;
                v--; 
                if(verticeExiste(v, numVertices))
                    cout << "O vértice " << v+1 << " existe no grafo.\n";
                else
                    cout << "O vértice " << v+1 << " não existe no grafo.\n";
                break;
            }
            case 6:
                cout << "Encerrando programa.\n";
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }

    } while(opcao != 6);

    return 0;
}


//cd Trabalho1_DFS_BFS
//g++ trabalho1.cpp -o trabalho1.exe
//./trabalho1.exe
