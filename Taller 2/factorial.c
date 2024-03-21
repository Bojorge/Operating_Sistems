#include <stdio.h>
#include <time.h>

// Función recursiva para calcular el factorial
unsigned long long factorial(unsigned int n) {
    printf("------");
    if (n == 0 || n == 1)
        return 1;
    else
        return n * factorial(n - 1);
}

int main() {
    unsigned int num = 200;
    
    // Inicia el temporizador
    clock_t start = clock();

    unsigned long long fact;
    fact = factorial(num);
    // Detiene el temporizador
    clock_t end = clock();
    
    // Calcula el tiempo transcurrido
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("El factorial de %d es %llu\n", num, fact);
    printf("Tiempo de ejecución: %.9f segundos\n", time_spent);

    return 0;
}
