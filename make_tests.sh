echo "Criando arquivos de teste..."
python rng.py 2047 4095 8195 12287 16383 24575 32767

echo "Compilando..."
gcc mmu.c -o mmu


for i in {1..7}
do
    for j in {500..1000..500}
    do
        echo "Executando teste $i com tempo de acesso $j usando o algoritmo de substituição Relógio..."
        ./mmu c $j < "input$i.txt" > "output$i-$j-c.txt"
    done
done
