#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char Jumper(void);
void OpenIo(void);
void CloseIo(void);
void OpenIeds(void);
void CloseIeds(void);
void Sound(int i);
void Beep(int i);
void Freq(int i);
void Leds(int i);
void Play(int f, int d);
int SPIFlashMount(void);
int SPIFlashUnMount(void);

#ifdef __cplusplus
}
#endif
#endif // SPI_H
