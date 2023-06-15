echo "Compilando..."
gcc mmu.c -o mmu -lm

# mmu <algoritmo> <% de desvio padrão> <acessos por interrupção> <seed> <num de interrupções> <tau>
# alg: c = clock, w = working set, a = aging
echo "Executando..."
time ./mmu w 10 3000 1 1000 
time ./mmu w 10 3000 1 1000 
time ./mmu w 10 3000 1 1000 
time ./mmu w 10 3000 1 1000 
