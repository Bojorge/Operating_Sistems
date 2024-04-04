import subprocess
import time
import matplotlib.pyplot as plt

prioridades = [0, 1, 2, 3, 4]  # Prioridades ascendentes
tiempos = []

for prioridad in prioridades:
    inicio = time.time()
    subprocess.run(["nice", f"-n{prioridad}", "python3", "factorial.py", "100"])
    fin = time.time()
    tiempos.append(fin - inicio)

plt.plot(prioridades, tiempos, marker='o')
plt.xlabel('Prioridad')
plt.ylabel('Tiempo de ejecución (segundos)')
plt.title('Comportamiento del programa con diferentes prioridades')
plt.grid(True)
plt.show()
