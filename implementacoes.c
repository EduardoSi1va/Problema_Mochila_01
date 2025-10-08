#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct item_
{
    int peso, valor;
    float razao;
    bool escolhido; // Variável para verificar quais itens foram escolhidos na combinação final de saída.
} ITEM;

// Função para imprimir itens escolhidos com um título
void imprimir_escolhidos(ITEM itens[], int n, const char *titulo)
{
    printf("\n%s\n", titulo);
    for (int j = 0; j < n; j++)
    {
        if (itens[j].escolhido)
        {
            printf("%dº: Valor = %d, Peso = %d\n", j + 1, itens[j].valor, itens[j].peso);
        }
    }
}

// Função que troca duas posições de um vetor de lugar
void swap(float v[], int i, int j)
{
    float temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

void quickSort(float v[], int bottom, int top)
{
    float pivo = v[(bottom + top) / 2];
    int i = bottom, j = top;
    do
    {
        while (v[i] < pivo) // acha um elemento maior que o pivo (lado esquerdo)
            i++;
        while (v[j] > pivo) // acha um elemento menor que o pivo (lado direito)
            j--;
        if (i <= j)
        {
            swap(v, i, j); // troca os valores, maior que o pivo à direita e menor que o pivo à esquerda
            i++;
            j--;
        }
    } while (i < j); // Quando os índices se encontrarem, encerra o laço de repetição

    if (j > bottom)
        quickSort(v, bottom, j);
    if (i < top)
        quickSort(v, i, top);
}

int forca_bruta(ITEM itens[], int n, int peso_max)
{
    int total_combinacoes = (int)pow(2, n); // 2^n combinações possíveis (já que cada item pode ou não ser incluído)
    int melhor_valor = 0;

    // Testa todas as combinações possíveis (com índices de 0 a 2^n - 1)
    for (int i = 0; i < total_combinacoes; i++)
    {
        int peso_atual = 0, valor_atual = 0;

        int comb_atual_binaria = i; // A cada iteração do for, pega um bit do número (ex: 5 = 1->0->1)
        for (int j = 0; j < n; j++)
        {
            // Se o último bit é 1, inclui o item j
            if (comb_atual_binaria % 2 == 1)
            {
                peso_atual += itens[j].peso;
                valor_atual += itens[j].valor;
            }
            comb_atual_binaria = comb_atual_binaria / 2; // Remove o último bit (divisão inteira por 2)
        }

        // Se o peso não excede o limite e o valor é melhor, atualiza
        if (peso_atual <= peso_max && valor_atual > melhor_valor)
        {
            melhor_valor = valor_atual;

            int melhor_comb_binaria = i; // Atribui o número da melhor combinação encontrada
            // limpar escolhas anteriores e marcar a nova melhor combinação
            for (int t = 0; t < n; t++)
                itens[t].escolhido = false;
            for (int k = 0; k < n; k++)
            {
                if (melhor_comb_binaria % 2 == 1) // Caso um item genérico tenha sido escolhido, marca como 1 (dentro da mochila)
                {
                    itens[k].escolhido = true;
                }
                melhor_comb_binaria = melhor_comb_binaria / 2; // Passa para o próximo bit
            }
        }
    }

    return melhor_valor;
}

int algoritmo_guloso(ITEM itens[], int n, int peso_max)
{

    int i;
    int maiorValor = 0; // Somatório dos valores escolhidos
    int maiorPeso = 0;  // Somatório dos pesos escolhidos

    // Alocação dinâmica do vetor de razões
    float *razao = malloc(n * sizeof *razao);
    if (!razao)
    {
        printf("Erro na alocação de memória\n");
        return 0;
    }

    // Percorre o vetor de itens e coloca as razões no vetor de razões
    for (i = 0; i < n; i++)
    {
        razao[i] = itens[i].razao;
    }

    // Faz a ordenação do vetor auxiliar pelo algoritmo de quicksort
    quickSort(razao, 0, n - 1);

    // Pega a partir da maior razão do vetor auxiliar e vai diminuindo
    // O loop acaba ou quando o somatório dos pesos for maior que o peso máximo
    // ou quando acabar o vetor.
    for (i = n - 1; i >= 0 && maiorPeso < peso_max; i--)
    {
        for (int j = 0; j < n; j++) // A cada razao escolhida, ele percorre o vetor original de itens
        {
            if (razao[i] == itens[j].razao)
            { // Percorre até achar o item que possui a razão desejada
                if (maiorPeso + itens[j].peso <= peso_max)
                {
                    maiorPeso += itens[j].peso;   // Adiciona o peso dele ao somatório de pesos
                    maiorValor += itens[j].valor; // Adiciona o valor dele ao somatório de valores
                    itens[j].escolhido = true;    // Marca-o como escolhido
                }
                break; // item encontrado, passa para a próxima razão
            }
        }
    }
    free(razao);

    return maiorValor; // Retorna o maior somatório de valores aproximado
}

// Função recursiva que se baseia nas 
int programacao_dinamica(ITEM itens[], int n, int peso_max, int **tabela_dinamica)
{
    // Casos bases para que as chamadas recursivas acabem
    if (n == 0 || peso_max == 0)
        return 0;

    // Caso a célula da tabela já tenha sido calculada, retorna ela mesma
    if (tabela_dinamica[n][peso_max] != -1)
        return tabela_dinamica[n][peso_max];

    // Caso o peso do item seja maior que o máximo, o item não pode ser incluído na mochila
    if (itens[n - 1].peso > peso_max)
        return tabela_dinamica[n][peso_max] = programacao_dinamica(itens, n - 1, peso_max, tabela_dinamica);

    // Calcula a "utilidade" gerada ao incluir e ao não incluir o item na mochila
    int incluir = itens[n - 1].valor + programacao_dinamica(itens, n - 1, (peso_max - itens[n - 1].peso), tabela_dinamica);
    int nao_incluir = programacao_dinamica(itens, n - 1, peso_max, tabela_dinamica);

    // Atribui o melhor resultado entre as duas possibilidades à tabela
    return tabela_dinamica[n][peso_max] = (incluir > nao_incluir) ? incluir : nao_incluir;
}

// Função para verificar quais itens da tabela dinâmica foram escolhidos 
void itens_escolhidos_dp(ITEM itens[], int n, int peso_max, int **tabela_dinamica)
{
    while (n > 0 && peso_max > 0)
    {
        if (tabela_dinamica[n][peso_max] != tabela_dinamica[n - 1][peso_max])
        {
            itens[n - 1].escolhido = 1;
            peso_max -= itens[n - 1].peso;
        }
        n--;
    }
}

int main(void)
{
    int n, peso_max;
    scanf("%d %d", &n, &peso_max);
    if (n <= 0)
    {
        printf("Nenhum item\n");
        return 0;
    }


    ITEM *itens = malloc(n * sizeof(ITEM));

    if (!itens)
    {
        printf("Erro na alocação de memória\n");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        scanf("%d %d", &itens[i].valor, &itens[i].peso);
        itens[i].razao = (float)itens[i].valor / (float)itens[i].peso; // razão como float
        itens[i].escolhido = false;                                    // Atribui false para todos os itens (inicialmente todos estão fora da mochila)
    }

    int valor_forca_bruta = forca_bruta(itens, n, peso_max);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Força Bruta:");
    printf("\nValor máximo (força bruta): %d\n", valor_forca_bruta);

    for (int i = 0; i < n; i++)
    {
        itens[i].escolhido = false; // Atribui false aos itens novamente para a saída do próximo algoritmo
    }

    int valor_algoritmo_guloso = algoritmo_guloso(itens, n, peso_max);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Algoritmo Guloso:");
    printf("\nValor máximo (algoritmo guloso): %d\n", valor_algoritmo_guloso);

    // ----- Programação Dinâmica (top-down com tabela contígua) -----
    // Aloca a tabela contígua (linhas = n+1, colunas = peso_max+1)
    int linhas = n + 1;
    int colunas = peso_max + 1;
    int total = linhas * colunas;

    int *buffer = malloc(total * sizeof(int));
    if (!buffer) {
        printf("Erro: sem memoria para tabela dinamica\n");
        free(itens);
        return 1;
    }

    int **tabela_dinamica = malloc(linhas * sizeof(int *));
    if (!tabela_dinamica) {
        printf("Erro: sem memoria para ponteiros da tabela\n");
        free(buffer);
        free(itens);
        return 1;
    }
    for (int i = 0; i < linhas; ++i) tabela_dinamica[i] = buffer + i * colunas;

    // inicializa com -1 (sentinela de nao calculado)
    for (int i = 0; i < total; ++i) buffer[i] = -1;

    // chama a programação dinâmica (preenche a tabela sob demanda)
    int valor_pd = programacao_dinamica(itens, n, peso_max, tabela_dinamica);
    // reconstrói os itens escolhidos a partir da tabela
    itens_escolhidos_dp(itens, n, peso_max, tabela_dinamica);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Programacao Dinamica:");
    printf("\nValor máximo (programacao dinamica): %d\n", valor_pd);

    // libera a tabela
    free(tabela_dinamica);
    free(buffer);

    free(itens);
    return 0;
}