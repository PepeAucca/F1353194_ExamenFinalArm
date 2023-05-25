#include <stdint.h>
#include "incFiles/stm32f103x6.h"
#include "motorDriver/motorDriver.h"
#include "serialUart/serialUart.h"

int main(void)
{
	//Inicializar el módulo UART y el módulo de control de motores
    inicializarUart();
    iniciarModulo();

		
	while(1);
	{
	// Verificar si existe un carácter recibido
        if (existeCaracter())
        {
            // Leer el carácter recibido
            uint8_t caracter = leerCaracter();

            // Ejecutar acciones según el carácter recibido
            switch (caracter)
            {
                case 'a':
                    avanzar(1);
                    break;
                case 'w':
                    avanzar(2);
                    break;
                case 'b':
                    retroceder(1);
                    break;
                case 's':
                    retroceder(2);
                    break;
                case 'q':
                    girarDerecha();
                    break;
                case 'e':
                    girarIzquierda();
                    break;
                default:
                    // No hacer nada para otros caracteres
                    break;
            }
        }
    }
    return 0;

}