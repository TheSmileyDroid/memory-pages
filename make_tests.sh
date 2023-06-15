echo "Compilando..."
gcc mmu.c -o mmu -lm

# mmu <algoritmo> <% de desvio padrão> <acessos por interrupção> <seed> <num de interrupções> <tau>
./mmu c 10 3000 1 10
./mmu w 10 3000 1 10 2000
./mmu a 10 300 1 10
