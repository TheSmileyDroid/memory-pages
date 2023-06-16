echo "Compilando..."
gcc mmu.c -o mmu -lm

# mmu <algoritmo> <% de desvio padrão> <acessos por interrupção> <seed> <num de interrupções> <tau>
# alg: c = clock, w = working set, a = aging
echo "Executando... Relógio com 5% de desvio padrão"
time ./mmu c 5 3000 1 1000
time ./mmu c 5 3000 2 1000
time ./mmu c 5 3000 3 1000
echo "-------------------------"

echo "Executando... WSClock com 5% de desvio padrão"
time ./mmu w 5 3000 1 1000
time ./mmu w 5 3000 2 1000
time ./mmu w 5 3000 3 1000
echo "-------------------------"

echo "Executando... Aging com 5% de desvio padrão"
time ./mmu a 5 3000 1 1000
time ./mmu a 5 3000 2 1000
time ./mmu a 5 3000 3 1000
echo "-------------------------"
exit
echo "Executando... Relógio com 10% de desvio padrão"
time ./mmu c 10 3000 1 1000
time ./mmu c 10 3000 2 1000
time ./mmu c 10 3000 3 1000
echo "-------------------------"

echo "Executando... WSClock com 10% de desvio padrão"
time ./mmu w 10 3000 1 1000
time ./mmu w 10 3000 2 1000
time ./mmu w 10 3000 3 1000
echo "-------------------------"

echo "Executando... Aging com 10% de desvio padrão"
time ./mmu a 10 3000 1 1000
time ./mmu a 10 3000 2 1000
time ./mmu a 10 3000 3 1000
echo "-------------------------"

echo "Executando... Relógio com 25% de desvio padrão"
time ./mmu c 25 3000 1 1000
time ./mmu c 25 3000 2 1000
time ./mmu c 25 3000 3 1000
echo "-------------------------"

echo "Executando... WSClock com 25% de desvio padrão"
time ./mmu w 25 3000 1 1000
time ./mmu w 25 3000 2 1000
time ./mmu w 25 3000 3 1000
echo "-------------------------"

echo "Executando... Aging com 25% de desvio padrão"
time ./mmu a 25 3000 1 1000
time ./mmu a 25 3000 2 1000
time ./mmu a 25 3000 3 1000
echo "-------------------------"

echo "Executando... Relógio com 40% de desvio padrão"
time ./mmu c 40 3000 1 1000
time ./mmu c 40 3000 2 1000
time ./mmu c 40 3000 3 1000
echo "-------------------------"

echo "Executando... WSClock com 40% de desvio padrão"
time ./mmu w 40 3000 1 1000
time ./mmu w 40 3000 2 1000
time ./mmu w 40 3000 3 1000
echo "-------------------------"

echo "Executando... Aging com 40% de desvio padrão"
time ./mmu a 40 3000 1 1000
time ./mmu a 40 3000 2 1000
time ./mmu a 40 3000 3 1000

