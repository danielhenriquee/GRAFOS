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

// Define o número máximo de vértices que o grafo pode ter.
const int MAX_VERTICES = 100;

// Códigos de escape ANSI para cores (para exibir a matriz de adjacência de forma mais legível).
constexpr const char* RED = "\033[31m";   // Vermelho para arestas ausentes (0)
constexpr const char* GREEN = "\033[32m"; // Verde para arestas existentes (1)
constexpr const char* RESET = "\033[0m";  // Reseta a cor para o padrão do terminal


void clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void pause() {
    cout << "\nPressione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa o buffer de entrada antes de esperar
    cin.get();
}


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


bool foiVisitado(int v, int visitados[], int numVertices) {
    if (v < 0 || v >= numVertices) {
        cout << "Aviso: vertice invalido acessado!\n";
        return true; 
    }
    return visitados[v];
}


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
                // Empilha os vizinhos não visitados em ordem inversa para visitar em ordem crescente
                for (int i = numVertices - 1; i >= 0; i--) { 
                    if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vertices adjacentes nao visitados na pilha
                        empilhar(S, i);
                    }
                }
            }
        } 
 
        // Se o grafo for desconexo, procura por um vértice ainda não visitado para continuar a busca.
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                empilhar(S, i);
                aux = true;
                break;
            }
        }

        if (!aux) { // Se todos os vertices foram marcados, a busca está completa.
            destruirPilha(S);
            break;
        }
    }
}


void BFS(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); // Enfileira o vertice de origem
    cout << "\nBFS (largura) a partir do vertice " << v + 1 << "\n";

    while (true) {
        while (!vazia(Q)){
            int atual;
            desenfileirar(Q, atual); // Desenfileira o primeiro vertice da fila

            if (!foiVisitado(atual, visitados, numVertices)) { // Verifica se o vertice ja foi visitado
                visitados[atual] = true; // Marca o vertice como visitado
                cout << "Visitando: " << atual + 1 << endl;
                // Enfileira todos os vizinhos não visitados.
                for (int i = 0; i < numVertices; i++) { 
                    if (G[atual][i] == 1 && !foiVisitado(i, visitados, numVertices)) { // Adiciona os vertices adjacentes nao visitados na pilha
                        enfileirar(Q, i);
                    }
                }
            }
        }

        // Se o grafo for desconexo, procura por um vértice ainda não visitado para continuar a busca.
        bool aux = false;
        for (int i = 0; i < numVertices; i++) {
            if (!visitados[i]) {
                enfileirar(Q, i);
                aux = true;
                break;
            }
        }

        if (!aux) { // Se todos os vertices foram marcados, a busca está completa.
            destruirFila(Q);
            break;
        }
    }
}

void FTD(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    fill(visitados, visitados+numVertices, -1); // -1 = nao visitado
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); 
    visitados[v] = 0; // Distância do vértice para ele mesmo é 0

    while (!vazia(Q)) {
        int atual;
        desenfileirar(Q, atual);

        for (int i = 0; i < numVertices; i++) {
            if (G[atual][i] == 1 && visitados[i] == -1) { // Se é um vizinho e ainda não foi visitado
                visitados[i] = visitados[atual] + 1; // Marca a distância
                enfileirar(Q, i);
            }
        }
    }
    destruirFila(Q);
}


void FTD_show(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    FTD(v, G, numVertices, visitados);

    cout << "\nFecho transitivo direto do vertice " << v + 1 << ":\n";
    bool encontrou = false;
    for (int i = 0; i < numVertices; i++) {
        if (i != v && visitados[i] > 0) {
            cout << "Alcançavel: " << i+1 << " (distancia: " << visitados[i] << " passos)" << endl;
            encontrou = true;
        }
    }
    if (!encontrou) {
        cout << "Nenhum outro vertice e alcançavel a partir de " << v + 1 << ".\n";
    }
}


void FTI(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    fill(visitados, visitados+numVertices, -1); // -1 = nao visitado
    Fila Q;
    criarFila(Q, MAX_VERTICES);
    enfileirar(Q, v); 
    visitados[v] = 0; 

    while (!vazia(Q)) {
        int atual;
        desenfileirar(Q, atual);

        // A lógica é invertida: procuramos quem tem 'atual' como vizinho
        for (int i = 0; i < numVertices; i++) {
            if (G[i][atual] == 1 && visitados[i] == -1) { // Se 'i' alcança 'atual' e não foi visitado
                visitados[i] = visitados[atual] + 1;
                enfileirar(Q, i);
            }
        }
    }
    destruirFila(Q);
}


void FTI_show(int v, int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    FTI(v, G, numVertices, visitados);

    cout << "\nFecho transitivo inverso do vertice " << v + 1 << ":\n";
    bool encontrou = false;
    for (int i = 0; i < numVertices; i++) {
        if (i != v && visitados[i] > 0) {
            cout << "O vertice " << i+1 << " alcança " << v + 1 << " (distancia: " << visitados[i] << " passos)" << endl;
            encontrou = true;
        }
    }
    if (!encontrou) {
        cout << "Nenhum outro vertice alcança " << v + 1 << ".\n";
    }
}


bool verticeExiste(int v, int numVertices) {
    return v >= 0 && v < numVertices;
}


void subgrafosFCM(int G[][MAX_VERTICES], int numVertices) {
    bool comSubgrafos[MAX_VERTICES] = {false}; // Marca os vertices que ja pertencem a um subgrafo exibido

    cout << "\nSubgrafos Fortemente Conexos Maximos:\n";

    for (int v = 0; v < numVertices; v++) { // Itera por todos os vertices
        if (!comSubgrafos[v]) {
            int ftd[MAX_VERTICES]; // Vértices alcançáveis por v
            int fti[MAX_VERTICES]; // Vértices que alcançam v
            
            // Calcula os fechos transitivos
            FTD(v, G, numVertices, ftd);
            FTI(v, G, numVertices, fti);

            // A interseção do FTD e FTI de um vértice 'v' forma um subgrafo fortemente conexo.
            cout << "{ ";
            for (int i = 0; i < numVertices; i++) {
                if (ftd[i] >= 0 && fti[i] >= 0) { // Se i é alcançável por v E v é alcançável por i
                    cout << i+1 << " ";
                    comSubgrafos[i] = true; // Marca o vértice para não processá-lo novamente
                }
            }
            cout << "}\n";
        }
    }
}


void conectividade(int G[][MAX_VERTICES], int numVertices, int visitados[]) {
    if (numVertices == 0) { 
        cout << "O grafo esta vazio.\n"; 
        return;
    }

    // 1. Testa se todos os vértices são alcançáveis a partir de um vértice inicial (ex: 0).
    fill(visitados, visitados + numVertices, false);
    FTD(0, G, numVertices, visitados);
    bool todosAlcancaveis = true;
    for (int i = 0; i < numVertices; i++) {
        if (visitados[i] == -1) { // FTD usa -1 para não visitado
            todosAlcancaveis = false;
            break;
        }
    }

    // 2. Testa se o vértice inicial (0) é alcançável por todos os outros vértices.
    fill(visitados, visitados + numVertices, false);
    FTI(0, G, numVertices, visitados);
    bool todosAlcancam = true;
    for (int i = 0; i < numVertices; i++) {
        if (visitados[i] == -1) {
            todosAlcancam = false;
            break;
        }
    }

    // Um grafo é fortemente conexo se, de um vértice qualquer, todos os outros são alcançáveis,
    // e este vértice é alcançável por todos os outros.
    if (todosAlcancaveis && todosAlcancam) {
        cout << "\nO grafo e fortemente conexo!\n";
    } else {
        // Se não for fortemente conexo, encontramos os componentes (subgrafos).
        cout << "\nO grafo não e fortemente conexo!\n";
        subgrafosFCM(G, numVertices);
    }
}

// ========= FUNÇÕES AUXILIARES DE ENTRADA E MANIPULAÇÃO =========


void inicializarGrafo(int G[][MAX_VERTICES], int numVertices) {
    for(int i = 0; i < numVertices; i++){
        for(int j = 0; j < numVertices; j++){
            G[i][j] = 0;
        }
    }
}


void lerArestas(int G[][MAX_VERTICES], int numVertices, bool dirigido) {
    cout << "Digite as arestas no formato 'u v'. Digite 0 em um dos vertices para finalizar:\n";
    while(true) {
        int u, v;
        cin >> u >> v;
        if(u == 0 || v == 0) break;
        u--; v--; // Ajusta para índice base 0 (usuário digita de 1 a N)
        if(verticeExiste(u, numVertices) && verticeExiste(v, numVertices)) {
            G[u][v] = 1;
            if (!dirigido) G[v][u] = 1; // Se não for dirigido, a aresta é de mão dupla
        } else {
            cout << "Aresta invalida! Vertices fora do intervalo. Digite novamente.\n";
        }
    }
}


int lerNumeroVertices() {
    int numVertices;
    while(true) {
        cout << "Digite o numero de vertices (max " << MAX_VERTICES << "): ";
        cin >> numVertices;
        if(!cin.fail() && numVertices > 0 && numVertices <= MAX_VERTICES) {
            return numVertices;
        }
        cout << "Numero invalido. Tente novamente.\n";
        cin.clear(); // Limpa o estado de erro do cin
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta a entrada inválida
    }
}


bool lerDirecionado() {
    char resposta;
    do {
        cout << "O grafo e direcionado? (s/n): ";
        cin >> resposta;
        resposta = tolower(resposta);
        if(resposta == 's') return true;
        if(resposta == 'n') return false;
        cout << "Resposta invalida! Use 's' para sim ou 'n' para nao.\n";
    } while(true);
}


int lerVertice(int numVertices, string prompt) {
    int v;
    do {
        cout << prompt;
        cin >> v;
        v--; // Ajusta para base 0
        if(verticeExiste(v, numVertices)) 
            return v;
        cout << "Vertice invalido. Tente novamente.\n";
    } while(true);
}

// Função principal que executa o programa.
int main() {
    int grafo[MAX_VERTICES][MAX_VERTICES] = {{0}};
    int visitados[MAX_VERTICES];
    
    // Configuração inicial do grafo
    int numVertices = lerNumeroVertices();
    bool dirigido = lerDirecionado();
    inicializarGrafo(grafo, numVertices);
    lerArestas(grafo, numVertices, dirigido);
    
    int opcao;
    do {
        clear();
        cout << "\nMatriz de adjacencia:\n";
        mostrarGrafo(grafo, numVertices);
        
        // Exibição do menu principal
        cout << "\n===== MENU =====\n";
        cout << "1. DFS (Busca em Profundidade)\n";
        cout << "2. BFS (Busca em Largura)\n";
        cout << "3. Pesquisar vertice\n";
        cout << "4. Fecho transitivo direto de um vertice\n";
        cout << "5. Fecho transitivo inverso de um vertice\n";
        cout << "6. Verificar Conectividade\n";
        cout << "7. Modificar grafo\n";
        cout << "8. Refazer grafo do zero\n";
        cout << "9. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch(opcao) {
            case 1: { // DFS
                fill(visitados, visitados+numVertices, false);
                int v = lerVertice(numVertices, "\nDigite o vertice inicial para o DFS (1 a " + to_string(numVertices) + "): ");
                DFS(v, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 2: { // BFS
                fill(visitados, visitados+numVertices, false);
                int v = lerVertice(numVertices, "\nDigite o vertice inicial para o BFS (1 a " + to_string(numVertices) + "): ");
                BFS(v, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 3: { // PESQUISAR VÉRTICE
                int v = lerVertice(numVertices, "Digite o vertice a ser pesquisado (1 a " + to_string(numVertices) + "): ");
                cout << "O vertice " << v + 1 << " existe no grafo.\n";
                pause();
                break;
            }
            case 4: { // FECHO TRANSITIVO DIRETO
                int v = lerVertice(numVertices, "\nDigite o vertice para o FTD (1 a " + to_string(numVertices) + "): ");
                FTD_show(v, grafo, numVertices, visitados);
                pause();
                break;
            }
            case 5: { // FECHO TRANSITIVO INVERSO
                int v = lerVertice(numVertices, "\nDigite o vertice para o FTI (1 a " + to_string(numVertices) + "): ");
                FTI_show(v, grafo, numVertices, visitados);
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
                } else if(subopcao == 2) {
                    cout << "Digite as arestas a remover (u v). Digite 0 para finalizar:\n";
                    while(true) {
                        int u, v;
                        cin >> u >> v;
                        if(u == 0 || v == 0) break;
                        u--; v--; // Ajusta para índice base 0
                        if(verticeExiste(u, numVertices) && verticeExiste(v, numVertices)) {
                            grafo[u][v] = 0;
                            if(!dirigido) grafo[v][u] = 0;
                        } else {
                            cout << "Aresta invalida!\n";
                        }
                    }
                } else if(subopcao == 3) { // Adicionar vertices
                    int qtd;
                    cout << "Quantos vertices deseja adicionar? ";
                    cin >> qtd;

                    if(qtd <= 0) {
                        cout << "Quantidade invalida!\n";
                    } else if(numVertices + qtd > MAX_VERTICES) {
                        cout << "Nao e possivel adicionar. Limite de " << MAX_VERTICES << " vertices atingido.\n";
                    } else {
                        int numVerticesAnterior = numVertices;
                        numVertices += qtd;
                        // Zera as novas linhas e colunas na matriz de adjacência
                        for(int i = numVerticesAnterior; i < numVertices; i++) {
                            for(int j = 0; j < numVertices; j++) {
                                grafo[i][j] = 0;
                                grafo[j][i] = 0;
                            }
                        }
                        cout << qtd << " vertice(s) adicionado(s). O grafo agora tem " << numVertices << " vertices.\n";
                    }
                } else if(subopcao == 4) {
                    int v = lerVertice(numVertices, "Digite o vertice a remover: ");
                    // Move todas as linhas e colunas para "apagar" o vértice
                    for(int i = v; i < numVertices - 1; i++) {
                        for(int j = 0; j < numVertices; j++) {
                            grafo[i][j] = grafo[i+1][j]; // Move a linha de baixo para cima
                        }
                    }
                    for(int i = v; i < numVertices - 1; i++) {
                        for(int j = 0; j < numVertices; j++) {
                            grafo[j][i] = grafo[j][i+1]; // Move a coluna da direita para a esquerda
                        }
                    }
                    numVertices--;
                    cout << "Vertice removido. O grafo agora tem " << numVertices << " vertices.\n";
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
                pause();
        }

    } while(opcao != 9);
    
    return 0;
}
