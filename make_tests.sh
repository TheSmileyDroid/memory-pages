echo "Criando arquivos de teste..."
python rng.py 2047 4095 6143 8195

echo "Compilando..."
gcc mmu.c -o mmu


for i in {1..4}
do
    echo "Executando teste $i..."
    ./mmu c < "input$i.txt" > "output$i.txt"
done
