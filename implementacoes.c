#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdbool.h>

typedef struct item_{
    float valor, peso;
    float razao;
    bool escolhido;
}ITEM;

//Função que troca duas posições de um vetor de lugar
void swap(int v[], int i, int j)
{
    int temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

void quicksort(int v[], int bottom, int top)
{
    int pivo = v[(bottom + top) / 2];
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
        quicksort(v, bottom, j);
    if (i < top)
        quicksort(v, i, top);
}

float algoritmo_guloso(ITEM* itens[], int n, float pesoMax){

    int i;
    float razao[n]; //Cria um vetor auxiliar que armazena as razões
    float maiorValor = 0; //Somatório dos valores escolhidos
    float maiorPeso = 0; //Somatório dos pesos escolhidos

    //Percorre o vetor de itens e coloca as razões no vetor de razões
    for(i = 0; i<n; i++){

        razao[i] = itens[i]->razao;

    }

    //Faz a ordenação do vetor auxiliar pelo algoritmo de quicksort
    quick_sort(razao,0,n);

    //Pega a partir da maior razão do vetor auxiliar e vai diminuindo
    //O loop acaba ou quando o somatório dos pesos for maior que o peso máximo
    //ou quando acabar o vetor.
    for(i = n; i>0 && maiorPeso<pesoMax; i--){
        for(int j = 0; j<n; j++){ //A cada razao escolhida, ele percorre o vetor original de itens
            if(razao[i] == itens[j]->razao){ //Percorre até achar o item que possui a razão desejada
                maiorPeso += itens[j]->peso; //Adiciona o peso dele ao somatório de pesos
                maiorValor += itens[j]->valor; //Adiciona o valor dele ao somatório de valores
                itens[j]->escolhido = true; //Marca-o como escolhido
            }

        }

    }

    return maiorValor; //Retorna o maior somatório de valores aproximado

}

typedef struct item_ 
{
    float peso, valor;
    float razao;
    bool escolhido; // Variável para verificar quais itens foram escolhidos na combinação final de saída.
} ITEM;

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
            for (int k = 0; k < n; k++)
            {
                if (melhor_comb_binaria % 2 == 1) //Caso um item genérico tenha sido escolhido, marca como 1(dentro da mochila)
                {
                    itens[k].escolhido = 1;
                }
                melhor_comb_binaria = melhor_comb_binaria / 2; // Passa para o próximo bit 
            }
        }
    }

    return melhor_valor;
}

int main(void)
{
    int n, peso_max;
    scanf("%d %d", &n, &peso_max);
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
        itens[i].escolhido = 0; // Atribui 0 para todos os itens (inicialmente todos estão fora da mochila)
    }

    float valor_forca_bruta = forca_bruta(itens, n, peso_max);

    printf("\nLista de itens escolhidos:\n");
    for (int j = 0; j < n; j++)
    {
        if (itens[j].escolhido)
        {
            printf("%dº: Valor = %.2f, Peso = %.2f\n", j + 1, itens[j].valor, itens[j].peso);
        }
    }
    printf("\nValor máximo (força bruta): %.2f\n", valor_forca_bruta);

    free(itens);
    return 0;
}