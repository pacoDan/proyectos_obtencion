sudo apt-get update
sudo apt-get install libcurl4-openssl-dev libjson-c-dev


Compilación
Para compilar este programa, necesitas tener libcurl y json-c instalados. Puedes compilarlo con:

sh
Copiar código
gcc -o github_cloner github_cloner.c -lcurl -ljson-c
Ejecución
Ejecuta el programa:

sh
Copiar código
./github_cloner
Este programa clonará todos los repositorios y ramas del usuario especificado (pacodan en este caso).