# Projeto: Controle de LEDs e Display com Joystick

## Descrição
Este projeto utiliza um Raspberry Pi Pico, um joystick analógico, três LEDs RGB, e um display OLED SSD1306 para controlar graficamente um quadrado no display e modular o brilho dos LEDs.

Os LEDs RGB são controlados em intensidade com PWM, de acordo com os valores lidos pelo joystick. Além disso, o quadrado na tela se move conforme a posição do joystick. Ao pressionar botões do joystick, altera-se o modo de operação, exibindo um quadrado maior que limita a área de movimento do quadrado menor.

## Funcionalidades
- Controle PWM dos LEDs vermelho e azul com base nos eixos do joystick.
- Controle da posição de um quadrado no display OLED.
- Botão do joystick ativa/desativa o LED verde (e sinaliza modificando a borda a cada acionamento).
- Botão adicional ativa/desativa o controle PWM dos LEDs.

## Conexões
| Componente      | Pino Pico |
|-----------------|-----------|
| LED Verde       | GP11      |
| LED Azul        | GP12 (PWM)|
| LED Vermelho    | GP13 (PWM)|
| Botão Joystick | GP22      |
| Botão A        | GP5       |
| Eixo X Joystick | GP26 (ADC)|
| Eixo Y Joystick | GP27 (ADC)|
| SDA (Display)   | GP14      |
| SCL (Display)   | GP15      |

## Funcionamento
1. Ao iniciar, o joystick controla o movimento do quadrado e a intensidade dos LEDs azul e vermelho.
2. Pressionar o botão do joystick ativa/desativa o LED verde (e aciona a borda no display).
3. Pressionar o botão A ativa/desativa o controle PWM dos LEDs. Quando desativado, os LEDs são desligados.

## Compilação e Execução
1. Faça um git clone no link da página.
2. Com o Pico SDK no VS Code, faça a importação da pasta onde foi clonado o repositório.
3. Compile o código, e com a placa no modo bootsel, mova o arquivo .uf2 para a mesma.

## Observações
- Alguns valores utilizados para mapeamento e configuração foram coletados empiricamente, isto é, 
por se tratar de componentes analógicos, os valores do joystick em "standby", os valores máximos
e mínimos foram testados e anotados para um funcionamento adequado.

## Autores
Victor Gabriel Guimarães Lopes

## Vídeo Demonstrativo
[Inserir Link do Vídeo Aqui]

