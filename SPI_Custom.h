#ifndef SPI_CUSTOM_H
#define SPI_CUSTOM_H

#include "stm32f10x.h"
#include "delay.h"

#define SPIDelay        MicroWait(1);//1us
#define delay_us(x)     MicroWait(x)
#define MicroWait(x)		DelayUs(x)

#define IOPORT_DIR_OUTPUT			GPIO_Mode_Out_PP
#define IOPORT_DIR_INPUT			GPIO_Mode_IN_FLOATING

#define HIGH_OUT				1
#define LOW_OUT					0

// Define SPI communication mode
typedef enum 
{
  Mode_1,   /* Clock Polarity is 0 and Clock Phase is 0 */
  Mode_2,   /* Clock Polarity is 0 and Clock Phase is 1 */
  Mode_3,   /* Clock Polarity is 1 and Clock Phase is 0 */
  Mode_4,   /* Clock Polarity is 1 and Clock Phase is 1 */
}SPIMode;

// Define SPI type
typedef enum
{
  SPIMaster,
  SPISlave,
}SPIType;

typedef struct
{
//  unsigned int port;
	GPIO_TypeDef *port;
  unsigned int pin;
}SPI_IO_t;

// Define SPI attribute
typedef struct
{
  SPI_IO_t nCS;
  SPI_IO_t nSCLK;
  SPI_IO_t nMOSI;
  SPI_IO_t nMISO;
  SPIMode SpiMode;
  SPIType SpiType;
}Spi_Parameter_t;


// Function prototypes
void SPISimulateInit( Spi_Parameter_t *pSpi );

void SPISimulate_CSIsEnable( Spi_Parameter_t *pSpi, int IsEnable );

void SPISimulate_WriteByte( Spi_Parameter_t *pSpi, unsigned char nByte );

void SPISimulateWrite( Spi_Parameter_t *pSpi, unsigned char *pdata, int len );

unsigned char SPISimulate_ReadByte( Spi_Parameter_t *pSpi );

void SPISimulateRead( Spi_Parameter_t *pSpi, unsigned char *pdata, int len );

#endif

