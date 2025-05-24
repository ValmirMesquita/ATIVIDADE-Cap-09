/*
    ***************************************************************************************
    Projeto: tempo ciclico DMA
    Autor: Valmir Linhares de Sousa de Mesquita
    Data: 24 de Maio de 2015
    ****************************************************************************************
    Este código é um programa para a placa Raspberry Pi Pico (RP2040) que realiza 
    monitoramento de temperatura e controle de LEDs RGB, com estrutura modular para 
    incluir futuras tarefas como controle de NeoPixels e display OLED.
    ------------------------------------------------------------------------------------
    Resumo do que o código faz:
    Inicializa o sistema, ADC e pinos de LEDs RGB.
    Usa um timer que dispara a cada 100ms para:
    Ativar a leitura da temperatura interna do chip.
    Alternar entre os LEDs RGB (vermelho, verde e azul) ciclicamente..
    A cada leitura, a temperatura é impressa via USB serial (no terminal do computador).
    O LED aceso muda em ciclos (ex: R → G → B) a cada leitura.
    O loop principal roda continuamente, verificando se é hora de executar as tarefas. 
    Link GitHub: https://github.com/ValmirMesquita/ATIVIDADE-Cap-09/tree/main/tempoCicloDMA
    
 */



#include <stdio.h>                     // Biblioteca padrão para entrada e saída (printf, etc.)
#include "pico/stdlib.h"              // Biblioteca da Raspberry Pi Pico para funcionalidades básicas
#include "hardware/adc.h"             // Biblioteca para controle do ADC (conversor analógico-digital)
#include "hardware/gpio.h"            // Biblioteca para controle das GPIOs

// Definições para os pinos dos LEDs RGB conectados nas GPIOs 12, 13 e 14
#define LED_PIN_R 12
#define LED_PIN_G 13
#define LED_PIN_B 14

// Variável usada como flag para ativar a Tarefa 1 (leitura de temperatura)
volatile bool task1_flag = false;

// Contador de ciclos do sistema, incrementado a cada callback do timer
uint32_t task_counter = 0;

// Função para ler a temperatura interna do chip RP2040
float read_internal_temp() {
    adc_select_input(4);  // Seleciona o canal 4, correspondente ao sensor de temperatura interno
    uint16_t raw = adc_read();  // Lê o valor bruto do ADC (0 a 4095)
    
    // Converte o valor bruto em tensão (0 a 3.3V)
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = raw * conversion_factor;
    
    // Converte a tensão para temperatura em graus Celsius
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

// Função de callback chamada automaticamente a cada 100ms pelo timer
bool repeating_timer_callback(repeating_timer_t *rt) {
    task1_flag = true;     // Sinaliza que a Tarefa 1 deve ser executada
    task_counter++;        // Incrementa o contador global de tarefas
    return true;           // Retorna true para manter o timer ativo
}

// Tarefa 1: lê a temperatura e imprime no terminal serial
void task1() {
    float temp = read_internal_temp();                    // Lê a temperatura
    printf("Temperatura: %.2f °C\n", temp);               // Exibe no terminal serial
    task1_flag = false;                                   // Reseta a flag até o próximo timer
}

// Tarefa 2: alterna os LEDs RGB com base no contador de tarefas
void task2() {
    switch (task_counter % 3) {  // Alterna entre 3 estados: R, G ou B
        case 0:
            gpio_put(LED_PIN_R, 1);  // Acende o LED vermelho
            gpio_put(LED_PIN_G, 0);
            gpio_put(LED_PIN_B, 0);
            break;
        case 1:
            gpio_put(LED_PIN_R, 0);
            gpio_put(LED_PIN_G, 1);  // Acende o LED verde
            gpio_put(LED_PIN_B, 0);
            break;
        case 2:
            gpio_put(LED_PIN_R, 0);
            gpio_put(LED_PIN_G, 0);
            gpio_put(LED_PIN_B, 1);  // Acende o LED azul
            break;
    }
}

// Tarefa 3: placeholder para controle da fita NeoPixel
void task3() {
    if (task_counter % 5 == 0) {
        // Aqui você pode inserir código para atualizar os LEDs NeoPixel
    }
}

// Tarefa 4: placeholder para controle de display OLED
void task4() {
    if (task_counter % 10 == 0) {
        // Aqui você pode inserir código para atualizar o display OLED
    }
}

// Inicializa os pinos GPIO dos LEDs RGB como saídas e os apaga inicialmente
void init_rgb_leds() {
    gpio_init(LED_PIN_R);
    gpio_init(LED_PIN_G);
    gpio_init(LED_PIN_B);
    
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    gpio_put(LED_PIN_R, 0);
    gpio_put(LED_PIN_G, 0);
    gpio_put(LED_PIN_B, 0);
}

// Função principal
int main() {
    stdio_init_all();      // Inicializa a saída padrão (USB serial)
    sleep_ms(500);         // Espera a conexão serial estabilizar
    printf("Sistema iniciado.\n");

    adc_init();                    // Inicializa o ADC
    adc_set_temp_sensor_enabled(true);  // Ativa o sensor de temperatura interno

    init_rgb_leds();              // Inicializa os LEDs RGB

    // Cria e inicia um timer que chama a função de callback a cada 100ms
    repeating_timer_t timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);

    // Loop principal
    while (true) {
        if (task1_flag) {  // Executa as tarefas apenas quando o timer ativa a flag
            task1();       // Leitura e exibição da temperatura
            task2();       // Alterna os LEDs RGB
            task3();       // Placeholder para NeoPixel
            task4();       // Placeholder para OLED
        }

        sleep_ms(1000);  // Pausa o loop principal para reduzir o uso de CPU
    }

    return 0;  // Nunca alcançado
}
