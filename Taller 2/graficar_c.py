import subprocess
import time
import matplotlib.pyplot as plt

prioridades = [19, 18, 17, 16, 15]  # Prioridades descendentes
tiempos = []

for prioridad in prioridades:
    inicio = time.time()
    subprocess.run(["nice", f"-n{prioridad}", "./factorial"])
    fin = time.time()
    tiempos.append(fin - inicio)

plt.plot(prioridades, tiempos, marker='o')
plt.xlabel('Prioridad')
plt.ylabel('Tiempo de ejecución (segundos)')
plt.title('Comportamiento del programa en C con diferentes prioridades')
plt.grid(True)
plt.show()
