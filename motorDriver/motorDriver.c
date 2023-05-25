#include "motorDriver.h"

#define RCC_BASE 0x40021000
#define GPIOA_BASE 0x40010800
#define GPIOB_BASE 0x40010C00
#define GPIOC_BASE 0x40011000
#define TIM4_BASE 0x40000800

#define RCC_APB2ENR (*((unsigned int*) (RCC_BASE + 0x18)))
#define RCC_APB1ENR (*((unsigned int*) (RCC_BASE + 0x1C)))
#define GPIOA_CRL (*((unsigned int*) (GPIOA_BASE + 0x00)))
#define GPIOA_CRH (*((unsigned int*) (GPIOA_BASE + 0x04)))
#define GPIOA_ODR (*((unsigned int*) (GPIOA_BASE + 0x0C)))
#define GPIOB_CRH (*((unsigned int*) (GPIOB_BASE + 0x04)))
#define GPIOC_CRH (*((unsigned int*) (GPIOB_BASE + 0x04)))
#define GPIOC_ODR (*((unsigned int*) (GPIOB_BASE + 0x0C)))

#define TIM4_PSC (*((unsigned int*) (TIM4_BASE + 0x28)))
#define TIM4_ARR (*((unsigned int*) (TIM4_BASE + 0x2C)))
#define TIM4_CCR3 (*((unsigned int*) (TIM4_BASE + 0x3C)))
#define TIM4_CCR4 (*((unsigned int*) (TIM4_BASE + 0x40)))
#define TIM4_CCMR2 (*((unsigned int*) (TIM4_BASE + 0x1C)))
#define TIM4_BDTR (*((unsigned int*) (TIM4_BASE + 0x44)))
#define TIM4_CCER (*((unsigned int*) (TIM4_BASE + 0x20)))
#define TIM4_CR1 (*((unsigned int*) (TIM4_BASE + 0x00)))

void iniciarModulo(void)
{
    // Habilitar la señal de reloj para el puerto GPIOA, GPIOC, UART4 y funciones alternas
    RCC->APB2ENR |= (1 << 2);   // Habilitar reloj para GPIOA
    RCC->APB2ENR |= (1 << 4);   // Habilitar reloj para GPIOB
    RCC->APB2ENR |= (1 << 6);   // Habilitar reloj para GPIOC
    RCC->APB1ENR |= (1 << 19);  // Habilitar reloj para TIM4

    // Configurar los puertos PA4, PA5, PA6, PA7 y PC13 como salidas digitales en modo push-pull y bajo (LOW)
    GPIOA->CRL &= ~((0xF << 16) | (0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOA->CRL |= (0x3 << 16) | (0x3 << 20) | (0x3 << 24) | (0x3 << 28);
    GPIOA->ODR &= ~((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));

    // Configurar el puerto PC13 como salida digital en modo push-pull y bajo (LOW)
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x3 << 20);
    GPIOC->ODR &= ~(1 << 13);

    // Configurar los puertos PB8 y PB9 como salidas digitales de funciones alternas
    GPIOB->CRH &= ~((0xF << 0) | (0xF << 4));
    GPIOB->CRH |= (0xB << 0) | (0xB << 4);

    // Configurar el timer TIM4
    TIM4->CCMR2 = ((0x68 << 8) | (0x68 << 0));  // Configurar el modo de captura/compare para los canales 3 y 4
    TIM4->BDTR |= (1 << 15);                    // Habilitar el control del break para el modo PWM
    TIM4->CCER |= (1 << 12) | (1 << 8);         // Habilitar la salida PWM para los canales 3 y 4
    TIM4->PSC = 7;                              // Configurar el prescaler
    TIM4->ARR = 7199;                           // Configurar el valor del auto-reload para generar una onda PWM de 100Hz
    TIM4->CCR4 = 0;                             // Inicializar el ciclo de trabajo del canal 4 en 0%
    TIM4->CCR3 = 0;                             // Inicializar el ciclo de trabajo del canal 3 en 0%
    TIM4->CR1 |= (1 << 7) | (1 << 0);           // Habilitar el temporizador

    // Establecer los puertos PB8 y PB9 en 0% del ciclo de trabajo (0 Volt)
    TIM4->CCR4 = 0;
    TIM4->CCR3 = 0;
}

void avanzar(uint8_t vel)
{
    // Colocar los puertos PA5 y PA6 en alta
    GPIOA->ODR |= (1 << 5) | (1 << 6);

    // Colocar los puertos PA4 y PA7 en baja
    GPIOA->ODR &= ~((1 << 4) | (1 << 7));

    // Configurar el ciclo de trabajo de las ondas PWM para PB8 y PB9
    if (vel == 1)
    {
        TIM4->CCR4 = 3599;  // 50% del ciclo de trabajo (1/2 de 7199)
        TIM4->CCR3 = 3599;  // 50% del ciclo de trabajo (1/2 de 7199)
    }
    else if (vel == 2)
    {
        TIM4->CCR4 = 7199;  // 100% del ciclo de trabajo (7199/7199)
        TIM4->CCR3 = 7199;  // 100% del ciclo de trabajo (7199/7199)
    }
}

void retroceder(uint8_t vel)
{
    // Colocar los puertos PA4 y PA7 en alta
    GPIOA->ODR |= (1 << 4) | (1 << 7);

    // Colocar los puertos PA5 y PA6 en baja
    GPIOA->ODR &= ~((1 << 5) | (1 << 6));

    // Configurar el ciclo de trabajo de las ondas PWM para PB8 y PB9
    if (vel == 1)
    {
        TIM4->CCR4 = 3599;  // 50% del ciclo de trabajo (1/2 de 7199)
        TIM4->CCR3 = 3599;  // 50% del ciclo de trabajo (1/2 de 7199)
    }
    else if (vel == 2)
    {
        TIM4->CCR4 = 7199;  // 100% del ciclo de trabajo (7199/7199)
        TIM4->CCR3 = 7199;  // 100% del ciclo de trabajo (7199/7199)
    }
}

void girarDerecha(void)
{
    // Colocar el puerto PA6 en alta
    GPIOA->ODR |= (1 << 6);

    // Colocar los puertos PA4, PA5 y PA7 en baja
    GPIOA->ODR &= ~((1 << 4) | (1 << 5) | (1 << 7));

    // Configurar el ciclo de trabajo de las ondas PWM para PB8 y PB9
    TIM4->CCR4 = 7199;  // 100% del ciclo de trabajo (7199/7199)
    TIM4->CCR3 = 0;     // 0% del ciclo de trabajo (0/7199)
}

void girarIzquierda(void)
{
    // Colocar el puerto PA5 en alta
    GPIOA->ODR |= (1 << 5);

    // Colocar los puertos PA4, PA6 y PA7 en baja
    GPIOA->ODR &= ~((1 << 4) | (1 << 6) | (1 << 7));

    // Configurar el ciclo de trabajo de las ondas PWM para PB8 y PB9
    TIM4->CCR4 = 0;     // 0% del ciclo de trabajo (0/7199)
    TIM4->CCR3 = 7199;  // 100% del ciclo de trabajo (7199/7199)
}



