# Define o comando para teste
$command = ".\build\concatenador"

# Define o tamanho máximo inicial
$maxLength = 32767  # Valor inicial, considerando o limite do Windows
$step = 100          # Incremento/decremento para testar limites
$currentLength = 1   # Tamanho inicial do argumento

# Loop para testar diferentes tamanhos
while ($currentLength -le $maxLength) {
    try {
        # Gera um argumento com o tamanho atual
        $argument = "a" * $currentLength

        # Monta e executa o comando
        $cmd = "$command $argument"
        Invoke-Expression $cmd

        # Exibe o tamanho testado
        Write-Host "Testado com $currentLength caracteres: Sucesso"

        # Incrementa o tamanho
        $currentLength += $step
    }
    catch {
        # Caso o comando falhe, exibe o tamanho limite
        Write-Host "Falha ao executar com $currentLength caracteres"
        Write-Host "Limite máximo suportado: $($currentLength - $step) caracteres"
        break
    }
}

Write-Host "Teste concluído."
