import sys
sys.setrecursionlimit(10000)

# Función recursiva para calcular el factorial
def factorial(n):
    print("Python")
    if n == 0 or n == 1:
        return 1
    else:
        return n * factorial(n - 1)

# Función principal
def main():
    num = 1000

    fact = factorial(num)
    print(f"El factorial de {num} es: {fact}")

if __name__ == "__main__":
    main()
