#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Estrutura que representa um item da mochila
typedef struct item_
{
    int peso, valor; // Peso e valor do item
    float razao;     // Razão valor/peso (usado no algoritmo guloso)
    bool escolhido;  // Flag para marcar se o item foi selecionado na solução
} ITEM;

// Função para imprimir itens escolhidos com um título para cada tipo de algoritmo
void imprimir_escolhidos(ITEM itens[], int n, const char *titulo)
{
    printf("\n%s\n", titulo);
    for (int j = 0; j < n; j++)
    {
        if (itens[j].escolhido)
        {
            // Imprime o índice (1-indexado), valor e peso do item escolhido
            printf("%dº: Valor = %d, Peso = %d\n", j + 1, itens[j].valor, itens[j].peso);
        }
    }
}

// Função auxiliar que troca dois elementos de posição em um vetor de floats
void swap(float v[], int i, int j)
{
    float temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

// Implementação do algoritmo QuickSort para ordenar vetor de floats (razões)
void quickSort(float v[], int bottom, int top)
{
    // Escolhe o elemento do meio como pivô
    float pivo = v[(bottom + top) / 2];
    int i = bottom, j = top;

    do
    {
        // Procura um elemento maior que o pivô na parte esquerda
        while (v[i] < pivo)
            i++;
        // Procura um elemento menor que o pivô na parte direita
        while (v[j] > pivo)
            j--;

        // Se os índices ainda não se cruzaram, faz a troca
        if (i <= j)
        {
            swap(v, i, j); // Coloca menor à esquerda e maior à direita do pivô
            i++;
            j--;
        }
    } while (i < j); // Continua até os índices se encontrarem

    // Chamadas recursivas para as duas partições
    if (j > bottom)
        quickSort(v, bottom, j); // Ordena parte esquerda
    if (i < top)
        quickSort(v, i, top); // Ordena parte direita
}

// Algoritmo de força bruta: testa todas as 2^n combinações possíveis
int forca_bruta(ITEM itens[], int n, int peso_max)
{
    // Calcula o número total de combinações (2^n)
    int total_combinacoes = (int)pow(2, n);
    int melhor_valor = 0;

    // Testa cada combinação possível (representada por números de 0 a 2^n-1)
    for (int i = 0; i < total_combinacoes; i++)
    {
        int peso_atual = 0, valor_atual = 0;

        // Converte o número i para binário para determinar quais itens incluir
        int comb_atual_binaria = i;
        for (int j = 0; j < n; j++)
        {
            // Se o bit menos significativo é 1, inclui o item j na combinação
            if (comb_atual_binaria % 2 == 1)
            {
                peso_atual += itens[j].peso;
                valor_atual += itens[j].valor;
            }
            // Remove o último bit (shift direita por divisão)
            comb_atual_binaria = comb_atual_binaria / 2;
        }

        // Verifica se a combinação atual é válida e melhor que a anterior
        if (peso_atual <= peso_max && valor_atual > melhor_valor)
        {
            melhor_valor = valor_atual;

            // Reconstrói a melhor combinação para marcar os itens escolhidos
            int melhor_comb_binaria = i;

            // Limpa todas as escolhas anteriores
            for (int t = 0; t < n; t++)
                itens[t].escolhido = false;

            // Marca os itens da melhor combinação
            for (int k = 0; k < n; k++)
            {
                if (melhor_comb_binaria % 2 == 1)
                {
                    itens[k].escolhido = true;
                }
                melhor_comb_binaria = melhor_comb_binaria / 2;
            }
        }
    }

    return melhor_valor;
}

// Algoritmo guloso: seleciona itens em ordem decrescente de razão valor/peso
int algoritmo_guloso(ITEM itens[], int n, int peso_max)
{
    int maiorValor = 0; // Somatório dos valores dos itens selecionados
    int maiorPeso = 0;  // Somatório dos pesos dos itens selecionados

    // Cria uma cópia do vetor de razões para ordenação
    float *razao = malloc(n * sizeof *razao);
    if (!razao)
    {
        printf("Erro na alocação de memória\n");
        return 0;
    }

    // Copia as razões dos itens para o vetor auxiliar
    for (int i = 0; i < n; i++)
    {
        razao[i] = itens[i].razao;
    }

    // Ordena o vetor de razões em ordem crescente (quicksort)
    quickSort(razao, 0, n - 1);

    // Processa as razões da maior para a menor (percorre de trás para frente)
    for (int i = n - 1; i >= 0 && maiorPeso < peso_max; i--)
    {
        // Para cada razão, encontra o item correspondente no vetor original
        for (int j = 0; j < n; j++)
        {
            // Caso existam itens com razões iguais ou parecidas, verifica se ele ja foi escolhido
            if (itens[j].escolhido)
                continue;

            // Compara a razão atual com a razão do item j
            if (razao[i] == itens[j].razao)
            {
                // Verifica se o item cabe na mochila
                if (maiorPeso + itens[j].peso <= peso_max)
                {
                    maiorPeso += itens[j].peso;   // Adiciona peso ao total
                    maiorValor += itens[j].valor; // Adiciona valor ao total
                    itens[j].escolhido = true;    // Marca item como selecionado
                }
                break; // Caso o item seja encontrado, passa para a próxima razão
            }
        }
    }

    free(razao); // Libera a memória do vetor auxiliar
    return maiorValor;
}

int programacao_dinamica(ITEM itens[], int n, int peso_max, int **tabela_dinamica)
{
    // Casos base: sem itens ou sem capacidade resulta em valor 0
    if (n == 0 || peso_max == 0)
        return 0;

    // Verifica se o subproblema já foi resolvido o -1 é usado como sentinela para indicar o status "não calculado"
    if (tabela_dinamica[n][peso_max] != -1)
        return tabela_dinamica[n][peso_max];

    // Se o item atual não cabe na mochila, não pode ser incluído
    if (itens[n - 1].peso > peso_max)
        return tabela_dinamica[n][peso_max] = programacao_dinamica(itens, n - 1, peso_max, tabela_dinamica);

    // Calcula o valor máximo considerando duas opções:
    // 1. Incluir o item atual: valor do item + melhor solução com capacidade reduzida
    int incluir = itens[n - 1].valor + programacao_dinamica(itens, n - 1, (peso_max - itens[n - 1].peso), tabela_dinamica);

    // 2. Não incluir o item atual: melhor solução sem este item
    int nao_incluir = programacao_dinamica(itens, n - 1, peso_max, tabela_dinamica);

    // Armazena e retorna o melhor resultado entre as duas opções
    return tabela_dinamica[n][peso_max] = (incluir > nao_incluir) ? incluir : nao_incluir;
}

void itens_escolhidos_dp(ITEM itens[], int n, int peso_max, int **tabela_dinamica)
{
    // Percorre a tabela do estado final (n, peso_max) até o inicial (0, 0)
    while (n > 0 && peso_max > 0)
    {
        // Se o valor com n itens difere do valor com n-1 itens, significa que o item n foi incluído na solução
        if (tabela_dinamica[n][peso_max] != tabela_dinamica[n - 1][peso_max])
        {
            itens[n - 1].escolhido = true; // Marca o item como escolhido
            peso_max -= itens[n - 1].peso; // Reduz a capacidade pelo peso do item
        }
        n--; // Move para o item anterior
    }
}

int main(void)
{
    int n, peso_max;

    // Lê o número de itens e a capacidade máxima da mochila
    scanf("%d %d", &n, &peso_max);

    if (n <= 0)
    {
        printf("Nenhum item\n");
        return 0;
    }

    // Aloca memória dinamicamente para o vetor de itens
    ITEM *itens = malloc(n * sizeof(ITEM));
    if (!itens)
    {
        printf("Erro na alocação de memória\n");
        return 1;
    }

    // Lê os dados de cada item (valor e peso) e calcula a razão valor/peso
    for (int i = 0; i < n; i++)
    {
        scanf("%d %d", &itens[i].valor, &itens[i].peso);
        if (itens[i].peso <= 0)
        {
            printf("Entrada de peso inválida");
            return 1;
        }
        itens[i].razao = (float)itens[i].valor / (float)itens[i].peso; // Calcula a utilidade (razão) do item
        itens[i].escolhido = false; // Inicializa como não escolhido
    }

    int valor_forca_bruta = forca_bruta(itens, n, peso_max);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Força Bruta:");
    printf("\nValor máximo (força bruta): %d\n", valor_forca_bruta);

    // Limpa as escolhas para o próximo algoritmo
    for (int i = 0; i < n; i++)
    {
        itens[i].escolhido = false;
    }

    int valor_algoritmo_guloso = algoritmo_guloso(itens, n, peso_max);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Algoritmo Guloso:");
    printf("\nValor máximo (algoritmo guloso): %d\n", valor_algoritmo_guloso);

    // Limpa as escolhas antes da programação dinâmica
    for (int i = 0; i < n; ++i)
        itens[i].escolhido = false;

    // Dimensões da tabela: (n+1) linhas × (peso_max+1) colunas
    int linhas = n + 1;
    int colunas = peso_max + 1;
    int total = linhas * colunas;

    // Aloca um bloco contíguo para todos os elementos da tabela
    int *celulas = malloc(total * sizeof(int));
    if (!celulas)
    {
        printf("Erro: Sem memória para alocação das células da tabela dinamica\n");
        free(itens);
        return 1;
    }

    // Aloca vetor de ponteiros para permitir acesso bidimensional tabela[i][j]
    int **tabela_dinamica = malloc(linhas * sizeof(int *));
    if (!tabela_dinamica)
    {
        printf("Erro: Sem memória para alocação dos ponteiros da tabela\n");
        free(celulas);
        free(itens);
        return 1;
    }

    // Configura cada ponteiro de linha para apontar para a posição correta no bloco contíguo de memória
    for (int i = 0; i < linhas; ++i)
        tabela_dinamica[i] = celulas + i * colunas;

    // Inicializa toda a tabela com -1 (sentinela para "não calculado")
    for (int j = 0; j < total; ++j)
        celulas[j] = -1;

    int valor_pd = programacao_dinamica(itens, n, peso_max, tabela_dinamica);

    // Reconstrói quais itens foram escolhidos na solução da programação dinâmica
    itens_escolhidos_dp(itens, n, peso_max, tabela_dinamica);

    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Programacao Dinamica:");
    printf("\nValor máximo (programacao dinamica): %d\n", valor_pd);

    free(tabela_dinamica); // Libera o vetor de ponteiros
    free(celulas);         // Libera o bloco contíguo de dados
    free(itens);           // Libera o vetor de itens

    return 0;
}