from flask import Flask, request, jsonify, send_file
import subprocess
import os

app = Flask(__name__)

# Rota para servir a página principal (index.html)
@app.route('/')
def index():
    return send_file('index.html')

# Rota para executar os comandos do grafo
@app.route('/run', methods=['POST'])
def run_command():
    data = request.get_json()

    command = data.get('command')
    num_vertices = data.get('numVertices')
    is_directed = data.get('isDirected')
    matrix = data.get('matrix')
    start_vertex = data.get('startVertex', 0) # Vértice inicial (padrão 0)

    # Prepara a string de entrada para o programa C++
    # Formato: comando numVertices dirigido matriz... vertice_inicial
    input_str = f"{command}\n"
    input_str += f"{num_vertices}\n"
    input_str += f"{1 if is_directed else 0}\n"
    
    # Adiciona a matriz à string de entrada
    for row in matrix:
        input_str += " ".join(map(str, row)) + "\n"

    # Adiciona o vértice inicial se o comando precisar de um
    if command in ['dfs', 'bfs', 'ftd', 'fti']:
        input_str += f"{start_vertex}\n"

    # Define o caminho para o executável
    executable_path = "./grafos"
    if not os.path.exists(executable_path):
         return jsonify({"error": "Executável 'grafos' não encontrado. Compile o código C++ primeiro com 'g++ main.cpp -o grafos'"}), 500

    # Executa o processo C++
    try:
        process = subprocess.run(
            [executable_path],
            input=input_str,
            capture_output=True,
            text=True,
            check=True,
            timeout=10 # Adiciona um timeout de 10 segundos
        )
        
        output_text = process.stdout
        
        # Se o comando foi 'colorir', tenta extrair os dados estruturados
        if command == 'colorir':
            try:
                # Isola a parte da saída que é para parsing
                parsed_section = output_text.split("---PARSED_OUTPUT---")[1]
                
                colors_list = []
                chromatic_num = 0
                
                for line in parsed_section.strip().split('\n'):
                    if line.startswith("Cores:"):
                        colors_list = [int(c) for c in line.split(":")[1].strip().split()]
                    elif line.startswith("NumeroCromatico:"):
                        chromatic_num = int(line.split(":")[1].strip())
                
                # Retorna um JSON estruturado com os dados da coloração
                return jsonify({
                    "output": output_text.split("---PARSED_OUTPUT---")[0], # Retorna a parte legível
                    "colors": colors_list,
                    "chromatic_number": chromatic_num
                })

            except (IndexError, ValueError) as e:
                # Se o parsing falhar, retorna o texto bruto com um aviso
                return jsonify({
                    "output": output_text,
                    "error": f"Falha ao parsear a saida do C++: {e}"
                })

        # Para outros comandos, retorna a saída padrão como antes
        return jsonify({"output": output_text})

    except subprocess.CalledProcessError as e:
        # Se o programa C++ retornar um erro
        error_message = f"Erro ao executar o programa C++:\n{e.stderr}"
        return jsonify({"error": error_message}), 500
    except subprocess.TimeoutExpired:
        return jsonify({"error": "A execução do programa C++ demorou muito (timeout)."}), 500


if __name__ == '__main__':
    # Garante que o executável existe antes de iniciar o servidor
    if not os.path.exists("./grafos"):
        print("AVISO: O executável 'grafos' não foi encontrado.")
        print("Por favor, compile o código C++ com o comando: g++ main.cpp -o grafos")
    
    print("\nServidor rodando em http://127.0.0.1:5000")
    print("Abra este endereço no seu navegador.")
    app.run(debug=True)