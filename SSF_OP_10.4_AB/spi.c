/*
	spi.c

	SPI I/O demo

	written by KHP on Sep. 22, 2011
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "stdlib.h"
//#include "mclibdefine.h"
#define SPIBIO	"/sys/devices/platform/omap2_mcspi.2/spi2.1/spibio/"
#include "spi.h"

#define TB_SMALL_MIXED_1PHASE_DOUBLE    1       //kimkt(121027) test



#ifdef __arm__
static int sfd, bfd, ffd, jfd, lfd;
static char buf[5];
#endif

#ifdef __arm__
static void build_cmd(int i)
{
	sprintf(buf, "%d", i);
}
#endif

unsigned char Jumper(void)
{
	int j;
#ifdef __arm__
	read(jfd, buf, 3);
	sscanf(buf, "%x", &j);
#else
	// Define for PC test Machine type
	// #include "mclibdefine.h" ÌååÏùºÏóêÏÑú Í∏∞Í≥Ñ Ï ïÎ≥¥ Ï∞∏Í≥  ÌïòÏó¨ Î¶¨ÌÑ¥ Í∞í ÏûÖÎ • ÌïòÏÑ∏Ïöî
	//j = MULTI_SB_1PHASE;
    j = TB_SMALL_MIXED_1PHASE_DOUBLE;
#endif
	return j;
}

void Sound(int i)
{
#ifdef __arm__
	build_cmd(i);
	write(sfd, buf, 3);
#endif
}

void Beep(int i)
{
#ifdef __arm__
	build_cmd(i);
	write(bfd, buf, 3);
#endif
}

void Freq(int i)
{
#ifdef __arm__
	build_cmd(i);
	write(ffd, buf, 3);
#endif
};

void Leds(int i)
{
#ifdef __arm__
	build_cmd(i);
	write(lfd, buf, 3);
#endif
}

void Play(int f, int d)
{
#ifdef __arm__
	Freq(f);
	Beep(d);
	usleep(10000 * d + 1000);
#endif
}

void OpenIo(void)
{
#ifdef __arm__
	sfd = open(SPIBIO"sound", O_WRONLY);
	bfd = open(SPIBIO"beep", O_WRONLY);
	ffd = open(SPIBIO"freq", O_WRONLY);
	lfd = open(SPIBIO"leds", O_WRONLY);
	jfd = open(SPIBIO"jumper", O_RDONLY);
#endif
}

void CloseIo(void)
{
#ifdef __arm__
	close(sfd);
	close(bfd);
	close(ffd);
	close(lfd);
	close(jfd);
#endif
}

void OpenIeds(void)
{
#ifdef __arm__
	lfd = open(SPIBIO"leds", O_WRONLY);
#endif
}

void CloseIeds(void)
{
#ifdef __arm__
	close(lfd);
#endif
}

int SPIFlashMount(void)
{
#ifdef __arm__
	return system("mount -t jffs2 /dev/mtdblock7 /mnt");
#else
	return -1;
#endif
}

int SPIFlashUnMount(void)
{
#ifdef __arm__
	return system("umount /mnt");
#else
	return -1;
#endif
}

#if 0
int mainControlTest(void)
{
	int i;

	OpenIo();

	printf("Jumper = %02X\n", Jumper());

	Sound(1);

	for (i = 5; i < 25; i++) {
		Play(i * 10, 4);
		Leds(1 + (i & 1));
	}

	for (i = 25; i >= 10; i--) {
		Play(i * 10, 4);
		Leds(1 + (i & 1));
	}

	Sound(0);

	Leds(0);

	CloseIo();

	return 0;
}
#endif


//---------------------------------------------------------------------
//---------------------------------------------------------------------
/*
		QFile srcFile;
		QString dest = INSTALL_FILE_APP_PATH;
		QString dstFileName;

		bool result = true;
		bool current_file_copy = false;
		bool important_file_flag = false;

		for(i = 0;i<installList.count();i++){

			srcFile.setFileName(QString("%1/%2").arg(usbPath,installList.at(i)));//.arg(INSTALL_FILE_USB_PATH,installList.at(i)));
			dstFileName = QString("%1/%2").arg(dest,installList.at(i));
			ui->laStatus->setText(installList.at(i));
			current_file_copy = false;

			switch(i){
			case 0://installproc
			case 6://init
			case 7://exec_qt
				if(ErrorStatusCall == 1 && !srcFile.exists() && (i == 0)){//Installproc¿Ã æ¯¿∏∏È error
					result = false;
				}
				else if(srcFile.exists()){
					ui->progressBar->setValue(80/(installList.count())*(i+1));
					qApp->processEvents();
					if(QFile::exists(dstFileName)){
						QFile::remove(dstFileName);
					}
					result = srcFile.copy(dstFileName);
					if(result == true){
						current_file_copy = true;
						important_file_flag = true;
#if __arm__
						system("sync");
						system("mount -o remount,rw /");
						if(i == 0){
							if(QFile::exists("/SunStar/installproc")){
								QFile::remove("/SunStar/installproc");
							}
							system("cp /SunStar/app/installproc /SunStar/ -a");
						}else if(i == 6){
							if(QFile::exists("/SunStar/init")){
								QFile::remove("/SunStar/init");
							}
							system("cp /SunStar/app/init /SunStar/ -a");
						}else if(i == 7){
							if(QFile::exists("/SunStar/exec_qt")){
								QFile::remove("/SunStar/exec_qt");
							}
							system("cp /SunStar/app/exec_qt /SunStar/ -a");
						}
						sleep(5);
						system("sync");
						sleep(1);
						system("mount -o remount,ro /");
						sleep(1);
#endif
					}
					else{
						MsgBox msgErr(this);
						msgErr.SetText(QString("copy error %1").arg(result));
						msgErr.exec();
						goto _ERROR_EXIT;
					}
				}
				break;
*/
