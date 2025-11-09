// Variável global para armazenar a instância da rede
let network = null;

// Adiciona nova linha de atividade
function addActivity(id = '', dur = '', deps = '') {
    const tbody = document.getElementById('table-body');
    const row = tbody.insertRow();
    
    // Injeção de HTML para a nova linha
    row.innerHTML = `
        <td><input type="text" value="${id}" placeholder="1" /></td>
        <td><input type="text" value="${dur}" placeholder="10" /></td>
        <td><input type="text" value="${deps}" placeholder="0 ou 1,2" /></td>
        <td><button class="btn-delete" onclick="removeActivity(this)">×</button></td>
    `;
}

// Remove linha de atividade
function removeActivity(btn) {
    btn.closest('tr').remove();
}

// Carrega dados iniciais (Exemplo Planta Industrial)
function loadInitialData() {
    const activities = [
        ['1', '2', '0'],
        ['2', '4', '1'],
        ['3', '10', '2'],
        ['4', '6', '3'],
        ['5', '4', '3'],
        ['6', '5', '5'],
        ['7', '7', '4'],
        ['8', '9', '5,7'],
        ['9', '7', '3'],
        ['10', '8', '6,9'],
        ['11', '4', '10'],
        ['12', '5', '10'],
        ['13', '2', '8'],
        ['14', '6', '11,12']
    ];
    
    activities.forEach(([id, dur, deps]) => addActivity(id, dur, deps));
}

// Processa os dados do backend
async function process() {
    try {
        const rows = document.getElementById('table-body').rows;
        let input = '';
        
        for (let i = 0; i < rows.length; i++) {
            const cells = rows[i].cells;
            const id = cells[0].querySelector('input').value.trim();
            const dur = cells[1].querySelector('input').value.trim();
            let deps = cells[2].querySelector('input').value.trim();
            
            if (!id || !dur) continue;
            if (deps === '') deps = '0'; // Garante "0" se estiver vazio
            
            input += `${id} ${dur} ${deps}\n`;
        }
        
        const response = await fetch('/processar', {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: input
        });
        
        if (!response.ok) throw new Error('Falha ao processar');
        
        const data = await response.json();
        
        renderGraph(data);
        renderStats(data);
        
        document.getElementById('error').innerHTML = '';
        
    } catch (error) {
        document.getElementById('error').innerHTML = 
            `<div class="error">${error.message}</div>`;
    }
}

// Renderiza o gráfico
function renderGraph(data) {
    if (network !== null) {
        network.destroy();
        network = null;
    }

    const activities = data.atividades;
    const nodes = [];
    const edges = [];
    
    const predecessors = new Set();
    activities.forEach(a => a.precedentes.forEach(p => predecessors.add(p)));

    // Nó de Início
    nodes.push({
        id: 'start',
        label: 'INÍCIO',
        color: {
            background: '#ffffff',
            border: '#ff477e'
        },
        font: { 
            color: '#ff477e',
            size: 13,
            bold: true
        },
        shape: 'box',
        margin: 10,
        borderWidth: 2
    });

    // Nó de Fim
    nodes.push({
        id: 'end',
        label: 'FIM',
        color: {
            background: '#ffffff',
            border: '#ff477e'
        },
        font: { 
            color: '#ff477e',
            size: 13,
            bold: true
        },
        shape: 'box',
        margin: 10,
        borderWidth: 2
    });

    // Nós de Atividades
    activities.forEach(a => {
        // --- TRADUÇÃO DAS ETIQUETAS ---
        const label = 
            `${a.id}\n` +
            `Duração: ${a.duracao}\n` +
            `${a.inicio_cedo} | ${a.fim_cedo}\n` +
            `${a.inicio_tarde} | ${a.fim_tarde}\n` +
            `Folga: ${a.folga_total}`;

        nodes.push({
            id: a.id,
            label: label,
            color: {
                background: a.critica ? '#ff477e' : '#ff8f8f', 
                border: a.critica ? '#c93756' : '#fda4af'
            },
            font: { 
                color: '#ffffff',
                size: 12,
                bold: true
            },
            shape: 'box',
            margin: 10,
            borderWidth: 2
        });

        // Conecta ao Início
        if (a.precedentes.length === 0) {
            edges.push({
                from: 'start',
                to: a.id,
                arrows: 'to',
                color: '#fda4af', 
                width: 1.5
            });
        }

        // Conecta ao Fim
        if (!predecessors.has(a.id)) {
            edges.push({
                from: a.id,
                to: 'end',
                arrows: 'to',
                color: a.critica ? '#ff477e' : '#fda4af', 
                width: a.critica ? 2.5 : 1.5
            });
        }

        // Conecta atividades
        a.precedentes.forEach(pid => {
            const pred = activities.find(x => x.id === pid);
            if (!pred) return;
            
            const critical = a.critica && pred.critica;
            
            edges.push({
                from: pid,
                to: a.id,
                arrows: 'to',
                color: critical ? '#ff477e' : '#fda4af',
                width: critical ? 2.5 : 1.5
            });
        });
    });

    const container = document.getElementById('graph');
    network = new vis.Network(container, { nodes, edges }, {
        layout: {
            hierarchical: {
                direction: 'LR',
                sortMethod: 'directed',
                nodeSpacing: 180,
                levelSeparation: 200
            }
        },
        physics: false,
        interaction: {
            hover: true,
            zoomView: true
        }
    });
}

// Renderiza as estatísticas
function renderStats(data) {
    const critical = data.atividades.filter(a => a.critica);
    
    document.getElementById('stats').innerHTML = `
        <div class="stats">
            <div class="stat-card">
                <div class="stat-value">${data.tempo_total}</div>
                <div class="stat-label">Duração do Projeto</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">${critical.length}</div>
                <div class="stat-label">Atividades Críticas</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">${data.atividades.length}</div>
                <div class="stat-label">Total de Atividades</div>
            </div>
        </div>
    `;
    
    // === CORREÇÃO 2: Garante que o caminho crítico seja ordenado ===
    // Ordena pelo Início Cedo (ES) para que o caminho venha na ordem correta
    critical.sort((a, b) => a.inicio_cedo - b.inicio_cedo);
    const path = critical.map(a => a.id).join(' → ');
    
    document.getElementById('critical-path').innerHTML = `
        <div class="critical-path">Caminho Crítico: ${path}</div>
    `;
}

// Inicializa
window.onload = function() {
    loadInitialData();
    process();
};