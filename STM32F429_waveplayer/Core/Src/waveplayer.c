/*
 * waveplayer.c
 *
 *  Created on: 2020. 11. 17.
 *      Author: USER
 */
/* Includes ------------------------------------------------------------------*/
#include "waveplayer.h"
#include "dac.h"
#include "dma.h"
#include "usart.h"
#include "main.h"
#include "fatfs.h"
#include "tim.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	READY = 0,
	BUSY
} Buff_State;

typedef enum
{
	RUN = 0,
	DONE
} DMA_State;

/* Private define ------------------------------------------------------------*/
#define  RIFFID         0x46464952  /* correspond to the letters 'RIFF' */
#define  WAVEID			0x45564157  /* correspond to the letters 'WAVE' */
#define  FMT_ID         0x20746D66  /* correspond to the letters 'fmt ' */
#define  DataID         0x61746164  /* correspond to the letters 'data' */
#define  FactID         0x74636166  /* correspond to the letters 'fact' */

#define  WAVE_FORMAT_PCM     0x01
#define  FormatChunkSize     0x10
#define  Channel_Mono        0x01

#define  SampleRate_8000     8000
#define  SampleRate_11025    11025
#define  SampleRate_22050    22050
#define  SampleRate_44100    44100
#define  SampleRate_48000    48000
#define  Bits_Per_Sample_8   8
#define  Bits_Per_Sample_16  16

/* Private macro -------------------------------------------------------------*/
//#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+((WORD)c2<<8)+(BYTE)c1)
#define	LD_WORD(ptr)		(WORD)(((WORD)*(BYTE*)((ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(((DWORD)*(BYTE*)((ptr)+3)<<24)|((DWORD)*(BYTE*)((ptr)+2)<<16)|((WORD)*(BYTE*)((ptr)+1)<<8)|*(BYTE*)(ptr))

/* Private variables ---------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_dac2;
static WAVE_HEADER			WAVE_Format;
static __IO ErrorCode WaveFileStatus = Unvalid_RIFF_ID;
static __IO uint32_t WaveDataLength;
extern __IO uint16_t TIM6ARRValue;

#define HEAD_SIZE	44
#define BUFF_SIZE	512
uint8_t wave_header[HEAD_SIZE];
uint16_t wavBuff1[BUFF_SIZE];
uint16_t wavBuff2[BUFF_SIZE];
uint8_t BUFF_SEL = 1;

extern uint16_t dma_complete;


/* Private function prototypes -----------------------------------------------*/
static ErrorCode WavePlayer_WaveParsing(void);
static int8_t WAV_Browser (char* fileName);
static int8_t read_waveheader(void);
static void read_wavedata(void);

/* Private functions ---------------------------------------------------------*/
int8_t WavePlayer_Start(char* path)
{
	if(f_mount(&USERFatFS, USERPath, 0) != FR_OK)
	{
    	/* fat_fs initialisation fails*/
		printf("Err : Cannot initialize File System!!!\n\r");
        return (-1);
    }

	/* Wave file Header Read */
	if(WAV_Browser(path) != FR_OK)
	{
		f_mount(NULL, USERPath, 1);
		return (-1);
	}

	WaveFileStatus = WavePlayer_WaveParsing();

	if(WaveFileStatus == Valid_WAVE_File) /* the .WAV file is valid */
	{
		/* Set WaveDataLenght to the Speech wave length */
		WaveDataLength = WAVE_Format.Data.ChunkSize;

		printf(" - Valid Wavd File....\n\r");

		MX_DAC_Init();
		MX_TIM6_Init(TIM6ARRValue);

		read_wavedata();
	}
	else
	{
		printf(" - Not Valid Wavd File!!!\n\r");
	}

	return 0;
}

static int8_t WAV_Browser (char* fileName)
{
	if(f_open(&USERFile, fileName, FA_READ) == FR_OK)
	{
		read_waveheader();
		return FR_OK;
	}
	else
	{
		printf("Err : File open Fail(%s)\n\r", fileName);
		return FR_NO_FILE;
	}
}

static int8_t read_waveheader(void)
{
	uint16_t numOfReadBytes = 0;
	FRESULT res;

	if(MMC_CDState()==1)
	{
	    res = f_read(&USERFile, wave_header, HEAD_SIZE, (void *)&numOfReadBytes);
	    if((numOfReadBytes == 0) || (res != FR_OK)) /*EOF or Error*/
	    {
	    	return (-1);
	    }
	//	printf("Wavefile Header +++++++++++\n");
	//	for(int i = 0 ; i < HEAD_SIZE; i++)
	//	{
	//	    printf("%02X ", wave_header[i]);
	//	}
	//	printf("\nWavefile Header +++++++++++\n");
	}
	else
	{
		printf("ERR : Not Insert SD Card!!!\n\r");
	}

	return FR_OK;
}

static ErrorCode WavePlayer_WaveParsing(void)
{
	/* Read chunkID, must be 'RIFF'  -----------------------------------------*/
	if(LD_DWORD(wave_header + 0) != RIFFID) //FCC('R','I','F','F'))//
	{
		printf("Err : Unvalid_RIFF_ID\n\r");
		return Unvalid_RIFF_ID;
	}

	/* Read the file length --------------------------------------------------*/
	WAVE_Format.Riff.ChunkSize = LD_DWORD(wave_header + 4);

	/* Read the file format, must be 'WAVE' ----------------------------------*/
	if(LD_DWORD(wave_header + 8) != WAVEID) //FCC('W','A','V','E'))//
	{
		printf("Err : Unvalid_WAVE_Format\n\r");
		return Unvalid_WAVE_Format;
	}

	/* Read the format chunk, must be'fmt ' ----------------------------------*/
	if(LD_DWORD(wave_header + 12) != FMT_ID) //FCC('f','m','t',' '))//
	{
		printf("Err : Unvalid_FormatChunk_ID\n\r");
		return Unvalid_FormatChunk_ID;
	}

	/* Read the length of the 'fmt' data, must be 0x10 -----------------------*/
	if(LD_DWORD(wave_header + 16) != FormatChunkSize)
	{
		printf("Err : Unvalid_FormatChunk_Size\n\r");
		return Unvalid_FormatChunk_Size;
	}

	/* Read the audio format, must be 0x01 (PCM) -----------------------------*/
	WAVE_Format.Fmt.AudioFormat = LD_WORD(wave_header + 20);
	if(WAVE_Format.Fmt.AudioFormat != WAVE_FORMAT_PCM)
	{
		printf("Err : Unsupporetd_FormatTag\n\r");
		return(Unsupporetd_FormatTag);
	}

	/* Read the number of channels, must be 0x01 (Mono) ----------------------*/
	WAVE_Format.Fmt.NumChannels = LD_WORD(wave_header + 22);
	if(WAVE_Format.Fmt.NumChannels != Channel_Mono)
	{
		printf("Err : Unsupporetd_Number_Of_Channel\n\r");
		return(Unsupporetd_Number_Of_Channel);
	}

	/* Read the Sample Rate --------------------------------------------------*/
	WAVE_Format.Fmt.SampleRate = LD_DWORD(wave_header + 24);

	/* Update the OCA value according to the .WAV file Sample Rate */
	switch(WAVE_Format.Fmt.SampleRate)
	{
		case SampleRate_8000 :
			TIM6ARRValue = 124;
			printf(" - SampleRate : %d\n\r", SampleRate_8000);
			break; /* 8KHz = 1MHz / 125 */
    	case SampleRate_11025:
			TIM6ARRValue = 90;
			printf(" - SampleRate : %d\n\r", SampleRate_11025);
			break; /* 11.025KHz = 1MHz / 91 */
    	case SampleRate_22050:
			TIM6ARRValue = 44;
			printf(" - SampleRate : %d\n\r", SampleRate_22050);
			break; /* 22.05KHz = 1MHz / 45 */
    	case SampleRate_44100:
			TIM6ARRValue = 22;
			printf(" - SampleRate : %d\n\r", SampleRate_44100);
			break; /* 44.1KHz = 1MHz / 23 */
		case SampleRate_48000:
			TIM6ARRValue = 20;
			printf(" - SampleRate : %d\n\r", SampleRate_48000);
			break; /* 48KHz = 1MHz / 21 */
    	default:
			printf("ERR(%X) : Unsupporetd_Sample_Rate\n\r", WAVE_Format.Fmt.SampleRate);
			return(Unsupporetd_Sample_Rate);
	}

	/* Read the Byte Rate ----------------------------------------------------*/
	WAVE_Format.Fmt.ByteRate = LD_DWORD(wave_header + 28);

	/* Read the block alignment ----------------------------------------------*/
	WAVE_Format.Fmt.BlockAlign = LD_WORD(wave_header + 32);

	/* Read the number of bits per sample ------------------------------------*/
	WAVE_Format.Fmt.BitsPerSample = LD_WORD(wave_header + 34);
	if((WAVE_Format.Fmt.BitsPerSample != Bits_Per_Sample_8)
	   && (WAVE_Format.Fmt.BitsPerSample != Bits_Per_Sample_16))
	{
		printf("Err : Unsupporetd_Bits_Per_Sample[%d bit]\n\r", WAVE_Format.Fmt.BitsPerSample);
		return(Unsupporetd_Bits_Per_Sample);
	}
	printf(" - BitPerSample : %d\n\r", WAVE_Format.Fmt.BitsPerSample);

	/* Read the Data chunk, must be 'data' -----------------------------------*/
	if(LD_DWORD(wave_header + 36) != DataID) //FCC('d','a','t','a'))//
	{
		printf("Err : Unvalid_DataChunk_ID\n\r");
		return(Unvalid_DataChunk_ID);
	}

	/* Read the number of sample data ----------------------------------------*/
	WAVE_Format.Data.ChunkSize = LD_DWORD(wave_header + 40);

	return(Valid_WAVE_File);
}

static void read_wavedata(void)
{
	uint16_t numOfReadBytes = 0;
	FRESULT res;
	uint8_t dma_state = DONE;
	uint8_t buf1 = READY, buf2 = READY;

	printf("wave data size : %X\r\n", WaveDataLength);

	/* TIM6 enable counter */
	HAL_TIM_Base_Start(&htim6);

	while(WaveDataLength != 0)
	{
		if(buf1 == READY)
		{
			res = f_read(&USERFile, wavBuff1, BUFF_SIZE, (void *)&numOfReadBytes);
			if((numOfReadBytes == 0) || (res != FR_OK))
			{
			//	printf("Err : File Read Fail +++++++++++\n");
				break;
			}
			else
			{
				WaveDataLength = WaveDataLength - BUFF_SIZE;

			//	printf("Wavefile Data1 +++++++++++\r\n");
			/*	for(int i = 0 ; i < BUFF_SIZE; i++)
				{
					printf("%02X ", wavBuff1[i]);
				}
				printf("\n");*/
			}
			buf1 = BUSY;
		}
		if(buf2 == READY)
		{
			res = f_read(&USERFile, wavBuff2, BUFF_SIZE, (void *)&numOfReadBytes);
			if((numOfReadBytes == 0) || (res != FR_OK))
			{
			//	printf("Err : File Read Fail +++++++++++\n");
				break;
			}
			else
			{
				WaveDataLength = WaveDataLength - BUFF_SIZE;

			//	printf("Wavefile Data2 +++++++++++\r\n");
			/*	for(int i = 0 ; i < BUFF_SIZE; i++)
				{
					printf("%02X ", wavBuff2[i]);
				}
				printf("\n");*/
			}
			buf2 = BUSY;
		}

		if(WaveDataLength < 512)
		{
			WaveDataLength = 0;
		}

		if(dma_state == DONE)
		{
			HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_2);
		//	printf("play buff(%d)...\r\n", BUFF_SEL);
			dma_state = RUN;

			if(BUFF_SEL == 1 && WAVE_Format.Fmt.BitsPerSample == 8)
			{
			//	printf("8bit buff1....\n");
				HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)wavBuff1, 512, DAC_ALIGN_8B_R);
			}
			else if(BUFF_SEL==2 && WAVE_Format.Fmt.BitsPerSample == 8)
			{
			//	printf("8bit buff2....\n");
				HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)wavBuff2, 512, DAC_ALIGN_8B_R);
			}
			else if(BUFF_SEL == 1 && WAVE_Format.Fmt.BitsPerSample == 16)
			{
			//	printf("16bit buff1....\n");
				HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)wavBuff1, 512, DAC_ALIGN_12B_R);
			}
			else if(BUFF_SEL ==2 && WAVE_Format.Fmt.BitsPerSample == 16)
			{
			//	printf("16bit buff2....\n");
				HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)wavBuff2, 512, DAC_ALIGN_12B_R);
			}
		}

		//DMA transfer flag 확인
//		printf("dma flag = %d\n", __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_dac2));
//		if(__HAL_DMA_GET_TC_FLAG_INDEX(&hdma_dac2))
//		printf("dma flag = %d\n", __HAL_DMA_GET_FLAG(&hdma_dac2, DMA_FLAG_TCIF2_6));
//		if(__HAL_DMA_GET_FLAG(&hdma_dac2, DMA_FLAG_TCIF2_6) == 0)
//		printf("dma flag status1 = %d\n", HAL_DMA_GetState(&hdma_dac2));
		if(HAL_DMA_GetState(&hdma_dac2) == HAL_DMA_STATE_READY)
		{
		//	printf("dma flag status2 = %d\n", HAL_DMA_GetState(&hdma_dac2));
		//	printf("dma_stat = done\n");
			dma_state = DONE;

			if(BUFF_SEL == 1)
			{
				buf1 = READY;
				BUFF_SEL = 2;
			}
			else
			{
				buf2 = READY;
				BUFF_SEL = 1;
			}
			//DMA transfer flag Clear
//			__HAL_DMA_CLEAR_FLAG(&hdma_dac2, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_dac2));
//			__HAL_DMA_CLEAR_FLAG(&hdma_dac2, DMA_FLAG_TCIF2_6);
//			printf("clear DMA flag [%d]\r\n", __HAL_DMA_CLEAR_FLAG(&hdma_dac2, DMA_FLAG_TCIF2_6));
		}
	}
	/* TIM6 disable counter */
	HAL_TIM_Base_Stop(&htim6);

	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_2);

//	HAL_DAC_MspDeInit(&hdac);

	f_close(&USERFile);
	f_mount(NULL, USERPath, 1);

	printf("Wave Play Stop!!!\r\n");
}

