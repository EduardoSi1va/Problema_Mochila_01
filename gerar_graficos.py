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
    Executa o programa C com os dados fornecidos e retorna o tempo de execução em milissegundos.
    """
    input_data = f"{n}\n{peso_max}\n"
    for valor, peso in itens:
        input_data += f"{valor} {peso}\n"

    start_time = time.time()
    
    subprocess.run(
        [EXECUTAVEL_C],
        input=input_data,
        text=True,
        capture_output=True
    )
    
    end_time = time.time()
    
    # Retorna o tempo em milissegundos
    return (end_time - start_time) * 1000

def calcular_constante(x, y, complexidade):
    """Estima a constante de escala 'c' para a curva teórica."""
    if not y or not x or y[-1] == 0:
        return 0.00001
    
    # Filtra pontos onde o tempo foi zero para evitar divisão por zero ou resultados estranhos
    valid_points = [(xi, yi) for xi, yi in zip(x, y) if yi > 0]
    if not valid_points:
        return 0.00001
        
    ultimo_x, ultimo_y = valid_points[-1]
    
    if complexidade == '2^n' and ultimo_x > 0:
        return ultimo_y / (2**ultimo_x)
    elif complexidade == 'nlogn' and ultimo_x > 1:
        return ultimo_y / (ultimo_x * np.log2(ultimo_x))
    elif complexidade == 'nW' and ultimo_x > 0:
        return ultimo_y / (ultimo_x * ultimo_x)

    return 0.00001


# --- FUNÇÕES DE PLOTAGEM (ATUALIZADAS PARA MS) ---

def plotar_forca_bruta(dados_empiricos):
    n_vals = [d[0] for d in dados_empiricos]
    tempos = [d[1] for d in dados_empiricos]
    
    c = calcular_constante(n_vals, tempos, '2^n')
    n_teorico = np.array(n_vals)
    tempo_teorico = c * (2**n_teorico)

    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, tempos, 'o-', label='Desempenho Empírico (Medido)')
    plt.plot(n_teorico, tempo_teorico, 'r--', label='Curva Teórica (c * $2^n$)')
    
    plt.title('Análise de Desempenho: Força Bruta')
    plt.xlabel('Número de Itens (n)')
    plt.ylabel('Tempo de Execução (ms)') # ATUALIZADO
    plt.legend()
    plt.grid(True)
    plt.yscale('log')
    plt.savefig('grafico_forca_bruta_ms.png')
    plt.show()

def plotar_guloso(dados_empiricos):
    # Filtra pontos onde o programa pode ter falhado (tempo ~0 para n grande)
    dados_validos = [d for d in dados_empiricos if not (d[0] > 1000 and d[1] < 1)]
    n_vals = [d[0] for d in dados_validos]
    tempos = [d[1] for d in dados_validos]

    c = calcular_constante(n_vals, tempos, 'nlogn')
    n_teorico = np.array(n_vals)
    log_vals = [n * np.log2(n) if n > 1 else 1 for n in n_teorico]
    tempo_teorico = c * np.array(log_vals)
    
    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, tempos, 'o-', label='Desempenho Empírico (Medido)')
    plt.plot(n_teorico, tempo_teorico, 'r--', label='Curva Teórica (c * n log n)')

    plt.title('Análise de Desempenho: Algoritmo Guloso')
    plt.xlabel('Número de Itens (n)')
    plt.ylabel('Tempo de Execução (ms)') # ATUALIZADO
    plt.legend()
    plt.grid(True)
    plt.savefig('grafico_guloso_ms.png')
    plt.show()
    
def plotar_pd(dados_empiricos):
    n_vals = [d[0] for d in dados_empiricos]
    tempos = [d[1] for d in dados_empiricos]

    c = calcular_constante(n_vals, tempos, 'nW')
    n_teorico = np.array(n_vals)
    tempo_teorico = c * (n_teorico**2)

    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, tempos, 'o-', label='Desempenho Empírico (Medido)')
    plt.plot(n_teorico, tempo_teorico, 'r--', label='Curva Teórica (c * $n^2$) - (Assumindo W ∝ n)')

    plt.title('Análise de Desempenho: Programação Dinâmica')
    plt.xlabel('Número de Itens (n) / Capacidade (W)')
    plt.ylabel('Tempo de Execução (ms)') # ATUALIZADO
    plt.legend()
    plt.grid(True)
    plt.savefig('grafico_pd_ms.png')
    plt.show()

def plotar_comparativo(dados_bf, dados_guloso, dados_pd):
    plt.figure(figsize=(12, 8))
    
    n_bf = [d[0] for d in dados_bf]
    t_bf = [d[1] for d in dados_bf]
    plt.plot(n_bf, t_bf, 'o-', label='Força Bruta ($O(2^n)$)', color='red')
    
    # Filtra possíveis falhas do guloso para um gráfico mais limpo
    dados_guloso_validos = [d for d in dados_guloso if not (d[0] > 1000 and d[1] < 1)]
    n_guloso = [d[0] for d in dados_guloso_validos]
    t_guloso = [d[1] for d in dados_guloso_validos]
    plt.plot(n_guloso, t_guloso, 's-', label='Guloso ($O(n \log n)$)', color='green')

    n_pd = [d[0] for d in dados_pd]
    t_pd = [d[1] for d in dados_pd]
    plt.plot(n_pd, t_pd, '^-', label='Prog. Dinâmica ($O(n \cdot W)$)', color='blue')
    
    plt.title('Comparativo de Desempenho dos Algoritmos')
    plt.xlabel('Número de Itens (n)')
    plt.ylabel('Tempo de Execução (ms) - Escala Logarítmica') # ATUALIZADO
    plt.legend()
    plt.grid(True, which="both", ls="--")
    plt.xscale('log')
    plt.yscale('log')
    plt.savefig('grafico_comparativo_ms.png')
    plt.show()

# --- BLOCO PRINCIPAL DE EXECUÇÃO (COM NOVOS INTERVALOS) ---
if __name__ == "__main__":
    print("Iniciando a simulação de desempenho. Isso pode levar alguns minutos...")

    # --- Testes para Força Bruta (n pequeno, mantido) ---
    print("\n1/4: Testando Força Bruta...")
    dados_bf = []
    for n in range(1, 23): # Testar de 1 a 22 itens
        itens = gerar_itens(n)
        peso_total = sum(p for v, p in itens)
        peso_max = int(peso_total * 0.5)
        print(f"  n={n}, W={peso_max}")
        tempo = rodar_teste(n, peso_max, itens)
        dados_bf.append((n, tempo))
    
    # --- NOVOS INTERVALOS PARA GULOSO E PD ---
    # Define uma lista de 'n' com mais pontos no início e mais espaçados no final
    n_testes_polinomiais = list(range(1, 21))  # n de 1 a 20, passo 1
    n_testes_polinomiais += list(range(21, 201, 20)) # n de 21 a 200, passo 20
    n_testes_polinomiais += list(range(201, 2001, 100)) # n de 201 a 2000, passo 100
    
    # --- Testes para Algoritmo Guloso ---
    print("\n2/4: Testando Algoritmo Guloso...")
    dados_guloso = []
    for n in n_testes_polinomiais:
        itens = gerar_itens(n)
        peso_total = sum(p for v, p in itens)
        peso_max = int(peso_total * 0.5)
        print(f"  n={n}, W={peso_max}")
        tempo = rodar_teste(n, peso_max, itens)
        dados_guloso.append((n, tempo))

    # --- Testes para Programação Dinâmica ---
    print("\n3/4: Testando Programação Dinâmica...")
    dados_pd = []
    for n in n_testes_polinomiais:
        itens = gerar_itens(n)
        peso_max = n # W cresce linearmente com n para testar o pior caso da PD
        print(f"  n={n}, W={peso_max}")
        tempo = rodar_teste(n, peso_max, itens)
        dados_pd.append((n, tempo))
        
    # --- Gerar Gráficos ---
    print("\n4/4: Gerando gráficos...")
    plotar_forca_bruta(dados_bf)
    plotar_guloso(dados_guloso)
    plotar_pd(dados_pd)
    plotar_comparativo(dados_bf, dados_guloso, dados_pd)
    
    print("\nSimulação concluída! Verifique os novos arquivos .png gerados (com sufixo _ms).")