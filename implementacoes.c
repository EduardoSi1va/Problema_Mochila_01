#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct item_
{
    float peso, valor;
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
            printf("%dº: Valor = %.2f, Peso = %.2f\n", j + 1, itens[j].valor, itens[j].peso);
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

float forca_bruta(ITEM itens[], int n, int peso_max)
{
    int total_combinacoes = (int)pow(2, n); // 2^n combinações possíveis (já que cada item pode ou não ser incluído)
    float melhor_valor = 0;

    // Testa todas as combinações possíveis (com índices de 0 a 2^n - 1)
    for (int i = 0; i < total_combinacoes; i++)
    {
        float peso_atual = 0, valor_atual = 0;

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
                if (melhor_comb_binaria % 2 == 1) // Caso um item genérico tenha sido escolhido, marca como 1(dentro da mochila)
                {
                    itens[k].escolhido = true;
                }
                melhor_comb_binaria = melhor_comb_binaria / 2; // Passa para o próximo bit
            }
        }
    }

    return melhor_valor;
}

float algoritmo_guloso(ITEM itens[], int n, float peso_max)
{

    int i;
    float maiorValor = 0; // Somatório dos valores escolhidos
    float maiorPeso = 0;  // Somatório dos pesos escolhidos

    // Aloca dinamicamente o vetor de razões para evitar VLA
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
        scanf("%f %f", &itens[i].valor, &itens[i].peso);
        itens[i].razao = itens[i].valor / itens[i].peso;
        itens[i].escolhido = false; // Atribui false para todos os itens (inicialmente todos estão fora da mochila)
    }

    float valor_forca_bruta = forca_bruta(itens, n, peso_max);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Força Bruta:");
    printf("\nValor máximo (força bruta): %.2f\n", valor_forca_bruta);

    for (int i = 0; i < n; i++)
    {
        itens[i].escolhido = false; // Atribui false aos itens para a saída do próximo algoritmo
    }

    float valor_algoritmo_guloso = algoritmo_guloso(itens, n, peso_max);
    imprimir_escolhidos(itens, n, "Lista de itens escolhidos - Algoritmo Guloso:");
    printf("\nValor máximo (algoritmo guloso): %.2f\n", valor_algoritmo_guloso);

    free(itens);
    return 0;
}