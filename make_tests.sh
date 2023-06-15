echo "Compilando..."
gcc mmu.c -o mmu -lm

# mmu <algoritmo> <% de desvio padrão> <acessos por interrupção> <tau>
./mmu c 10 100
./mmu w 10 100 10000
./mmu a 10 100
