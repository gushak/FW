#include "common.h"

PCIF *gpPcif = NULL;
uint8_t PCIF_TxBuff[32] = { 0xAA, 0x55, 0, };
static void pcif_process();

PCIF::PCIF()
{
	bKeepRunning = true;
	lastPacketBuff = PCIF_TxBuff;
	pcif_thread.start(pcif_process);
}

void pcif_process()
{
	DBGMSG(0, "[PCIF] Process Started===>\r\n");

	while (gpPcif->bKeepRunning)
	{
		while (BitOpMode & 0xff00)
		{
			wait(1);
		}

		int size = gUsbDisk.cdc_getRxSize();
		if (size < 5)
		{
			wait_ms(10);
			continue;
		}
		uint8_t * buff = gUsbDisk.cdc_getRxBuffer();
		int count = size;
		while (*(uint16_t*)buff != 0x55AA && count > 1)
		{
			buff ++;
			count--;
		}
		if (count != size)
		{
			gUsbDisk.cdc_flushRx(size - count);
			if (count < 2)
			{
				wait_ms(10);
				continue;
			}
		}
		buff = gUsbDisk.cdc_getRxBuffer();
		size = gUsbDisk.cdc_getRxSize();
		if (size < 4 || size < (*(uint16_t*)&buff[2] + 2))
		{
			wait_ms(10);
			continue;
		}
		size = *(uint16_t*)&buff[2];
		int off = 4;
		switch (buff[off])
		{
		case PCIF::eCMD_ACK:
			break;
		case PCIF::eCMD_NACK:
			gpPcif->send(gpPcif->lastPacketBuff, gpPcif->lastPacketSize);
			break;
		case PCIF::eCMD_DEVINFO:
			PCIF_TxBuff[2] = 14;	PCIF_TxBuff[3] = 0;
			PCIF_TxBuff[4] = PCIF::eCMD_DEVINFO;
			PCIF_TxBuff[5] = CFG(Setting).devId;

			//< SerialNo(UID) Query & Build ... as like usb serial-no generation rule
			memcpy(&PCIF_TxBuff[6], g_Context.mcuId, sizeof(g_Context.mcuId));
			gpPcif->send(PCIF_TxBuff, 18);
			break;
		case PCIF::eCMD_FWUPDATE:
			{
				int count = sizeof(CFG(fw_update)) / sizeof(CFG(fw_update[0])) - 1;
				while (CFG(fw_update[count])!=(uint32_t)(-1) && count >= 0) 
				{
					count--;	
				}
				uint32_t mark = FLASH_CONFIG_UPDATE_PENDING_MARK;
				if (count < 0 || gConf.isConfChanged())
				{
					if (count < 0)
						count = sizeof(CFG(fw_update)) / sizeof(CFG(fw_update[0])) - 1;
					
					gConf.ChangeConfig(&CFG(fw_update[count]), &mark, sizeof(CFG(fw_update[0])));
					gConf.SaveConfig();
				}
				else
				{
					gConf.ChangeConfig(&CFG(fw_update[count]), &mark, sizeof(CFG(fw_update[0])));
					gConf.SaveConfig(true);
				}
				PowerOff(MSG_KEY_PRESS_LONG);
				return;
			}
			break;
		case PCIF::eCMD_DISKVOLUME:
			if (buff[off+1] == 1) { //queried by PC
				PCIF_TxBuff[2] = 2;		PCIF_TxBuff[3] = 0;
				PCIF_TxBuff[4] = PCIF::eCMD_DISKVOLUME;
				PCIF_TxBuff[5] = CFG(Setting).devId;
				gpPcif->send(PCIF_TxBuff, 6);
			}
			else if (CFG(Setting).devId != buff[6])
				gConf.ChangeConfigU8(&CFG(Setting).devId, buff[6]); 
			break;
		case PCIF::eCMD_BLEINFO:
			break;
		case PCIF::eCMD_GNSSDISABLESVS:
			break;
		case PCIF::eCMD_GPS_DEFOUTPUT:
			break;
		case PCIF::eCMD_OUTPATH:
			break;
		case PCIF::eCMD_GNSSSAVE:

			break;
		case PCIF::eCMD_GPS:
			break;
		case PCIF::eCMD_RF:
			break;
		case PCIF::eCMD_IMU:
			break;
		case PCIF::eCMD_BT:
		}
		
		if (buff[off] < PCIF::eCMD_MAX)
			gUsbDisk.cdc_flushRx(2+buff[2]);

		wait_ms(10);
	}
}

void process_waiting_send()
{
	gpPcif->send(gpPcif->lastPacketBuff, gpPcif->lastPacketSize);
}

void PCIF::Init()
{
}

int PCIF::send(uint8_t *buff, uint8_t size)
{
	lastPacketSize = size;
	if (gUsbDisk.isCDC_writable())
		return gUsbDisk.cdc_put(buff, size);
	else
		Timeout4waitingSend.attach(process_waiting_send, 1.0f);
	return -1;
}