Microcontroladores 2022
SE/3 Seção de Engenharia Elétrica

GABRIEL HENRIQUE MARCELLINO SILVA 19 de mai.
# Acionamento de periféricos da STM32f407 controlada via internet

## Motivação
Aplicações de conectividade e automação têm tomado cada vez mais espaço no nosso cotidiano. Um dos veículos desse desenvolvimento é o conceito de IoT, que trata da conexão de dispositivos à rede local para que possam se comunicar com outros elementos da rede.
Baseado nesse conceito, a proposta desse trabalho é fazer a conexão de um smartphone com uma STM32 através de uma rede local e acionar alguns periféricos da placa.

## Conceito
Utilizando um módulo MKR1000 para estabelecer uma conexão wi-fi, vamos interfaceá-lo com um módulo STM32F407 para controlar alguns dos seus periféricos.
