#Executar: /benchmark1.sh
#Mude EXEC para alterar a versao do codigo a ser executada(par: paralelo, seq: sequencial)
#!/bin/bash

EXEC=./par
INPUT=entrada.txt
RUNS=15

tempos=()

echo "Rodando $RUNS execuções..."

for ((i=1; i<=RUNS; i++)); do
    output=$($EXEC $INPUT)

    # pega o tempo da última linha
    tempo=$(echo "$output" | grep "Tempo de resposta" | awk '{print $NF}' | sed 's/s//')

    tempos+=($tempo)

    echo "Run $i: $tempo s"
done

# cálculo da média
soma=0
for t in "${tempos[@]}"; do
    soma=$(echo "$soma + $t" | bc -l)
done

media=$(echo "$soma / $RUNS" | bc -l)

# cálculo do desvio padrão
soma_diff=0
for t in "${tempos[@]}"; do
    diff=$(echo "$t - $media" | bc -l)
    soma_diff=$(echo "$soma_diff + ($diff * $diff)" | bc -l)
done

variancia=$(echo "$soma_diff / $RUNS" | bc -l)
desvio=$(echo "sqrt($variancia)" | bc -l)

echo ""
echo "=============================="
echo "Media: $media s"
echo "Desvio padrão: $desvio s"
echo "=============================="

