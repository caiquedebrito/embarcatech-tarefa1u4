#include "pico/stdlib.h"
#include <stdio.h>

// Definição dos pinos do teclado matricial
const uint8_t colunas[4] = {4, 3, 2, 1}; // Pinos conectados às colunas do teclado
const uint8_t linhas[4] = {5, 6, 7, 8};  // Pinos conectados às linhas do teclado

// Mapeamento das teclas para os valores correspondentes
const char teclado[4][4] = 
{
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char leitura_teclado();           // Função que realiza a leitura do teclado matricial

int main() 
{
    // Inicializa a comunicação UART para exibição de mensagens na serial
    stdio_init_all();

    // Configura os pinos das colunas como saídas (para controlar as colunas do teclado)
    for (int i = 0; i < 4; i++)
    {
        gpio_init(colunas[i]);
        gpio_set_dir(colunas[i], GPIO_OUT);   // Configura as colunas como saídas
        gpio_put(colunas[i], 1);               // Define o valor inicial como 1 (desligado)
    }

    // Configura os pinos das linhas como entradas (para detectar as teclas pressionadas)
    for (int i = 0; i < 4; i++)
    {
        gpio_init(linhas[i]);
        gpio_set_dir(linhas[i], GPIO_IN);      // Configura as linhas como entradas
        gpio_pull_up(linhas[i]);                // Habilita o pull-up para as linhas (evita leitura errada)
    }

    while (true) 
    {
        // Lê a tecla pressionada e armazena o valor retornado
        char tecla = leitura_teclado();
        
        if (tecla != 'n') // Se uma tecla for pressionada, exibe o valor na serial
        {
            printf("Tecla pressionada: %c\n", tecla);
        }

        sleep_ms(200); // Aguarda 200ms para evitar leituras erradas (debounce)
    }

    return 0;
}

// Função para ler o teclado matricial (retorna o caractere da tecla pressionada)
char leitura_teclado()
{
    char numero = 'n'; // Inicializa como 'n' para indicar que nenhuma tecla foi pressionada

    // Percorre todas as colunas
    for (int coluna = 0; coluna < 4; coluna++)
    {
        // Ativa a coluna atual (coloca o pino da coluna como 0)
        gpio_put(colunas[coluna], 0);

        // Verifica se alguma linha foi pressionada
        for (int linha = 0; linha < 4; linha++)
        {
            // Se a linha estiver em 0, significa que a tecla foi pressionada
            if (gpio_get(linhas[linha]) == 0)
            {
                numero = teclado[3 - linha][coluna]; // Mapeia o valor da tecla pressionada com a inversão da linha
                // Aguarda a tecla ser liberada (debounce)
                while (gpio_get(linhas[linha]) == 0)
                {
                    sleep_ms(10); // Atraso para evitar múltiplas leituras
                }
                break; // Sai do laço de linhas quando uma tecla é detectada
            }
        }

        // Desativa a coluna atual (coloca o pino da coluna como 1)
        gpio_put(colunas[coluna], 1);

        if (numero != 'n') // Se uma tecla foi pressionada, sai do laço das colunas
        {
            break;
        }
    }

    return numero; // Retorna o valor da tecla pressionada
}
