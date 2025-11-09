/*
SERVIDOR PERT/CPM 
Compilar: g++ -o pert_cpm main.cpp -std=c++11
Executar: ./pert_cpm
Acessar:  http://localhost:8080

Certifique-se que index.html, style.css e app.js estejam na 
mesma pasta do executável.
*/

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <vector>

// Compatibilidade multiplataforma para sockets
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define closesocket close
#endif

using namespace std;

const int MAX_ATIVIDADES = 100; // Número máximo de atividades
const int PORT = 8080;

// Estrutura de atividade
struct Atividade {
    int id;                          // Id da atividade
    int duracao;                     // Duração da atividade
    int precedentes[MAX_ATIVIDADES]; // Índices (no vetor atividades[]) das atividades predecessoras
    int numPrecedentes;              // Quantidade de predecessores
    
    // Tempos calculados pelo algoritmo
    int inicio_cedo;                 // Earliest Start (ES)
    int fim_cedo;                    // Earliest Finish (EF)
    int inicio_tarde;                // Latest Start (LS)
    int fim_tarde;                   // Latest Finish (LF)
    int folga_total;                 // Total Slack (LS - ES)
    
    bool critica;                    // True se folga = 0
};

// Iniciliaza a atividade com valores padrão
void inicializarAtividade(Atividade& ativ) {
    ativ.numPrecedentes = 0;
    ativ.inicio_cedo    = 0;
    ativ.fim_cedo       = 0;
    ativ.inicio_tarde   = 0;
    ativ.fim_tarde      = 0;
    ativ.folga_total    = 0;
    ativ.critica        = false;
    
    // Inicializa vetor de precedentes com -1 (vazio)
    for (int i = 0; i < MAX_ATIVIDADES; i++) {
        ativ.precedentes[i] = -1;
    }
}

// Constrói o grafo de sucessores a partir dos precedentes
void construirGrafoSucessores(Atividade atividades[], int numAtividades, int sucessores[][MAX_ATIVIDADES], int numSucessores[]) {
    // Inicializa os vetores
    for (int i = 0; i < numAtividades; i++) {
        numSucessores[i] = 0;
        for (int j = 0; j < MAX_ATIVIDADES; j++) {
            sucessores[i][j] = -1;
        }
    }
    
    // Para cada atividade, adiciona ela como sucessora de seus precedentes
    for (int i = 0; i < numAtividades; i++) {
        for (int j = 0; j < atividades[i].numPrecedentes; j++) {
            int precIndex = atividades[i].precedentes[j]; // já é índice no array
            if (precIndex >= 0 && precIndex < numAtividades) {
                sucessores[precIndex][numSucessores[precIndex]] = i;
                numSucessores[precIndex]++;
            }
        }
    }
}

// Calcula Início Cedo e Fim Cedo
// IC = max(FC dos predecessores), FC = IC + Duração
void calcularIniciosCedos(Atividade atividades[], int numAtividades) {
    bool mudou = true;
    
    // Fica no loop até não haver mais mudanças
    while (mudou) {
        mudou = false;
        
        for (int i = 0; i < numAtividades; i++) {
            int max_fim_prec = 0;
            
            // Encontra o maior Fim Cedo entre os predecessores
            for (int j = 0; j < atividades[i].numPrecedentes; j++) {
                int prec = atividades[i].precedentes[j];
                if (prec >= 0 && prec < numAtividades) {
                    max_fim_prec = max(max_fim_prec, atividades[prec].fim_cedo);
                }
            }
            
            // Calcula novos valores
            int novo_inicio = max_fim_prec;
            int novo_fim = novo_inicio + atividades[i].duracao;
            
            // Se mudou, marca para outra iteração
            if (novo_inicio != atividades[i].inicio_cedo || novo_fim != atividades[i].fim_cedo) {
                atividades[i].inicio_cedo = novo_inicio;
                atividades[i].fim_cedo = novo_fim;
                mudou = true;
            }
        }
    }
}

// Encontra o tempo de duração total (é o maior Fim Cedo)
int tempoTotal(Atividade atividades[], int numAtividades) {
    int tempo_total = 0;
    for (int i = 0; i < numAtividades; i++) {
        tempo_total = max(tempo_total, atividades[i].fim_cedo);
    }
    return tempo_total;
}

// Calcula Início Tarde e Fim Tarde
// FT = min(IT dos sucessores), IT = FT - Duração
void temposTardios(Atividade atividades[], int numAtividades, int sucessores[][MAX_ATIVIDADES], int numSucessores[], int tempo_total) {
    
    // Inicializa todos com o tempo total
    for (int i = 0; i < numAtividades; i++) {
        atividades[i].fim_tarde = tempo_total;
        atividades[i].inicio_tarde = tempo_total;
    }
    
    // Atividades finais (sem sucessores) têm FT = tempo total
    for (int i = 0; i < numAtividades; i++) {
        if (numSucessores[i] == 0) {
            atividades[i].fim_tarde = tempo_total;
            atividades[i].inicio_tarde = tempo_total - atividades[i].duracao;
        }
    }
    
    bool mudou = true;
    
    // Fica no loop até mudou ser false
    while (mudou) {
        mudou = false;
        
        for (int i = 0; i < numAtividades; i++) {
            int min_inicio_suc = tempo_total;
            
            // Encontra o menor Início Tarde entre os sucessores
            for (int j = 0; j < numSucessores[i]; j++) {
                int suc = sucessores[i][j];
                if (suc >= 0 && suc < numAtividades) {
                    min_inicio_suc = min(min_inicio_suc, atividades[suc].inicio_tarde);
                }
            }
            
            // Calcula novos valores
            int novo_fim = min_inicio_suc;
            int novo_inicio = novo_fim - atividades[i].duracao;
            
            // Se mudou, marca para outra iteração
            if (novo_fim != atividades[i].fim_tarde || novo_inicio != atividades[i].inicio_tarde) {
                atividades[i].fim_tarde = novo_fim;
                atividades[i].inicio_tarde = novo_inicio;
                mudou = true;
            }
        }
    }
}

// Calcula folgas e identifica atividades críticas
// Folga Total = IT - IC, se folga=0 -> Crítica=True
void calcularFolgas(Atividade atividades[], int numAtividades) {
    for (int i = 0; i < numAtividades; i++) {
        atividades[i].folga_total = atividades[i].inicio_tarde - atividades[i].inicio_cedo;
        atividades[i].critica = (atividades[i].folga_total == 0);
    }
}

// PERT/CPM
void processar(Atividade atividades[], int numAtividades) {
    int sucessores[MAX_ATIVIDADES][MAX_ATIVIDADES];
    int numSucessores[MAX_ATIVIDADES];
    
    construirGrafoSucessores(atividades, numAtividades, sucessores, numSucessores);
    calcularIniciosCedos(atividades, numAtividades);
    int tempo_total = tempoTotal(atividades, numAtividades);
    temposTardios(atividades, numAtividades, sucessores, numSucessores, tempo_total);
    calcularFolgas(atividades, numAtividades);
}


// Converte os resultados para formato JSON - para envio ao frontend
string gerarJSON(Atividade atividades[], int numAtividades) {
    stringstream ss;
    ss << "{\"atividades\":[";
    
    for (int i = 0; i < numAtividades; i++) {
        if (i > 0) ss << ",";
        
        ss << "{"
           << "\"id\":" << atividades[i].id << ","
           << "\"duracao\":" << atividades[i].duracao << ","
           << "\"precedentes\":[";
        
        // Converte índices para IDs dos precedentes
        bool primeiro = true;
        for (int j = 0; j < atividades[i].numPrecedentes; j++) {
            int precIndex = atividades[i].precedentes[j];
            if (precIndex >= 0 && precIndex < numAtividades) {
                if (!primeiro) ss << ",";
                ss << atividades[precIndex].id;
                primeiro = false;
            }
        }
        
        ss << "],"
           << "\"inicio_cedo\":" << atividades[i].inicio_cedo << ","
           << "\"fim_cedo\":" << atividades[i].fim_cedo << ","
           << "\"inicio_tarde\":" << atividades[i].inicio_tarde << ","
           << "\"fim_tarde\":" << atividades[i].fim_tarde << ","
           << "\"folga_total\":" << atividades[i].folga_total << ","
           << "\"critica\":" << (atividades[i].critica ? "true" : "false")
           << "}";
    }
    
    ss << "],\"tempo_total\":" << tempoTotal(atividades, numAtividades) << "}";
    return ss.str();
}

// PARSING DE ENTRADA

/**
 * Converte texto do formato "ID DURAÇÃO PRECEDENTES" em estruturas
 * Exemplo: "1 10 0" ou "5 5 2,4"
 *
 * Implementação em duas fases:
 *  - primeira fase: le as atividades (id, dur) e armazena predecessores como IDs temporários
 *  - segunda fase: converte esses predecessor-IDs para índices no array atividades[]
 */
void parseAtividades(const string& texto, Atividade atividades[], int& numAtividades) {
    numAtividades = 0;
    stringstream ss(texto);
    string linha;

    // temporários para guardar predecessores como IDs até sabermos todos os IDs lidos
    int tempPreIDs[MAX_ATIVIDADES][MAX_ATIVIDADES];
    int numTempPreIDs[MAX_ATIVIDADES];

    for (int i = 0; i < MAX_ATIVIDADES; i++) {
        numTempPreIDs[i] = 0;
        for (int j = 0; j < MAX_ATIVIDADES; j++) {
            tempPreIDs[i][j] = -1;
        }
    }
    
    // fase 1: leitura e armazenamento (predecessores como IDs)
    while (getline(ss, linha) && numAtividades < MAX_ATIVIDADES) {
        // Remove \r e espaços desnecessários
        linha.erase(remove(linha.begin(), linha.end(), '\r'), linha.end());
        if (linha.empty()) continue;

        inicializarAtividade(atividades[numAtividades]);
        
        stringstream linhaSS(linha);
        int id = 0, duracao = 0;
        string precedentes_str;
        
        // tenta ler id, duracao e a string de precedentes (se houver)
        linhaSS >> id >> duracao >> precedentes_str;
        
        atividades[numAtividades].id = id;
        atividades[numAtividades].duracao = duracao;
        atividades[numAtividades].numPrecedentes = 0; // por enquanto
        
        // Guarda os predecessores como IDs temporários
        if (!precedentes_str.empty() && precedentes_str != "0") {
            stringstream precSS(precedentes_str);
            string prec;
            int idx = 0;
            
            while (getline(precSS, prec, ',') && idx < MAX_ATIVIDADES) {
                // remova espaços
                prec.erase(remove(prec.begin(), prec.end(), ' '), prec.end());
                if (prec.empty()) continue;
                try {
                    int prec_id = stoi(prec);
                    tempPreIDs[numAtividades][idx++] = prec_id;
                } catch (...) {
                    // se stoi falhar, ignora esse predecessor
                }
            }
            numTempPreIDs[numAtividades] = idx;
        } else {
            numTempPreIDs[numAtividades] = 0;
        }

        numAtividades++;
    }

    // fase 2: converter predecessor IDs para índices reais no vetor atividades[] 
    for (int i = 0; i < numAtividades; i++) {
        atividades[i].numPrecedentes = 0; // reset
        for (int j = 0; j < numTempPreIDs[i]; j++) {
            int prec_id = tempPreIDs[i][j];
            // encontra o índice k tal que atividades[k].id == prec_id
            int foundIndex = -1;
            for (int k = 0; k < numAtividades; k++) {
                if (atividades[k].id == prec_id) {
                    foundIndex = k;
                    break;
                }
            }
            if (foundIndex != -1) {
                atividades[i].precedentes[atividades[i].numPrecedentes++] = foundIndex;
            } else {
                // predecessor não encontrado — ignorar (poderia logar/retornar erro)
            }
        }
    }
}

// SERVIDOR HTTP
//Lê conteúdo de arquivo do disco
string readFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Erro: arquivo nao encontrado - " << filePath << endl;
        return "";
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

 // Processa requisições HTTP e retorna respostas apropriadas
string handleRequest(const string& request) {
    // Helper para criar respostas HTTP
    auto criarResposta = [](const string& body, const string& contentType) {
        stringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << contentType << "; charset=utf-8\r\n"
                 << "Content-Length: " << body.length() << "\r\n"
                 << "Access-Control-Allow-Origin: *\r\n"
                 << "\r\n"
                 << body;
        return response.str();
    };

    // Roteamento de arquivos estáticos
    if (request.find("GET / ") == 0) {
        string html = readFile("index.html");
        return html.empty() ? "HTTP/1.1 404 Not Found\r\n\r\n" 
                            : criarResposta(html, "text/html");
    }
    else if (request.find("GET /style.css") == 0) {
        string css = readFile("style.css");
        return css.empty() ? "HTTP/1.1 404 Not Found\r\n\r\n" 
                           : criarResposta(css, "text/css");
    }
    else if (request.find("GET /app.js") == 0) {
        string js = readFile("app.js");
        return js.empty() ? "HTTP/1.1 404 Not Found\r\n\r\n" 
                          : criarResposta(js, "application/javascript");
    }
    // API de processamento
    else if (request.find("POST /processar") == 0) {
        // Extrai corpo da requisição
        size_t bodyStart = request.find("\r\n\r\n");
        if (bodyStart == string::npos) {
            return "HTTP/1.1 400 Bad Request\r\n\r\n";
        }
        
        string body = request.substr(bodyStart + 4);
        
        try {
            Atividade atividades[MAX_ATIVIDADES];
            int numAtividades;
            
            // Processa dados
            parseAtividades(body, atividades, numAtividades);
            processar(atividades, numAtividades);
            string json = gerarJSON(atividades, numAtividades);
            
            return criarResposta(json, "application/json");
            
        } catch (const exception& e) {
            cerr << "Erro no processamento: " << e.what() << "\n";
            return "HTTP/1.1 500 Internal Server Error\r\n\r\n{\"error\":\"Erro interno\"}";
        }
    }
    
    return "HTTP/1.1 404 Not Found\r\n\r\n";
}


int main() {
    cout << "\n";
    cout << "    SERVIDOR PERT/CPM       \n";
    
    // Inicialização de sockets (Windows)
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif

    // Cria socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "ERRO: Nao foi possivel criar socket\n";
        return 1;
    }

    // Permite reutilização de porta
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Configura endereço
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind na porta
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "ERRO: Nao foi possivel fazer bind na porta " << PORT << "\n";
        closesocket(serverSocket);
        return 1;
    }

    // Escuta conexões
    listen(serverSocket, 5);

    cout << "Servidor iniciado com sucesso!\n";
    cout << "Porta: " << PORT << "\n";
    cout << "Acesse: http://localhost:" << PORT << "\n\n";
    cout << "Aguardando requisicoes...\n";

    // Loop principal do servidor
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            cerr << "Erro ao aceitar conexao\n";
            continue;
        }

        // Recebe requisição
        char buffer[16384] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            string request(buffer, bytesRead);
            string response = handleRequest(request);
            send(clientSocket, response.c_str(), response.length(), 0);
        }

        closesocket(clientSocket);
    }

    // Cleanup
    closesocket(serverSocket);
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    return 0;
}