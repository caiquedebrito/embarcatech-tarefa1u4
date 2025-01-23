#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
  
// Definições de pinos e constantes
#define PIN_LED_R 13
#define PIN_LED_B 12
#define PIN_LED_G 11
#define PIN_BUZZER 21
#define FREQ_PADRAO 350 // Frequência do buzzer em Hz
#define DEBOUNCE_DELAY 200 // Delay para debounce de teclas em milissegundos

// Variável global para armazenar o slice do PWM
int slice_buzzer;

// Mapeamento do teclado matricial
const uint8_t colunas[4] = {4, 3, 2, 1}; // Pinos das colunas
const uint8_t linhas[4] = {5, 6, 7, 8};  // Pinos das linhas
const char teclado[4][4] = 
{
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Protótipos das funções
void configurar_leds();
void configurar_buzzer(uint freq);
void configurar_teclado();
void turn_on_led(bool red, bool blue, bool green);
void liga_buzzer();
void desliga_buzzer();
char leitura_teclado(); 

int main() 
{
    stdio_init_all();

    // Configurações iniciais
    configurar_leds();
    configurar_buzzer(FREQ_PADRAO);
    configurar_teclado();

    char tecla_atual = 'n'; // Tecla atualmente detectada
    char tecla_ultima = 'n'; // Última tecla processada

    while (true) 
    {    
        tecla_atual = leitura_teclado(); // Verifica qual tecla foi pressionada

        // Se uma tecla for pressionada
        if (tecla_atual != 'n') {
            if (tecla_atual != tecla_ultima) { // Se for uma tecla diferente da última processada
                tecla_ultima = tecla_atual; // Atualiza a última tecla processada
                switch (tecla_atual) {
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
                        liga_buzzer(); // Ativa o buzzer
                        break;
                    default:
                        break;
                }
            }
        } else {
            // Se nenhuma tecla for pressionada, desliga os dispositivos
            if (tecla_ultima != 'n') {
                turn_on_led(0, 0, 0); // Desliga LEDs
                desliga_buzzer(); // Desativa o buzzer
                tecla_ultima = 'n'; // Reseta a última tecla processada
            }
        }

        // Se uma tecla estiver pressionada, mantém a ação associada à tecla enquanto ela for pressionada
        while (tecla_atual != 'n') {
            tecla_atual = leitura_teclado(); // Atualiza a tecla pressionada
            sleep_ms(50); // Delay para evitar leitura muito rápida
        }

        sleep_ms(50); // Delay para evitar leituras repetidas e otimizar o uso da CPU
    }

    return 0;
}

void configurar_leds() {
    gpio_init(PIN_LED_R);
    gpio_set_dir(PIN_LED_R, GPIO_OUT);
    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    gpio_init(PIN_LED_G);
    gpio_set_dir(PIN_LED_G, GPIO_OUT);
}

// Função para configurar o buzzer
void configurar_buzzer(uint freq) {
    gpio_set_function(PIN_BUZZER, GPIO_FUNC_PWM);
    slice_buzzer = pwm_gpio_to_slice_num(PIN_BUZZER);

    uint32_t clk_sys = clock_get_hz(clk_sys); // Frequência do sistema (125 MHz por padrão)
    uint16_t wrap = clk_sys / freq - 1; // Calcula o valor de wrap

    pwm_set_wrap(slice_buzzer, wrap); // Configura o valor de wrap
    pwm_set_gpio_level(PIN_BUZZER, wrap / 2); // Define o duty cycle (50%)
    pwm_set_enabled(slice_buzzer, false); // Inicialmente desligado
}

// Função para configurar o teclado
void configurar_teclado() {
    for (int i = 0; i < 4; i++) {
        gpio_init(colunas[i]);
        gpio_set_dir(colunas[i], GPIO_OUT);
        gpio_put(colunas[i], 1); // Inicializa as colunas com 1 (desligado)
    }

    for (int i = 0; i < 4; i++) {
        gpio_init(linhas[i]);
        gpio_set_dir(linhas[i], GPIO_IN);
        gpio_pull_up(linhas[i]); // Habilita pull-up para evitar leituras erradas
    }
}

// Função para acionar os LEDs
void turn_on_led(bool red, bool blue, bool green) {
    gpio_put(PIN_LED_R, red);
    gpio_put(PIN_LED_B, blue);
    gpio_put(PIN_LED_G, green);
}

// Função para ligar o buzzer
void liga_buzzer() {
    pwm_set_enabled(slice_buzzer, true); // Ativa o PWM no slice do buzzer
}

// Função para desligar o buzzer
void desliga_buzzer() {
    pwm_set_enabled(slice_buzzer, false); // Desativa o PWM no slice do buzzer
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