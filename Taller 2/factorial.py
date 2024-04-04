import time



# Función recursiva para calcular el factorial
def factorial(n):
    print("---")
    if n == 0 or n == 1:
        return 1
    else:
        return n * factorial(n - 1)

# Función principal
def main():
    num = 200

    # Inicia el temporizador
    start_time = time.time()

    fact = factorial(num)
    # Detiene el temporizador
    end_time = time.time()

    # Calcula el tiempo transcurrido
    elapsed_time = end_time - start_time

    print(f"El factorial de {num} es {fact}")
    print(f"Tiempo de ejecución: {elapsed_time:.9f} segundos")

if __name__ == "__main__":
    main()

