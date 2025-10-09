import subprocess
import time
import random
import matplotlib.pyplot as plt
import numpy as np

# --- CONFIGURAÇÕES ---
EXECUTAVEL_C = './implementacoes' # Nome do seu programa C compilado

# --- FUNÇÕES AUXILIARES ---

def gerar_itens(n):
    """Gera uma lista de n itens com pesos e valores aleatórios."""
    return [(random.randint(1, 100), random.randint(1, 100)) for _ in range(n)]

def rodar_teste(n, peso_max, itens):
    """
    Executa o programa C com os dados fornecidos e retorna o tempo de execução.
    """
    # Formata a entrada para o programa C (n, peso_max, seguido dos itens)
    input_data = f"{n}\n{peso_max}\n"
    for valor, peso in itens:
        input_data += f"{valor} {peso}\n"

    start_time = time.time()
    
    # Roda o processo C, passando os dados via stdin
    # Nota: Medimos o tempo total de execução do programa C. Para entradas maiores,
    # o tempo será dominado pelo algoritmo mais lento (Força Bruta > DP > Guloso).
    subprocess.run(
        [EXECUTAVEL_C],
        input=input_data,
        text=True,
        capture_output=True # Usamos capture_output para esconder a saída do C no terminal
    )
    
    end_time = time.time()
    
    return end_time - start_time

def calcular_constante(x, y, complexidade):
    """Estima a constante de escala 'c' para a curva teórica."""
    if not y or not x:
        return 0.00001
    
    # Usa o último ponto para alinhar a curva teórica com a empírica
    ultimo_x = x[-1]
    ultimo_y = y[-1]
    
    if complexidade == '2^n':
        return ultimo_y / (2**ultimo_x)
    elif complexidade == 'nlogn':
        return ultimo_y / (ultimo_x * np.log2(ultimo_x) if ultimo_x > 1 else 1)
    elif complexidade == 'nW':
        # Para DP, a complexidade depende de n * W.
        # Aqui, W é o último valor de n ou peso_max testado.
        return ultimo_y / (ultimo_x * ultimo_x) # Assumindo W cresce linearmente com n

    return 0.00001


# --- FUNÇÕES DE PLOTAGEM ---

def plotar_forca_bruta(dados_empiricos):
    n_vals = [d[0] for d in dados_empiricos]
    tempos = [d[1] for d in dados_empiricos]
    
    # Curva Teórica (O(2^n))
    c = calcular_constante(n_vals, tempos, '2^n')
    n_teorico = np.array(n_vals)
    tempo_teorico = c * (2**n_teorico)

    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, tempos, 'o-', label='Desempenho Empírico (Medido)')
    plt.plot(n_teorico, tempo_teorico, 'r--', label='Curva Teórica (c * $2^n$)')
    
    plt.title('Análise de Desempenho: Força Bruta')
    plt.xlabel('Número de Itens (n)')
    plt.ylabel('Tempo de Execução (s)')
    plt.legend()
    plt.grid(True)
    plt.yscale('log') # Escala logarítmica é melhor para visualizar crescimento exponencial
    plt.savefig('grafico_forca_bruta.png')
    plt.show()

def plotar_guloso(dados_empiricos):
    n_vals = [d[0] for d in dados_empiricos]
    tempos = [d[1] for d in dados_empiricos]

    # Curva Teórica (O(n log n))
    c = calcular_constante(n_vals, tempos, 'nlogn')
    n_teorico = np.array(n_vals)
    # Evita log(0) ou log(1)
    log_vals = [n * np.log2(n) if n > 1 else 1 for n in n_teorico]
    tempo_teorico = c * np.array(log_vals)
    
    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, tempos, 'o-', label='Desempenho Empírico (Medido)')
    plt.plot(n_teorico, tempo_teorico, 'r--', label='Curva Teórica (c * n log n)')

    plt.title('Análise de Desempenho: Algoritmo Guloso')
    plt.xlabel('Número de Itens (n)')
    plt.ylabel('Tempo de Execução (s)')
    plt.legend()
    plt.grid(True)
    plt.savefig('grafico_guloso.png')
    plt.show()
    
def plotar_pd(dados_empiricos):
    n_vals = [d[0] for d in dados_empiricos]
    tempos = [d[1] for d in dados_empiricos]

    # Curva Teórica (O(n * W)) - Assumindo W cresce com n
    c = calcular_constante(n_vals, tempos, 'nW')
    n_teorico = np.array(n_vals)
    # Se W cresce com n, a complexidade se parece com n*n = n^2
    tempo_teorico = c * (n_teorico**2)

    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, tempos, 'o-', label='Desempenho Empírico (Medido)')
    plt.plot(n_teorico, tempo_teorico, 'r--', label='Curva Teórica (c * $n^2$) - (Assumindo W ∝ n)')

    plt.title('Análise de Desempenho: Programação Dinâmica')
    plt.xlabel('Número de Itens (n) / Capacidade (W)')
    plt.ylabel('Tempo de Execução (s)')
    plt.legend()
    plt.grid(True)
    plt.savefig('grafico_pd.png')
    plt.show()

def plotar_comparativo(dados_bf, dados_guloso, dados_pd):
    plt.figure(figsize=(12, 8))
    
    # Plot Força Bruta
    n_bf = [d[0] for d in dados_bf]
    t_bf = [d[1] for d in dados_bf]
    plt.plot(n_bf, t_bf, 'o-', label='Força Bruta ($O(2^n)$)', color='red')
    
    # Plot Guloso
    n_guloso = [d[0] for d in dados_guloso]
    t_guloso = [d[1] for d in dados_guloso]
    plt.plot(n_guloso, t_guloso, 's-', label='Guloso ($O(n \log n)$)', color='green')

    # Plot PD
    n_pd = [d[0] for d in dados_pd]
    t_pd = [d[1] for d in dados_pd]
    plt.plot(n_pd, t_pd, '^-', label='Prog. Dinâmica ($O(n \cdot W)$)', color='blue')
    
    plt.title('Comparativo de Desempenho dos Algoritmos')
    plt.xlabel('Número de Itens (n)')
    plt.ylabel('Tempo de Execução (s) - Escala Logarítmica')
    plt.legend()
    plt.grid(True, which="both", ls="--")
    plt.xscale('log')
    plt.yscale('log')
    plt.savefig('grafico_comparativo.png')
    plt.show()

# --- BLOCO PRINCIPAL DE EXECUÇÃO ---
if __name__ == "__main__":
    print("Iniciando a simulação de desempenho. Isso pode levar alguns minutos...")

    # --- Testes para Força Bruta (n pequeno) ---
    print("\n1/4: Testando Força Bruta...")
    dados_bf = []
    for n in range(1, 23): # Testar de 1 a 22 itens
        itens = gerar_itens(n)
        peso_total = sum(p for v, p in itens)
        peso_max = int(peso_total * 0.5) # Mochila com 50% da capacidade total
        print(f"  n={n}, W={peso_max}")
        tempo = rodar_teste(n, peso_max, itens)
        dados_bf.append((n, tempo))
    
    # --- Testes para Algoritmo Guloso (n grande) ---
    print("\n2/4: Testando Algoritmo Guloso...")
    dados_guloso = []
    for n in range(10, 5001, 200): # Testar de 10 a 5000 itens
        itens = gerar_itens(n)
        peso_total = sum(p for v, p in itens)
        peso_max = int(peso_total * 0.5)
        print(f"  n={n}, W={peso_max}")
        tempo = rodar_teste(n, peso_max, itens)
        dados_guloso.append((n, tempo))

    # --- Testes para Programação Dinâmica (n e W intermediários) ---
    print("\n3/4: Testando Programação Dinâmica...")
    dados_pd = []
    for n in range(10, 1001, 50): # Testar de 10 a 1000 itens/capacidade
        itens = gerar_itens(n)
        # Para a PD, o peso_max (W) é crucial. Vamos fazê-lo crescer com n.
        peso_max = n 
        print(f"  n={n}, W={peso_max}")
        tempo = rodar_teste(n, peso_max, itens)
        dados_pd.append((n, tempo))
        
    # --- Gerar Gráficos ---
    print("\n4/4: Gerando gráficos...")
    plotar_forca_bruta(dados_bf)
    plotar_guloso(dados_guloso)
    plotar_pd(dados_pd)
    plotar_comparativo(dados_bf, dados_guloso, dados_pd)
    
    print("\nSimulação concluída! Verifique os arquivos .png gerados.")