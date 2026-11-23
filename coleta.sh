
(
set -e

pasta="resultados/$(date +%Y%m%d-%H%M%S)"
mkdir -p "$pasta"

echo "aplicacao,entrada,modo,threads,repeticao,tempo_s" > "$pasta/tempos.csv"

registrar() {
    local app=$1 entrada=$2 modo=$3 p=$4 rep=$5 saida tempo
    local comando=("./$app" "$modo")

    [[ $app != contadores ]] && comando+=("$entrada")
    [[ $p -ne 1 ]] && comando+=("$p")

    saida=$("${comando[@]}") || return 1
    tempo=$(awk '/^Tempo de execucao: / {print $4}' <<< "$saida")

    [[ -n $tempo ]] || return 1

    if [[ $app == contadores ]] &&
       ! grep -qx 'Contador: 100000000' <<< "$saida"; then
        echo "Erro no contador!"
        return 1
    fi

    echo "$app,$entrada,$modo,$p,$rep,$tempo" >> "$pasta/tempos.csv"
    echo "$app | $entrada | $modo | $p threads | repeticao $rep | $tempo s"
}

for rep in {1..5}; do
    for app in mult_mat imagens; do
        for n in 256 512 1024; do
            registrar "$app" "$n" S 1 "$rep"
            for p in 2 4 8; do
                registrar "$app" "$n" P "$p" "$rep"
            done
        done
    done

    registrar contadores 100000000 S 1 "$rep"

    for modo in A B; do
        for p in 2 4 8; do
            registrar contadores 100000000 "$modo" "$p" "$rep"
        done
    done
done

wc -l "$pasta/tempos.csv"
)
