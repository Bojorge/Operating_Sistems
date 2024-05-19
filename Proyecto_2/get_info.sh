#!/bin/bash

# Archivo que contiene las direcciones IP o nombres de host de las máquinas esclavas
MACHINES_FILE="machines.txt"

# Bucle a través de cada línea del archivo
while IFS= read -r MACHINE; do
    echo "Conectando a $MACHINE..."

    # Obtener el nombre de usuario y hostname de la máquina esclava
    ssh "$MACHINE" 'echo "Usuario: $(whoami), Hostname: $(hostname)"'

done < "$MACHINES_FILE"
