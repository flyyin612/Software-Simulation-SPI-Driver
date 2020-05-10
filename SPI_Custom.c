#include "SPI_Custom.h"

/**************************SPI模式配置*********************************
Mode1：CPOL=0，CPHA=0
Mode2：CPOL=0，CPHA=1
Mode3：CPOL=1，CPHA=0
Mode4：CPOL=1，CPHA=1

时钟极性CPOL是用来配置SCLK的电平出于哪种状态时是空闲态或者有效态，时钟相位CPHA 
是用来配置数据采样是在第几个边沿：
CPOL=0，表示当SCLK=0时处于空闲态，所以有效状态就是SCLK处于高电平时
CPOL=1，表示当SCLK=1时处于空闲态，所以有效状态就是SCLK处于低电平时
CPHA=0，表示数据采样是在第1个边沿，数据发送在第2个边沿
CPHA=1，表示数据采样是在第2个边沿，数据发送在第1个边沿

********************************************************************/

static void xIoPort_set_pin_dir(GPIO_TypeDef *port,uint16_t gpio_pin,GPIOMode_TypeDef dir_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = gpio_pin;
	GPIO_InitStructure.GPIO_Mode = dir_state;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port,&GPIO_InitStructure);
}

static void xIoPort_set_pin_out(GPIO_TypeDef *port,uint16_t gpio_pin,uint8_t state)
{
	if(state == 1)
	{
		GPIO_SetBits(port,gpio_pin);
	}
	else
	{
		GPIO_ResetBits(port,gpio_pin);
	}
}

static uint8_t xIoPort_pin_get(GPIO_TypeDef *port,uint16_t gpio_pin)
{
	return GPIO_ReadInputDataBit(port,gpio_pin);
}

/*********************************************************************
* @fn      SPISimulateInit
*
* @brief   模拟SPI初始化
*           
*
* @param   none
*
* @return  none
*/
void SPISimulateInit( Spi_Parameter_t *pSpi )
{
  if( pSpi == (void *)0 )
    return;
  
  if( pSpi->SpiType == SPIMaster )
  {
    xIoPort_set_pin_dir( pSpi->nCS.port, pSpi->nCS.pin, IOPORT_DIR_OUTPUT );
    xIoPort_set_pin_dir( pSpi->nSCLK.port, pSpi->nSCLK.pin, IOPORT_DIR_OUTPUT );
    xIoPort_set_pin_dir( pSpi->nMOSI.port, pSpi->nMOSI.pin, IOPORT_DIR_OUTPUT );
    xIoPort_set_pin_dir( pSpi->nMISO.port, pSpi->nMISO.pin, IOPORT_DIR_INPUT );
  }
  else
  {
    xIoPort_set_pin_dir( pSpi->nCS.port, pSpi->nCS.pin, IOPORT_DIR_INPUT );
    xIoPort_set_pin_dir( pSpi->nSCLK.port, pSpi->nSCLK.pin, IOPORT_DIR_INPUT );
    xIoPort_set_pin_dir( pSpi->nMOSI.port, pSpi->nMOSI.pin, IOPORT_DIR_INPUT );
    xIoPort_set_pin_dir( pSpi->nMISO.port, pSpi->nMISO.pin, IOPORT_DIR_OUTPUT );
  }
  
  xIoPort_set_pin_out( pSpi->nCS.port, pSpi->nCS.pin, HIGH_OUT );//CS 置高
  
  switch(pSpi->SpiMode)
  {
    case Mode_1:
    case Mode_2:
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      break;  
    case Mode_3:
    case Mode_4:
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      break;
  }
}

/*********************************************************************
* @fn      SPISimulate_CSIsEnable
*
* @brief   模拟SPI片选使能
*           
*
* @param   none
*
* @return  none
*/
void SPISimulate_CSIsEnable( Spi_Parameter_t *pSpi, int IsEnable )
{
  if( pSpi == (void *)0 )
    return;
  
  if( IsEnable )
  {
    xIoPort_set_pin_out( pSpi->nCS.port, pSpi->nCS.pin, LOW_OUT );//使能
  }
  else
  {
    xIoPort_set_pin_out( pSpi->nCS.port, pSpi->nCS.pin, HIGH_OUT );//禁止使能
  }
}

/*********************************************************************
* @fn      SPISimulate_WriteByte
*
* @brief   模拟SPI写一个字节数据
*           
*
* @param   none
*
* @return  none
*/
void SPISimulate_WriteByte( Spi_Parameter_t *pSpi, unsigned char nByte )
{
  int i = 0;
  
  if( pSpi == (void *)0 )
    return;
  
  switch( pSpi->SpiMode )
  {
    case Mode_1://当SCLK=0时处于空闲态，数据采样是在上升沿，数据发送是在下降沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      
      for( i = 8; i > 0; i-- )//高位在前
      {  
        if( nByte & 0x80 )
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, HIGH_OUT );
        else
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, LOW_OUT );
        
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        nByte <<= 1;
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      break;
      
    case Mode_2://当SCLK=0时处于空闲态，数据采样是在下降沿，数据发送是在上升沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      
      for( i = 8; i > 0; i-- )//高位在前
      {
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );      
        
        if( nByte & 0x80 )
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, HIGH_OUT );
        else
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, LOW_OUT );
        
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        nByte <<= 1;
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      break;
      
    case Mode_3://当SCLK=1时处于空闲态，数据采集是在下降沿，数据发送是在上升沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      
      for( i = 8; i > 0; i-- )//高位在前
      {
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );      
        
        if( nByte & 0x80 )
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, HIGH_OUT );
        else
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, LOW_OUT );
        
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        nByte <<= 1;
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      break;
      
    case Mode_4://当SCLK=1时处于空闲态，数据采集是在上升沿，数据发送是在下降沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      
      for( i = 8; i > 0; i-- )//高位在前
      {
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );      
        
        if( nByte & 0x80 )
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, HIGH_OUT );
        else
          xIoPort_set_pin_out( pSpi->nMOSI.port, pSpi->nMOSI.pin, LOW_OUT );
        
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
        nByte <<= 1;
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      break;
  }
}
/*********************************************************************
* @fn      SPISimulateWrite
*
* @brief   模拟SPI写数据
*           
*
* @param   none
*
* @return  none
*/
void SPISimulateWrite( Spi_Parameter_t *pSpi, unsigned char *pdata, int len )
{
	int i;
	
  if( pSpi == (void *)0 || pdata == (void *)0 || len <= 0 )
    return;
  
  SPISimulate_CSIsEnable( pSpi, 1 );
  delay_us(8);
  
  for( i = 0; i < len; i++ )
    SPISimulate_WriteByte( pSpi, pdata[i] );
  
  delay_us(8);
  SPISimulate_CSIsEnable( pSpi, 0 );
}
/*********************************************************************
* @fn      SPISimulate_ReadByte
*
* @brief   模拟SPI读取一个字节
*           
*
* @param   none
*
* @return  none
*/
unsigned char SPISimulate_ReadByte( Spi_Parameter_t *pSpi )
{
  int i = 0;
  unsigned char Rdata = 0;
  unsigned char IoVal = 1<<pSpi->nMISO.pin;
  
  if( pSpi == (void *)0 )
    return 0;
  
  switch( pSpi->SpiMode )
  {
    case Mode_1://当SCLK=0时处于空闲态，数据采样是在上升沿，数据发送是在下降沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      
      for( i = 0; i < 8; i++ )
      { 
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        
        Rdata <<= 1;
        Rdata |= (xIoPort_pin_get(pSpi->nMISO.port, pSpi->nMISO.pin) == IoVal)? 1 : 0;
        
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      break;
      
    case Mode_2://当SCLK=0时处于空闲态，数据采样是在下降沿，数据发送是在上升沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      
      for( i = 0; i < 8; i++ )
      { 
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
        
        
        Rdata |= (xIoPort_pin_get(pSpi->nMISO.port, pSpi->nMISO.pin) == IoVal)? 1 : 0;
        Rdata <<= 1;
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
      break;
      
    case Mode_3://当SCLK=1时处于空闲态，数据采集是在下降沿，数据发送是在上升沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      
      for( i = 0; i < 8; i++ )
      { 
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
        
        Rdata <<= 1;
        Rdata |= (xIoPort_pin_get(pSpi->nMISO.port, pSpi->nMISO.pin) == IoVal)? 1 : 0;
        
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      break;
      
    case Mode_4://当SCLK=1时处于空闲态，数据采集是在上升沿，数据发送是在下降沿
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      
      for( i = 0; i < 8; i++ )
      { 
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, LOW_OUT );
        SPIDelay;
        xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
        
        Rdata <<= 1;
        Rdata |= (xIoPort_pin_get(pSpi->nMISO.port, pSpi->nMISO.pin) == IoVal)? 1 : 0;
        
        SPIDelay;
      }
      
      xIoPort_set_pin_out( pSpi->nSCLK.port, pSpi->nSCLK.pin, HIGH_OUT );
      break;
  }
  
  return Rdata;
}
/*********************************************************************
* @fn      SPISimulateRead
*
* @brief   模拟SPI读数据
*           
*
* @param   none
*
* @return  none
*/
void SPISimulateRead( Spi_Parameter_t *pSpi, unsigned char *pdata, int len )
{
	int i;
	
  if( pSpi == (void *)0 || pdata == (void *)0 || len <= 0 )
    return;
  
  SPISimulate_CSIsEnable( pSpi, 1 );
  delay_us(8);
  
  for( i = 0; i < len; i++ )
  {
//    SPISimulate_WriteByte( pSpi, 0x00 );
    pdata[i] = SPISimulate_ReadByte( pSpi );
  }
  
  delay_us(8);
  SPISimulate_CSIsEnable( pSpi, 0 );
}


