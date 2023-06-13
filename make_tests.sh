echo "Criando arquivos de teste..."
python rng.py 268435456 

echo "Compilando..."
gcc mmu.c -o mmu


for i in 1
do
    for j in 1000000
    do
        echo "Executando teste $i com tempo de acesso $j usando o algoritmo de substituição Relógio..."
        ./mmu c 1000000 < "input$i.txt" > "output$i-$j-c.txt"
    done
done
