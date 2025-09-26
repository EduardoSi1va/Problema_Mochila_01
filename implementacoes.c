#include <stdio.h>
#include <math.h>

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