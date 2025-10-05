/*
cd Atividade3
g++ main.cpp -o grafos
python server.py
*/

#include "pilha.hpp"
#include "fila.hpp"
#include <limits>
#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
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

// ========= FUNÇÃO DE COLORAÇÃO =========
void colorirGrafo(int G[][MAX_VERTICES], int numVertices, int cores[]) {
    fill(cores, cores + numVertices, 0); // 0 = não colorido
    int numCromatico = 0;
    int ordemColoracao[MAX_VERTICES]; // Array para guardar a ordem de coloração.

    if (numVertices > 0) {
        int verticesColoridos = 0;
        
        // Calcula os graus de todos os vértices UMA VEZ e guarda para usar depois.
        int graus[MAX_VERTICES];
        for (int i = 0; i < numVertices; i++) {
            graus[i] = 0;
            for (int j = 0; j < numVertices; j++) {
                if (G[i][j] == 1) graus[i]++;
                if (G[j][i] == 1) graus[i]++;
            }
        }
        
        // Encontra o vértice de maior grau e o colore com a primeira cor.
        int maxGrau = -1;
        int verticeInicial = -1;
        for (int i = 0; i < numVertices; i++) {
            if (graus[i] > maxGrau) {
                maxGrau = graus[i];
                verticeInicial = i;
            }
        }
        
        // Colore o vértice inicial escolhido com a primeira cor (cor 1).
        cores[verticeInicial] = 1;
        ordemColoracao[verticesColoridos] = verticeInicial; // Guarda o primeiro vértice na ordem.
        verticesColoridos++;

        // Laço principal: continua até que todos os vértices tenham sido coloridos.
        while (verticesColoridos < numVertices) {
            // Encontra o vértice não colorido com o maior 'grau de saturação'
            // (vértice adjacente ao maior número de cores diferentes).
            int proximoVertice = -1;
            int maxSaturacao = -1;

            for (int v = 0; v < numVertices; v++) {
                if (cores[v] == 0) { // Considera apenas vértices ainda não coloridos.
                    bool coresVizinhas[MAX_VERTICES + 1] = {false};
                    int saturacaoAtual = 0;
                    for (int u = 0; u < numVertices; u++) {
                        if ((G[v][u] == 1 || G[u][v] == 1) && cores[u] != 0) {
                            if (!coresVizinhas[cores[u]]) {//Só atualiza se for uma cor nova 
                                coresVizinhas[cores[u]] = true;
                                saturacaoAtual++;
                            }
                        }
                    }
                    // Compara a saturação do vértice atual com a maior já encontrada.
                    if (saturacaoAtual > maxSaturacao) {
                        maxSaturacao = saturacaoAtual;
                        proximoVertice = v; // Se for maior, este vértice é o novo candidato.
                    } else if (saturacaoAtual == maxSaturacao) {
                        // Se a saturação é a mesma, desempata pelo maior grau original.
                        if (proximoVertice == -1 || graus[v] > graus[proximoVertice]) {
                            proximoVertice = v;
                        }
                    }
                }
            }
            
            if (proximoVertice == -1) {
                for(int i = 0; i < numVertices; i++){
                    if(cores[i] == 0) { proximoVertice = i; break; }
                }
            }
            
            // Atribui a menor cor possível que não esteja em uso pelos vizinhos.
            bool coresAdmissiveis[MAX_VERTICES + 1];
            fill(coresAdmissiveis, coresAdmissiveis + MAX_VERTICES + 1, true);

            for (int u = 0; u < numVertices; u++) {
                if ((G[proximoVertice][u] == 1 || G[u][proximoVertice] == 1) && cores[u] != 0) {
                    // Marca a cor de um vizinho como inválida para o vértice atual.
                    coresAdmissiveis[cores[u]] = false;
                }
            }
            
            int novaCor = 1;
            // Procura o primeiro número de cor que ainda está marcado como 'true'.
            while (!coresAdmissiveis[novaCor]) {
                novaCor++;
            }
            // Aplica a menor cor encontrada ao vértice escolhido.
            cores[proximoVertice] = novaCor;
            ordemColoracao[verticesColoridos] = proximoVertice; // Guarda o vértice na ordem.
            verticesColoridos++;
        }
    }

    // --- SAÍDA PARA O USUÁRIO ---
    cout << "Resultado da Coloracao (Heuristica):\n";
    if (numVertices > 0) {
        
        // Este laço percorre as cores finais para encontrar a maior (o número cromático)
        // e também para exibir o resultado de cada vértice.
        cout << "\nCores Finais:\n";
        for(int i = 0; i < numVertices; i++) {
            cout << "Vertice " << i + 1 << ": Cor " << cores[i] << endl;
            if(cores[i] > numCromatico) {
                numCromatico = cores[i];
            }
        }

        cout << "\nNumero Cromático (estimado): " << numCromatico << "\n";
        
        // Mostra a ordem de coloração.
        cout << "\nOrdem de coloracao dos vertices:\n";
        for (int i = 0; i < numVertices; i++) {
            int verticeDaVez = ordemColoracao[i];
            cout << i + 1 << "º -> Vertice " << verticeDaVez + 1 << " (Cor " << cores[verticeDaVez] << ")" << endl;
        }

    } else {
        cout << "O grafo esta vazio.\n";
    }
    
    // --- SAÍDA PARA PARSING ---
    cout << "\n---PARSED_OUTPUT---\n";
    cout << "Cores:";
    for(int i = 0; i < numVertices; i++) {
        cout << " " << cores[i];
    }
    cout << "\n";
    cout << "Ordem:";
    for(int i = 0; i < numVertices; i++) {
        cout << " " << ordemColoracao[i] + 1;
    }
    cout << "\n";
    cout << "NumeroCromatico: " << numCromatico << "\n";
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

// Função principal modificada para ser controlada por um script
int main() {
    string command;
    cin >> command; // Lê o comando: "dfs", "bfs", "conectividade", etc.

    int numVertices;
    cin >> numVertices;

    bool dirigido;
    cin >> dirigido;

    int grafo[MAX_VERTICES][MAX_VERTICES];
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            cin >> grafo[i][j];
        }
    }

    int visitados[MAX_VERTICES];

    if (command == "dfs") {
        int v;
        cin >> v; // Lê o vértice inicial
        fill(visitados, visitados + numVertices, false);
        DFS(v, grafo, numVertices, visitados);
    } else if (command == "bfs") {
        int v;
        cin >> v;
        fill(visitados, visitados + numVertices, false);
        BFS(v, grafo, numVertices, visitados);
    } else if (command == "ftd") {
        int v;
        cin >> v;
        FTD_show(v, grafo, numVertices, visitados);
    } else if (command == "fti") {
        int v;
        cin >> v;
        FTI_show(v, grafo, numVertices, visitados);
    } else if (command == "conectividade") {
        conectividade(grafo, numVertices, visitados);
    }else if (command == "colorir") {
        int cores[MAX_VERTICES];
        colorirGrafo(grafo, numVertices, cores);
    }
    else if (command == "show") {
        mostrarGrafo(grafo, numVertices);
    } else {
        cout << "Comando desconhecido: " << command << endl;
    }

    return 0;
}