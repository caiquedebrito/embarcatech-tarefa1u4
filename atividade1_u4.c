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

// Função para ler o teclado e manter LEDs/buzzer ativos até outra tecla ser pressionada
char leitura_teclado() {
    static char ultima_tecla = 'n'; // Armazena a última tecla pressionada
    char tecla = 'n';              // Inicializa com 'n' (nenhuma tecla pressionada)

    for (int coluna = 0; coluna < 4; coluna++) {
        gpio_put(colunas[coluna], 0); // Ativa a coluna (coloca como 0)

        for (int linha = 0; linha < 4; linha++) {
            if (gpio_get(linhas[linha]) == 0) { // Se uma tecla foi pressionada
                tecla = teclado[3 - linha][coluna]; // Mapeia a tecla pressionada

                // Aguarda até que a tecla seja liberada para evitar leituras repetidas
                while (gpio_get(linhas[linha]) == 0) {
                    sleep_ms(10); // Delay para evitar leitura muito rápida
                }

                break; // Sai do loop da linha
            }
        }

        gpio_put(colunas[coluna], 1); // Desativa a coluna (coloca como 1)

        if (tecla != 'n') break; // Sai do laço se uma tecla foi pressionada
    }

    // Atualiza o estado apenas se uma nova tecla foi pressionada
    if (tecla != 'n' && tecla != ultima_tecla) {
        ultima_tecla = tecla; // Atualiza a última tecla pressionada

        // Define a ação para a nova tecla pressionada
        switch (tecla) {
            case 'A':
                turn_on_led(1, 0, 0); // Liga LED vermelho
                break;
            case 'B':
                turn_on_led(0, 1, 0); // Liga LED verde
                break;
            case 'C':
                turn_on_led(0, 0, 1); // Liga LED azul
                break;
            case 'D':
                turn_on_led(1, 1, 1); // Liga todos os LEDs
                break;
            case '#':
                pwm_set_enabled(pwm_gpio_to_slice_num(PIN_BUZZER), true); // Ativa o buzzer
                break;
            default:
                turn_on_led(0, 0, 0); // Desliga LEDs como fallback
                pwm_set_enabled(pwm_gpio_to_slice_num(PIN_BUZZER), false); // Desativa o buzzer
                break;
        }
    }

    return tecla; // Retorna a tecla pressionada ou 'n' se nenhuma tecla foi detectada
}