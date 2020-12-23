/*
 * waveplayer.h
 *
 *  Created on: 2020. 11. 17.
 *      Author: USER
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WAVEPLAYER_H
#define __WAVEPLAYER_H

/* .WAV file format :

 Endian      Offset      Length      Contents
  big         0           4 bytes     'RIFF'             // 0x52494646
  little      4           4 bytes     <file length - 8>
  big         8           4 bytes     'WAVE'             // 0x57415645

Next, the fmt chunk describes the sample format:

  big         12          4 bytes     'fmt '          // 0x666D7420
  little      16          4 bytes     0x00000010      // Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001          // Format tag: 1 = PCM
  little      22          2 bytes     <channels>      // Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>   // Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>  // sample rate * block align
  little      32          2 bytes     <block align>   // channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>   // 8 or 16

Finally, the data chunk contains the sample data:

  big         36          4 bytes     'data'        // 0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>

*/

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint8_t		ChunkID[4];		// Contains the letters "RIFF" in ASCII form
	uint32_t	ChunkSize;		// 36 + SubChunk2Size(DATA.Chunksize)
	uint8_t		Format[4];		// Contains the letters "WAVE" in ASCII form
} RIFF;

typedef struct
{
	uint8_t		ChunkID[4];		// Contains the letters "fmt " in ASCII form
	uint32_t	ChunkSize;		// 16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
	uint16_t	AudioFormat;	// PCM = 1
	uint16_t	NumChannels;	// Mono = 1, Stereo = 2, etc.
	uint32_t	SampleRate;		// 8000, 44100, etc.
	uint32_t	ByteRate;		// SampleRate * NumChannels * BitsPerSample/8
	uint16_t	BlockAlign;		// NumChannels * BitsPerSample/8
	uint16_t	BitsPerSample;	// 8 bits = 8, 16 bits = 16, etc
} FMT;

typedef struct
{
	uint8_t		ChunkID[4];		// Contains the letters "data" in ASCII form
	uint32_t	ChunkSize;		// NumSamples * NumChannels * BitsPerSample/8
} DATA;

typedef struct
{
	RIFF	    Riff;
	FMT			Fmt;
	DATA	    Data;
} WAVE_HEADER;

typedef enum
{
	Valid_WAVE_File = 0,
	Unvalid_RIFF_ID,
	Unvalid_WAVE_Format,
	Unvalid_FormatChunk_ID,
	Unvalid_FormatChunk_Size,
	Unsupporetd_FormatTag,
	Unsupporetd_Number_Of_Channel,
	Unsupporetd_Sample_Rate,
	Unsupporetd_Bits_Per_Sample,
	Unvalid_DataChunk_ID,
	Unsupporetd_ExtraFormatBytes,
	Unvalid_FactChunk_ID
} ErrorCode;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int8_t WavePlayer_Start(char* path);



#endif /*__WAVEPLAYER_H */
