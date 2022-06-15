Microcontroladores 2022
SE/3 Seção de Engenharia Elétrica

GABRIEL HENRIQUE MARCELLINO SILVA 19 de mai.
# Acionamento de periféricos da STM32f407 controlada via internet

## Motivação
Aplicações de conectividade e automação têm tomado cada vez mais espaço no nosso cotidiano. Um dos veículos desse desenvolvimento é o conceito de IoT, que trata da conexão de dispositivos à rede local para que possam se comunicar com outros elementos da rede.
Baseado nesse conceito, a proposta desse trabalho é fazer a conexão de um smartphone com uma STM32 através de uma rede local e acionar alguns periféricos da placa.

## Conceito
Utilizando um módulo ESP32 para estabelecer uma conexão wi-fi, vamos interfaceá-lo com um módulo STM32F407 via UART para gerar uma função de onda e controlar seus periféricos via PWM.

## Circuito
Com ambas ESP32 e STM32 alimentadas, conectamos os pinos PA2 e PA3 aos pinos TX0 e RX0 da ESP32, para fazermos a conexão UART.

## ESP32
No dispositivo, inicializamos todos os periféricos e depois esperamos por uma conexão wifi no SSID e senhas explicitados.
Depois disso, o dispositivo fica no aguardo de uma chamada HTTP GET contendo a configuração dos periféricos para STM32.
Ao receber a string como parâmetro de URL, a string é encaminhada à STM32.

## HTTP
A chamada HTTP tem o seguinte formato:
http://192.168.158.231/?cs=Y-0500-0500-H@O-1000-0000-P@R-0500-0375-R@B-0500-0125-C
Primeiramente o endereço IP da ESP32 na rede local, depois o parâmetro "cs" significando configuration string.

## Configuration String
A configuration string tem o seguinte formato:
**Y-0500-0500-H@O-1000-0000-P@R-0500-0375-R@B-0500-0125-C**
O comando consiste de uma string UTF8 composta por até 4 intruções para periféricos separados por "@", totalizando 55 caracteres.
A um grupo contido entre "@" cahama-se configuration block.

### Configuration Block
Um bloco de configuração tem o seguinte formato:
**Y-0500-0500-H**
Um bloco de configuração tem seus parâmetros separados por "-", sendo cada um deles:
1º - um char maiúculo contendo a inicial da cor que se deseja acender (Y - yellow - amarelo, O - orange - laranja, R - red - vermelho, B - blue - azul);
2º - um inteiro pad 0 de 4 dígitos representando a quantidade de amostras na função de onda (tendo em vista que cada iteração tem um delay de 5ms, um número 0100 representaria uma onda de período 500ms por exemplo);
3º - um inteiro pad 0 de 4 dígitos representando o atraso de amostras na função de onda (ângulo de fase);
4º - um char maiúculo contendo a inicial da função de onda escolhida (C - Cosinusoid, L - Low, H - High, R - Ramp, P - Parabola, S - Square);

## STM32
Na inicialização, a STM32 configura e inicia os periféricos. Depois faz o parsing da sua string default que representa 4 ondas quadradas defasadas de  um quarto de onda e passa como parâmetro para as estruturas que controlam o funcionamento dos LEDS. Por fim, ela configura a interupção que permite a troca da configuração das ondas e entra no loop permanente em que fica setando os valores de onda no CCR dos canais de PWM do TIM4.

## Interrupção
Quando a STM recebe uma string de configuração, ela aciona uma interrupção que faz o parsing da string e ajusta os parâmetros das funções de onda. Depois disso ela reseta a espera por interrupção e entra no loop contínuo até que uma nova interrupção ocorra.
