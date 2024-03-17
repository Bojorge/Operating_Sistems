#include <stdio.h>

// Función recursiva para calcular el factorial
unsigned long long factorial(unsigned long long n) {
    printf("C");
    if (n == 0 || n == 1)
        return 1;
    else
        return n * factorial(n - 1);
}

int main() {
    int num = 100;
    unsigned long long fact;

    fact = factorial(num);
    printf("El factorial de %d es: %llu\n", num, fact);

    return 0;
}
