#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Struct que representa um item da mochila
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

// Função auxiliar que troca dois ITENS de posição
void swap_itens(ITEM v[], int i, int j)
{
    ITEM temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

// Implementação do algoritmo QuickSort para ordenar um vetor de ITENS pela razao (decrescente)
void quickSort(ITEM v[], int bottom, int top)
{
    // Escolhe o elemento do meio como pivô
    ITEM pivo = v[(bottom + top) / 2];
    int i = bottom, j = top;

    do
    {
        // Procura um elemento menor que o pivô na parte esquerda (ordem decrescente)
        while (v[i].razao > pivo.razao)
            i++;
        // Procura um elemento maior que o pivô na parte direita (ordem decrescente)
        while (v[j].razao < pivo.razao)
            j--;

        // Se os índices ainda não se cruzaram, faz a troca
        if (i <= j)
        {
            swap_itens(v, i, j);
            i++;
            j--;
        }
    } while (i <= j); // Erro corrigido aqui (era i < j, o correto é i <= j)

    // Chamadas recursivas para as duas partições
    if (j > bottom)
        quickSort(v, bottom, j); // Ordena parte esquerda
    if (i < top)
        quickSort(v, i, top); // Ordena parte direita
}

// Algoritmo de força bruta: testa todas as 2^n combinações possíveis
int forca_bruta(ITEM itens[], int n, int peso_max)
{

    int total_combinacoes = 1;
    for (int k = 0; k < n; k++) // Calcula o número total de combinações (2^n)
        total_combinacoes *= 2;

    int melhor_valor = 0;
    int melhor_comb_binaria_final = 0;

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
            melhor_comb_binaria_final = i;
        }
    }

    // Marca os itens da melhor combinação DEPOIS de testar todas
    for (int k = 0; k < n; k++)
    {
        if ((melhor_comb_binaria_final >> k) & 1)
        {
            itens[k].escolhido = true;
        }
        else
        {
            itens[k].escolhido = false;
        }
    }

    return melhor_valor;
}

// Algoritmo guloso O(n log n): ordena os itens pela razão valor/peso e seleciona os melhores.
int algoritmo_guloso(ITEM itens[], int n, int peso_max)
{
    int maiorValor = 0; // Somatório dos valores dos itens selecionados
    int maiorPeso = 0;  // Somatório dos pesos dos itens selecionados

    // Ordena o array de itens diretamente, em ordem decrescente de razão.
    quickSort(itens, 0, n - 1);

    // Percorre o array já ordenado e adicionar os itens à mochila.
    for (int i = 0; i < n; i++)
    {
        // Verifica se o item atual (que tem a melhor razão disponível) cabe na mochila
        if (maiorPeso + itens[i].peso <= peso_max)
        {
            maiorPeso += itens[i].peso;   // Adiciona peso ao total
            maiorValor += itens[i].valor; // Adiciona valor ao total
            itens[i].escolhido = true;    // Marca item como selecionado
        }
    }
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
    printf("Digite a quantidade de itens: ");
    int n;
    scanf("%d", &n);
    printf("\n");
    printf("Digite o peso máximo suportado pela mochila: ");
    int peso_max;
    scanf("%d", &peso_max);
    printf("\n");

    if (n <= 0)
    {
        printf("Nenhum item\n");
        return 0;
    }

    ITEM *itens_original = malloc(n * sizeof(ITEM));
    ITEM *itens_guloso = malloc(n * sizeof(ITEM)); // Vetor de itens auxiliar para o algoritmo guloso (já que esse faz a ordenação dos itens mediante do quicksort)

    if (!itens_original || !itens_guloso)
    {
        printf("Erro na alocação de memória\n");
        return 1;
    }

    // Lê os dados de cada item e já faz a cópia
    for (int i = 0; i < n; i++)
    {
        scanf("%d %d", &itens_original[i].valor, &itens_original[i].peso);
        if (itens_original[i].peso <= 0)
        {
            printf("Entrada de peso inválida");
            return 1;
        }
        itens_original[i].razao = (float)itens_original[i].valor / (float)itens_original[i].peso;
        itens_original[i].escolhido = false;

        // Copia os dados para o array que será usado pelo guloso
        itens_guloso[i] = itens_original[i];
    }

    // A força bruta usa o array original
    int valor_forca_bruta = forca_bruta(itens_original, n, peso_max);
    imprimir_escolhidos(itens_original, n, "Lista de itens escolhidos - Força Bruta:");
    printf("\nValor máximo (força bruta): %d\n", valor_forca_bruta);

    // O algoritmo guloso usa SUA PRÓPRIA CÓPIA para poder ordenar sem afetar os outros
    int valor_algoritmo_guloso = algoritmo_guloso(itens_guloso, n, peso_max);
    imprimir_escolhidos(itens_guloso, n, "Lista de itens escolhidos - Algoritmo Guloso:");
    printf("\nValor máximo (algoritmo guloso): %d\n", valor_algoritmo_guloso);

    // Limpa as escolhas do array original antes da programação dinâmica
    for (int i = 0; i < n; ++i)
        itens_original[i].escolhido = false;

    int linhas = n + 1;
    int colunas = peso_max + 1;
    int total = linhas * colunas;

    int *celulas = malloc(total * sizeof(int));
    if (!celulas)
    {
        printf("Sem memória para alocação das células da tabela dinamica\n");
        free(itens_original);
        free(itens_guloso);
        return 1;
    }

    int **tabela_dinamica = malloc(linhas * sizeof(int *));
    if (!tabela_dinamica)
    {
        printf("Sem memória para alocação dos ponteiros da tabela\n");
        free(celulas);
        free(itens_original);
        free(itens_guloso);
        return 1;
    }

    for (int i = 0; i < linhas; ++i)
        tabela_dinamica[i] = celulas + i * colunas;

    for (int j = 0; j < total; ++j)
        celulas[j] = -1;

    int valor_pd = programacao_dinamica(itens_original, n, peso_max, tabela_dinamica);
    itens_escolhidos_dp(itens_original, n, peso_max, tabela_dinamica);
    imprimir_escolhidos(itens_original, n, "Lista de itens escolhidos - Programacao Dinamica:");
    printf("\nValor máximo (programacao dinamica): %d\n", valor_pd);

    free(tabela_dinamica);
    free(celulas);
    free(itens_original);
    free(itens_guloso);

    return 0;
}