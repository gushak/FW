#include "common.h"
#include "serial_api.h"
#include "i2c_api.h"
#include "spi_api.h"
#if DEVICE_CAN
#include "can_api.h"
#endif

Timeout *pJustBootup = NULL;
bool DevKeepRunning = false;
volatile uint32_t BitOpMode = 0;
uint32_t devHandle = 0;
uint8_t	BitOpModeBuffer[16] = { 0 };

void _Main();

void closeBootup()
{
	delete pJustBootup;
	pJustBootup = NULL;
}
void killSystem() 
{	
	DevKeepRunning = false; 
}
int main()
{
	DBGMSG(0, "[MAIN] Process Started===>\r\n");

	pJustBootup = new Timeout();
	pJustBootup->attach(closeBootup, 6.0f);
	DevKeepRunning = true;
	while (DevKeepRunning)
	{
		while (BitOpMode & 0xff00)
		{
//			int size = gUsbDisk.cdc_getRxSize();
//			if (size >= 4)
//			{
//				sendDirectDevice(gUsbDisk.cdc_getRxBuffer(), size);
//				gUsbDisk.cdc_flushRx(size);
//			}
//			if(readableDirectDevice() && gUsbDisk.isCDC_writable())
//			{
//				
//				int size = receiveDirectDevice(BitOpModeBuffer);
//				size = gUsbDisk.cdc_put(BitOpModeBuffer, size);
//			}
			wait(0.01);
		}
		wait(1);
	}
	DBGMSG(0, "[MAIN] Terminate \r\n");
	wait(1);
	_Main();
}

void _CloseDevice()
{
	uint32_t baseTick = HAL_GetTick();
	uint32_t curTick;
	
//	closePCIF();
//	gUsbDisk.killProcess();
	if (pgConf->isConfChanged())
		pgConf->SaveConfig();
	
//	while (HAL_GetElapsedTick(baseTick) < 4000 && gUsbDisk.getThreadState() != Thread::Deleted);
//	
//	gGps.killProcess();
//	gDisp.killProcess();
	HAL_Delay(100);
	
	#if defined(USE_JUMP2BOOTLOADER)
	uint32_t mark = FLASH_CONFIG_UPDATE_PENDING_MARK;
	int count = sizeof(CFG(fw_update)) / sizeof(CFG(fw_update[0])) - 1;
	while (count >= 0) 
	{
		if (CFG(fw_update[count]) == mark)
		{
			JumpToBootloader();
			while(1);
		}
		count--;	
	}
	#endif
	
//	power_off = LOW;
}
void _Main()
{
	_CloseDevice();
	while (true);
}

void PowerOff(uint32_t opr)
{
	killSystem();
	DBGMSG(0, "[MAIN] Trigger main termination by Power-Off\r\n");
}

void FactoryMode(uint32_t opr)
{
		CTX_MAIN(facSeq)++;
//		if (!gUsbDisk.isConfigured())
//		{
//			return;
//		}
//		gGps.Change2NextOutputFormat();
		printf("Operation Mode is changed to %d!!!\r\n", CTX_MAIN(facSeq));
}
void ReStart()
{
	
}

//void setUSBProcessStep(bool bMount)
//{
//	if (bMount)
//		gUsbDisk.pending_steps = USB_DISK::eUSBDISK_LOCAL_MOUNT;
//	else
//		gUsbDisk.pending_steps = USB_DISK::eUSBDISK_LOCAL_DEMOUNT;
//}
//
//bool isUSBconnected()
//{
//	return gUsbDisk.isConnected();
//}
//
//void openPCIF()
//{
//	if (gpPcif == NULL)
//		gpPcif = new PCIF();
//}
//
//void closePCIF()
//{
//	if (gpPcif != NULL)
//		delete gpPcif;
//}

void JumpToBootloader(void) 
{
	void(*SysMemBootJump)(void);
    
	/**
	 * Step: Set system memory address. 
	 *       
	 *       For STM32F429, system memory is on 0x1FFF 0000
	 *       For other families, check AN2606 document table 110 with descriptions of memory addresses 
	 */
	volatile uint32_t addr = 0x1FFFF000;
    

	/**
	 * Step: Disable RCC, set it to default (after reset) settings
	 *       Internal clock, no PLL, etc.
	 */
#if defined(USE_HAL_DRIVER)
	HAL_RCC_DeInit();
#endif /* defined(USE_HAL_DRIVER) */
#if defined(USE_STDPERIPH_DRIVER)
	RCC_DeInit();
#endif /* defined(USE_STDPERIPH_DRIVER) */
    
	    /**
	     * Step: Disable systick timer and reset it to default values
	     */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
 
    /**
     * Step: Disable all interrupts
     */
	__disable_irq();
    
	/**
	 * Step: Remap system memory to address 0x0000 0000 in address space
	 *       For each family registers may be different. 
	 *       Check reference manual for each family.
	 *
	 *       For STM32F4xx, MEMRMP register in SYSCFG is used (bits[1:0])
	 *       For STM32F0xx, CFGR1 register in SYSCFG is used (bits[1:0])
	 *       For others, check family reference manual
	 */
	//Remap by hand... {
#if defined(STM32F4)
	SYSCFG->MEMRMP = 0x01;
#endif
#if defined(STM32F0)
	SYSCFG->CFGR1 = 0x01;
#endif
	    //} ...or if you use HAL drivers
	    //__HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();    //Call HAL macro to do this for you
    
	    /**
	     * Step: Set jump memory location for system memory
	     *       Use address with 4 bytes offset which specifies jump location where program starts
	     */
	SysMemBootJump = (void(*)(void))(*((uint32_t *)(addr + 4)));
	SCB->VTOR = addr;
	/**
	 * Step: Set main stack pointer.
	 *       This step must be done last otherwise local variables in this function
	 *       don't have proper value since stack pointer is located on different position
	 *
	 *       Set direct address location which specifies stack pointer in SRAM location
	 */
	__set_MSP(*(uint32_t *)addr);
    
	/**
	 * Step: Actually call our function to jump to set location
	 *       This will start system memory execution
	 */
	SysMemBootJump();
    
	/**
	 * Step: Connect USB<->UART converter to dedicated USART pins and test
	 *       and test with bootloader works with STM32 Flash Loader Demonstrator software
	 */
}

//	pLineCoding.format      0: 1bit Stop, 2: 2bits Stop
//	pLineCoding.paritytype  0:None, 1:Odd, 2:Even, 3:Mark, 4:Space
//	pLineCoding.datatype    5:5bits, 6:6bits, 7:7bits, 8:8bits
void setDirectDevice(USBD_CDC_LineCodingTypeDef * pLineCoding)
{
	uint16_t curOpMode = ((uint32_t)((pLineCoding->datatype - 5) << 4) | pLineCoding->paritytype) << 8;
	if ((BitOpMode >> 15) && curOpMode == (BitOpMode & 0xEF00))
		return;

	// UART1: 5bits-None, UART2: 5bits-Odd, UART3: 5bits-Even, UART4: 5bits-Mark, UART5: 5bits-Space,
	// UART6: 6bits-None, I2C1: 6bits-Odd, I2C2: 6bits-Even, I2C3: 6bits-Mark, SPI1: 6bits-Space
	// SPI2: 7bits-None, SPI3: 7bits-Odd, SPI3: 7bits-Even, CAN1: 7bits-Mark, SDIO: 7bits-Space
	switch (pLineCoding->datatype)
	{
	case 5:	//5 bits
		{
			devHandle = getSerialHandle(pLineCoding->paritytype);
			serial_baud((serial_t*)devHandle, pLineCoding->bitrate);
		}
		break;
	case 6:	//6 bits
		if (pLineCoding->paritytype < 1)
		{
			devHandle = getSerialHandle(5 + pLineCoding->paritytype);
			serial_baud((serial_t*)devHandle, pLineCoding->bitrate);
		}
		else if (pLineCoding->paritytype < 4)
		{
			devHandle = getI2cHandle(pLineCoding->paritytype - 1);
			i2c_frequency((i2c_t *)devHandle, (pLineCoding->bitrate > 400000) ? 400000 : pLineCoding->bitrate);
		}
		else
		{
			devHandle = getSpiHandle(pLineCoding->paritytype - 4);
			spi_frequency((spi_t *)devHandle, pLineCoding->bitrate*3);
		}
		break;
	case 7:	//7 bits
		switch (pLineCoding->paritytype)
		{
		case 0:	// None parity
		case 1:	// Odd parity
			devHandle = getSpiHandle(pLineCoding->paritytype + 1);
			spi_frequency((spi_t *)devHandle, pLineCoding->bitrate * 3);
		case 2:	// Even parity
		case 3:	// Mark parity
			#if DEVICE_CAN
			devHandle = getCanHandle(pLineCoding->paritytype - 2);
			can_frequency((can_t *)devHandle, pLineCoding->bitrate * 3);
			#endif
			break;
		case 4:	// Space parity
			#if DEVICE_SDIO
			devHandle = getSdioHandle(pLineCoding->paritytype - 4);
			sdio_frequency((sdio_t *)devHandle, pLineCoding->bitrate * 3);
			#endif
			break;
		default :
			break;
		}
		break;
	default:
		return;
	}

	BitOpMode = eDOM_DIRECT_DEVICE;
	BitOpMode &= ~0x7FFF;
	BitOpMode |= curOpMode;
}

void sendDirectDevice(uint8_t *buff, int len)
{
	int index = 0;
	switch (BitOpMode & 0xff00)
	{
	case DOM_UART1:
	case DOM_UART2:
	case DOM_UART3:
	case DOM_UART4:
	case DOM_UART5:
	case DOM_UART6:
		while (index < len)
		{
			if (serial_writable((serial_t *)devHandle))
				serial_putc((serial_t *)devHandle, buff[index++]);
		}
		break;
	case DOM_I2C1:
	case DOM_I2C2:
	case DOM_I2C3: 
		{
			int dev_addr;
			i2c_write((i2c_t *)devHandle, dev_addr, (const char *)buff, len, 0);
		}
		break;
	case DOM_SPI1:
	case DOM_SPI2:
	case DOM_SPI3:
		break;
	case DOM_CAN1:
	case DOM_CAN2:
		break;
	case DOM_SDIO:
		break;
	}
}
int readableDirectDevice()
{
	switch (BitOpMode & 0xff00)
	{
	case DOM_UART1:
	case DOM_UART2:
	case DOM_UART3:
	case DOM_UART4:
	case DOM_UART5:
	case DOM_UART6:
		return serial_readable((serial_t *)devHandle);
	case DOM_I2C1:
	case DOM_I2C2:
	case DOM_I2C3: 

		break;
	case DOM_SPI1:
	case DOM_SPI2:
	case DOM_SPI3:
		break;
	case DOM_CAN1:
	case DOM_CAN2:
		break;
	case DOM_SDIO:
		break;
	}

}
int receiveDirectDevice(uint8_t *buff)
{
	int index = 0;
	switch (BitOpMode & 0xff00)
	{
	case DOM_UART1:
	case DOM_UART2:
	case DOM_UART3:
	case DOM_UART4:
	case DOM_UART5:
	case DOM_UART6:
		while (serial_readable((serial_t *)devHandle))
		{
			buff[index++] = serial_getc((serial_t *)devHandle);
		}
		break;
	case DOM_I2C1:
	case DOM_I2C2:
	case DOM_I2C3: 

		break;
	case DOM_SPI1:
	case DOM_SPI2:
	case DOM_SPI3:
		break;
	case DOM_CAN1:
	case DOM_CAN2:
		break;
	case DOM_SDIO:
		break;
	}
	
	return index;
}