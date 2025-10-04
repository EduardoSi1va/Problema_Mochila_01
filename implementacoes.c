#include <stdio.h>
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

float forca_bruta(float pesos[], float valores[], int n, int peso_max)
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
                peso_atual += pesos[j];
                valor_atual += valores[j];
            }
            comb_atual_binaria = comb_atual_binaria / 2; // Remove o último bit (divisão inteira por 2)
        }

        // Se o peso não excede o limite e o valor é melhor, atualiza
        if (peso_atual <= peso_max && valor_atual > melhor_valor)
        {
            melhor_valor = valor_atual;
        }
    }

    return melhor_valor;
}

int main(void)
{
    int n, peso_max;
    scanf("%d %d", &n, &peso_max);
    float valores[n], pesos[n];

    for (int i = 0; i < n; i++)
    {
        scanf("%f %f", &valores[i], &pesos[i]);
    }

    float valor_forca_bruta = forca_bruta(pesos, valores, n, peso_max);
    printf("Valor máximo (força bruta): %.2f\n", valor_forca_bruta);

    return 0;
}