#include <stdio.h>
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