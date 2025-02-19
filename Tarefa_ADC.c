#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define LED_G 11
#define LED_B 12
#define LED_R 13
#define BTN_JOY 22
#define BTN_A 5
#define JOY_X 26
#define JOY_Y 27
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SQUARE_SIZE 8
#define JOY_X_MIN 22
#define JOY_X_MAX 4090
#define JOY_Y_MIN 22
#define JOY_Y_MAX 4090


// Estado dos LEDs e variáveis globais
bool cor = true;
bool led_pwm_enabled = true;
bool led_green_state = false;
int square_x = SCREEN_WIDTH / 2 - SQUARE_SIZE / 2;
int square_y = SCREEN_HEIGHT / 2 - SQUARE_SIZE / 2;
uint joy_x, joy_y;
ssd1306_t ssd; // Inicializa a estrutura do display
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Inicializa os pinos GPIO
void initGPIO() {
    uint pwm_leds[] = {LED_R, LED_B};
    for (int i = 0; i < 2; i++) {
        gpio_set_function(pwm_leds[i], GPIO_FUNC_PWM);
    }

    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    //inicialização do botão A
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, false);
    gpio_pull_up(BTN_A);
    //inicialização do botão do joystick
    gpio_init(BTN_JOY);
    gpio_set_dir(BTN_JOY, false);
    gpio_pull_up(BTN_JOY);
}

// Configura o display SSD1306
void SSD1306_setup(){
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_init(&ssd, SCREEN_WIDTH, SCREEN_HEIGHT, false, 0x3C, I2C_PORT);
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Mapeia o quadrado do display
int map(int val, int in_min, int in_max, int out_min, int out_max) {
    if (val < in_min) val = in_min;
    if (val > in_max) val = in_max;
    return out_min + (val - in_min) * (out_max - out_min) / (in_max - in_min);
}

// Atualiza o display
void update_display() {
    ssd1306_fill(&ssd, !cor);
    if (led_green_state) {
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
    }
    ssd1306_rect(&ssd, square_y, square_x, SQUARE_SIZE, SQUARE_SIZE, cor, !cor);
    ssd1306_send_data(&ssd);
}

// Faz a leitura do Joystick
void joystick_read() {
    adc_select_input(0);
    joy_y = adc_read();
    adc_select_input(1);
    joy_x = adc_read();

    if (led_green_state) {
        square_x = map(joy_x, JOY_X_MIN, JOY_X_MAX, 5, SCREEN_WIDTH - SQUARE_SIZE - 5);
        square_y = map(joy_y, JOY_Y_MIN, JOY_Y_MAX, SCREEN_HEIGHT - SQUARE_SIZE - 5, 5);
    }else {
        square_x = map(joy_x, JOY_X_MIN, JOY_X_MAX, 0, SCREEN_WIDTH - SQUARE_SIZE);
        square_y = map(joy_y, JOY_Y_MIN, JOY_Y_MAX, SCREEN_HEIGHT - SQUARE_SIZE, 0);
    }



    if (led_pwm_enabled) {
        pwm_set_gpio_level(LED_R, abs((int)joy_x - 2147) * 65535 / 2147);
        pwm_set_gpio_level(LED_B, abs((int)joy_y - 1890) * 65535 / 1890);
    }

    update_display();
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 200000) { // 200ms de debounce
        last_time = current_time;
        if (gpio == BTN_JOY) {
            led_green_state = !led_green_state;
            gpio_put(LED_G, led_green_state);
        } else if (gpio == BTN_A) {
            led_pwm_enabled = !led_pwm_enabled;
            if (!led_pwm_enabled) {
                pwm_set_gpio_level(LED_R, 0);
                pwm_set_gpio_level(LED_B, 0);
            }
        }
    }
}

int main() {
    stdio_init_all();
    initGPIO();
    i2c_init(I2C_PORT, 400 * 1000); //Inicializa o i2c em 400kHz
    SSD1306_setup();

    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);
    uint slice_r = pwm_gpio_to_slice_num(LED_R);
    uint slice_b = pwm_gpio_to_slice_num(LED_B);
    pwm_set_enabled(slice_r, true);
    pwm_set_enabled(slice_b, true);
    pwm_set_gpio_level(LED_R, 0);
    pwm_set_gpio_level(LED_B, 0);

    gpio_set_irq_enabled_with_callback(BTN_JOY, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    adc_init();
    adc_gpio_init(JOY_X);
    adc_gpio_init(JOY_Y);
    adc_select_input(0);  

    while (1) {
        joystick_read();
        sleep_ms(100);
    }
    
}
