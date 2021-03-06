

#ifndef	__S3C_DMA_PL330_H_
#define	__S3C_DMA_PL330_H_

#define S3C2410_DMAF_AUTOSTART		(1 << 0)
#define S3C2410_DMAF_CIRCULAR		(1 << 1)

enum dma_ch {
	DMACH_UART0_RX,
	DMACH_UART0_TX,
	DMACH_UART1_RX,
	DMACH_UART1_TX,
	DMACH_UART2_RX,
	DMACH_UART2_TX,
	DMACH_UART3_RX,
	DMACH_UART3_TX,
	DMACH_IRDA,
	DMACH_I2S0_RX,
	DMACH_I2S0_TX,
	DMACH_I2S0S_TX,
	DMACH_I2S1_RX,
	DMACH_I2S1_TX,
	DMACH_I2S2_RX,
	DMACH_I2S2_TX,
	DMACH_SPI0_RX,
	DMACH_SPI0_TX,
	DMACH_SPI1_RX,
	DMACH_SPI1_TX,
	DMACH_SPI2_RX,
	DMACH_SPI2_TX,
	DMACH_AC97_MICIN,
	DMACH_AC97_PCMIN,
	DMACH_AC97_PCMOUT,
	DMACH_EXTERNAL,
	DMACH_PWM,
	DMACH_SPDIF,
	DMACH_HSI_RX,
	DMACH_HSI_TX,
	DMACH_PCM0_TX,
	DMACH_PCM0_RX,
	DMACH_PCM1_TX,
	DMACH_PCM1_RX,
	DMACH_PCM2_TX,
	DMACH_PCM2_RX,
	DMACH_MSM_REQ3,
	DMACH_MSM_REQ2,
	DMACH_MSM_REQ1,
	DMACH_MSM_REQ0,
	/* END Marker, also used to denote a reserved channel */
	DMACH_MAX,
};

static inline bool s3c_dma_has_circular(void)
{
	return true;
}

#include <plat/dma.h>

#endif	/* __S3C_DMA_PL330_H_ */
