// pert_cpm.cpp
// Compilar: g++ -std=c++17 main.cpp -o pert_cpm
// Uso: ./pert_cpm
#include <bits/stdc++.h>
using namespace std;

struct Activity {
    int id;
    int dur;
    vector<int> preds;
    vector<int> succ;
    long long ES=0, EF=0, LS=0, LF=0;
    long long slack=0;
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "PERT/CPM - entrada interativa\n";
    cout << "Informe número de atividades: ";
    int n; 
    if(!(cin>>n)) return 0;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // leitura flexível de ids; map id->index
    vector<int> ids;
    unordered_map<int,int> id2idx;
    vector<Activity> acts;
    acts.reserve(n);

    cout << "Formato por atividade: ID Duração Precedentes(comma ou vazio)\n";
    cout << "Exemplo linha para atividade sem precedentes: 1 10\n";
    cout << "Exemplo com precedentes: 5 5 2,4\n\n";

    for(int i=0;i<n;i++){
        string line;
        cout << "Atividade " << (i+1) << ": ";
        getline(cin, line);
        if(line.empty()){ i--; continue; }
        stringstream ss(line);
        int id,dur;
        ss >> id >> dur;
        string rest;
        getline(ss, rest);
        vector<int> preds;
        // trim rest
        auto trim = [&](string &s){ 
            size_t a = s.find_first_not_of(" \t");
            size_t b = s.find_last_not_of(" \t");
            if(a==string::npos) { s=""; return; }
            s = s.substr(a, b-a+1);
        };
        trim(rest);
        if(!rest.empty()){
            // can be "2,4" or "2, 4" or "2"
            string cur;
            for(char c: rest){
                if(c==',' || isspace((unsigned char)c)){
                    if(!cur.empty()){ preds.push_back(stoi(cur)); cur.clear(); }
                } else cur.push_back(c);
            }
            if(!cur.empty()) preds.push_back(stoi(cur));
        }
        Activity A;
        A.id = id; A.dur = dur; A.preds = preds;
        id2idx[id] = acts.size();
        acts.push_back(A);
        ids.push_back(id);
    }

    // build successors and adjacency
    for(auto &a: acts){
        for(int p: a.preds){
            if(id2idx.find(p)==id2idx.end()){
                cerr<<"Erro: precedência "<<p<<" não encontrada. Abortando.\n";
                return 1;
            }
            acts[id2idx[p]].succ.push_back(a.id);
        }
    }

    // build graph using indices
    int m = acts.size();
    vector<vector<int>> adj(m);
    vector<int> indeg(m,0);
    for(int i=0;i<m;i++){
        for(int s_id: acts[i].succ){
            int j = id2idx[s_id];
            adj[i].push_back(j);
            indeg[j]++;
        }
    }

    // topo sort Kahn
    queue<int> q;
    for(int i=0;i<m;i++) if(indeg[i]==0) q.push(i);
    vector<int> topo;
    while(!q.empty()){
        int u=q.front(); q.pop();
        topo.push_back(u);
        for(int v: adj[u]){
            indeg[v]--;
            if(indeg[v]==0) q.push(v);
        }
    }
    if((int)topo.size()!=m){
        cerr<<"Grafo tem ciclo. Não é DAG. Abortando.\n";
        return 1;
    }

    // forward pass: ES, EF
    for(int idx: topo){
        long long es = 0;
        for(int pid : acts[idx].preds){
            int pidx = id2idx[pid];
            es = max(es, acts[pidx].EF);
        }
        acts[idx].ES = es;
        acts[idx].EF = es + acts[idx].dur;
    }

    // project duration = max EF
    long long project_duration = 0;
    for(auto &a: acts) project_duration = max(project_duration, a.EF);

    // backward pass: LF, LS
    // initialize LF of sinks = project_duration
    for(int i=0;i<m;i++){
        if(adj[i].empty()) { acts[i].LF = project_duration; acts[i].LS = acts[i].LF - acts[i].dur; }
    }
    // process reverse topo
    for(int t = (int)topo.size()-1; t>=0; --t){
        int u = topo[t];
        if(adj[u].empty()) {
    // já definido para os nós finais
    continue;
}
long long lf = LLONG_MAX;
for(int v: adj[u]) {
    lf = min(lf, acts[v].LS);
}
acts[u].LF = lf;
acts[u].LS = acts[u].LF - acts[u].dur;


        if(!adj[u].empty()){
            long long lf = LLONG_MAX;
            for(int v: adj[u]){
                lf = min(lf, acts[v].LS);
            }
            acts[u].LF = lf;
            acts[u].LS = acts[u].LF - acts[u].dur;
        } else {
            // already set for sinks
        }
    }

    // slack
    for(auto &a: acts){
        a.slack = a.LS - a.ES;
    }

    // identify critical activities (slack==0)
    vector<int> critical_ids;
    for(auto &a: acts) if(a.slack==0) critical_ids.push_back(a.id);

    // build one critical path by walking from a start critical node following critical successors
    vector<int> critical_path;
    // find start nodes with slack 0 and indegree 0
    vector<int> start_candidates;
    for(int i=0;i<m;i++){
        if(acts[i].slack==0 && acts[i].preds.empty()) start_candidates.push_back(i);
    }
    if(start_candidates.empty()){
        // fallback: any critical with minimal ES
        long long minES = LLONG_MAX; int pick=-1;
        for(int i=0;i<m;i++) if(acts[i].slack==0 && acts[i].ES < minES){ minES=acts[i].ES; pick=i;}
        if(pick!=-1) start_candidates.push_back(pick);
    }

    function<bool(int)> build_path = [&](int idx)->bool {
        critical_path.push_back(acts[idx].id);
        // if sink and critical -> stop
        if(adj[idx].empty()) return true;
        for(int v: adj[idx]){
            if(acts[v].slack==0 && acts[idx].EF == acts[v].ES){
                if(build_path(v)) return true;
            }
        }
        // dead end: remove and backtrack
        critical_path.pop_back();
        return false;
    };

    bool found=false;
    for(int s: start_candidates){
        if(build_path(s)){ found=true; break; }
    }
    if(!found) critical_path.clear(); // no chain built

    // Output results table
    cout << "\nResultado (ES EF LS LF Slack)\n";
    cout << "ID\tDur\tES\tEF\tLS\tLF\tSlack\n";
    // sort by id for display
    vector<int> order(m);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a,int b){ return acts[a].id < acts[b].id; });
    for(int idx: order){
        auto &a = acts[idx];
        cout << a.id << '\t' << a.dur << '\t' << a.ES << '\t' << a.EF << '\t' << a.LS << '\t' << a.LF << '\t' << a.slack << '\n';
    }
    cout << "\nDuração do projeto: " << project_duration << '\n';
    cout << "Atividades críticas (slack==0): ";
    for(size_t i=0;i<critical_ids.size();++i){
        if(i) cout<<",";
        cout<<critical_ids[i];
    }
    cout << '\n';
    if(!critical_path.empty()){
        cout << "Um caminho crítico (sequência): ";
        for(size_t i=0;i<critical_path.size();++i){
            if(i) cout<<" -> ";
            cout<<critical_path[i];
        }
        cout << '\n';
    }

    // gerar grafo.dot
    string fname = "grafo.dot";
    ofstream f(fname);
    f << "digraph PERT {\n";
    f << "  rankdir=LR;\n";
    f << "  node [shape=record];\n";
    for(auto &a: acts){
        string color = (a.slack==0) ? "red" : "black";
        f << "  \""<<a.id<<"\" [color="<<color<<", fontcolor="<<color<<", label=\"{"
          << "At "<<a.id<<" | Dur:"<<a.dur<<" | ES:"<<a.ES<<", EF:"<<a.EF<<" | LS:"<<a.LS<<", LF:"<<a.LF<<" | Slack:"<<a.slack
          << "}\"];\n";
    }
    for(int i=0;i<m;i++){
        for(int v: adj[i]){
            f << "  \""<<acts[i].id<<"\" -> \""<<acts[v].id<<"\";\n";
        }
    }
    f << "}\n";
    f.close();
    cout << "Arquivo gerado: " << fname << " (use Graphviz para renderizar .png se desejar)\n";
    cout << "Exemplo: dot -Tpng " << fname << " -o grafo.png\n";
    return 0;
}
