/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "i2c.h"

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void delay(uint32_t nCount);


/* Private variables ---------------------------------------------------------*/
TP_STATE TP_State;


/* Private user code ---------------------------------------------------------*/
void TSP_RESET(void)
{
	uint8_t data;

	/* Power Down the IO_Expander */
//	I2C_WriteDeviceRegister(IOE_REG_SYS_CTRL1, 0x02);
//	HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	data = 0x02;
  	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_SYS_CTRL1, 1, &data, 1, 10);
  	/* wait for a delay to insure registers erasing */
 	delay(20);
//  osDelay(20);

  	/* Power On the Codec after the power off => all registers are reinitialized*/
// 	I2C_WriteDeviceRegister(IOE_REG_SYS_CTRL1, 0x00);
//	HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
  	data = 0x00;
  	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_SYS_CTRL1, 1, &data, 1, 10);
}

void TSP_FnctCmd(uint8_t Fct, FunctionalState NewState)
{
  uint8_t tmp = 0;

  /* Get the register value */
//  tmp = I2C_ReadDeviceRegister(IOE_REG_SYS_CTRL2);
  HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_SYS_CTRL2, 1, &tmp, 1, 10);

  if (NewState != DISABLE)
  {
    /* Set the Functionalities to be Enabled */
    tmp &= ~(uint8_t)Fct;
  }
  else
  {
    /* Set the Functionalities to be Disabled */
    tmp |= (uint8_t)Fct;
  }

  /* Set the register value */
//  I2C_WriteDeviceRegister(IOE_REG_SYS_CTRL2, tmp);
  HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_SYS_CTRL2, 1, &tmp, 1, 10);
}

void TSP_IOAFConfig(uint8_t IO_Pin, FunctionalState NewState)
{
  uint8_t tmp = 0;

  /* Get the current state of the GPIO_AF register */
//  tmp = I2C_ReadDeviceRegister(IOE_REG_GPIO_AF);
  HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_GPIO_AF, 1, &tmp, 1, 10);

  if (NewState != DISABLE)
  {
    /* Enable the selected pins alternate function */
    tmp |= (uint8_t)IO_Pin;
  }
  else
  {
    /* Disable the selected pins alternate function */
    tmp &= ~(uint8_t)IO_Pin;
  }

  /* Write back the new value in GPIO_AF register */
//  I2C_WriteDeviceRegister(IOE_REG_GPIO_AF, tmp);
  HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_GPIO_AF, 1, &tmp, 1, 10);
}

void TSP_TP_Config(void)
{
	uint8_t tmp = 0;
	/* Enable touch Panel functionality */
	TSP_FnctCmd(IOE_TP_FCT, ENABLE);

	/* Select Sample Time, bit number and ADC Reference */
//	I2C_WriteDeviceRegister(IOE_REG_ADC_CTRL1, 0x49);
	tmp = 0x49;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_ADC_CTRL1, 1, &tmp, 1, 10);
	/* Wait for ~20 ms */
	delay(20);
//	osDelay(20);

	/* Select the ADC clock speed: 3.25 MHz */
//	I2C_WriteDeviceRegister(IOE_REG_ADC_CTRL2, 0x01);
	tmp = 0x01;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_ADC_CTRL2, 1, &tmp, 1, 10);

	/* Select TSC pins in non default mode */
	TSP_IOAFConfig((uint8_t)TOUCH_IO_ALL, DISABLE);

	/* Select 2 nF filter capacitor */
//	I2C_WriteDeviceRegister(IOE_REG_TP_CFG, 0x9A);
	tmp = 0x9A;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_TP_CFG, 1, &tmp, 1, 10);

	/* Select single point reading  */
//	I2C_WriteDeviceRegister(IOE_REG_FIFO_TH, 0x01);
	tmp = 0x01;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_FIFO_TH, 1, &tmp, 1, 10);

	/* Write 0x01 to clear the FIFO memory content. */
//	I2C_WriteDeviceRegister(IOE_REG_FIFO_STA, 0x01);
	tmp = 0x01;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_FIFO_STA, 1, &tmp, 1, 10);

	/* Write 0x00 to put the FIFO back into operation mode  */
//	I2C_WriteDeviceRegister(IOE_REG_FIFO_STA, 0x00);
	tmp = 0x00;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_FIFO_STA, 1, &tmp, 1, 10);

	/* set the data format for Z value: 7 fractional part and 1 whole part */
//	I2C_WriteDeviceRegister(IOE_REG_TP_FRACT_XYZ, 0x01);
	tmp = 0x01;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_TP_FRACT_XYZ, 1, &tmp, 1, 10);

	/* set the driving capability of the device for TSC pins: 50mA */
//	I2C_WriteDeviceRegister(IOE_REG_TP_I_DRIVE, 0x01);
	tmp = 0x01;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_TP_I_DRIVE, 1, &tmp, 1, 10);

	/* Use no tracking index, touch-panel controller operation mode (XYZ) and enable the TSC */
//	I2C_WriteDeviceRegister(IOE_REG_TP_CTRL, 0x03);
	tmp = 0x03;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_TP_CTRL, 1, &tmp, 1, 10);

	/*  Clear all the status pending bits */
//	I2C_WriteDeviceRegister(IOE_REG_INT_STA, 0xFF);
	tmp = 0xff;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_INT_STA, 1, &tmp, 1, 10);

	/* Initialize the TS structure to their default values */
	TP_State.TouchDetected = TP_State.X = TP_State.Y = TP_State.Z = 0;
}

static uint16_t IOE_TP_Read_X(void)
{
  int16_t x, xr;
  uint8_t tmp_x[2];

  /* Read x value from DATA_X register */
//  x = I2C_ReadDataBuffer(IOE_REG_TP_DATA_X);
  HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_TP_DATA_X, 1, &tmp_x, 2, 10);
//  printf("	x : %02x%02x\r\n", tmp_x[0],tmp_x[1]);

  x = tmp_x[0]<<8 | tmp_x[1];
  /* x value first correction */
  if(x <= 3000)
  {
  x = 3870 - x;
  }
  else
  {
   x = 3800 - x;
  }

  /* x value second correction */
  xr = x / 15;

  /* return x position value */
  if(xr <= 0)
  {
    xr = 0;
  }
  else if (xr > 240)
  {
    xr = 239;
  }
  else
  {}
  return (uint16_t)(xr);
}

/**
  * @brief  Return Touch Panel Y position value
  * @param  None
  * @retval Y position.
  */
static uint16_t IOE_TP_Read_Y(void)
{
  int16_t y, yr;
  uint8_t tmp_y[2];

  /* Read y value from DATA_Y register */
//  y = I2C_ReadDataBuffer(IOE_REG_TP_DATA_Y);
  HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_TP_DATA_Y, 1, &tmp_y, 2, 10);
//  printf("	y : %02x%02x\r\n", tmp_y[0],tmp_y[1]);

  y = tmp_y[0]<<8 | tmp_y[1];
  /* y value first correction */

  y -= 360;

  /* y value second correction */
  yr = y / 11;

  /* return y position value */
  if(yr <= 0)
  {
    yr = 0;
  }
  else if (yr > 320)
  {
    yr = 319;
  }
  else
  {}
  return (uint16_t)(yr);
}
static uint16_t IOE_TP_Read_Z(void)
{
  uint8_t z;

  /* Read z value from DATA_Z register */
//  z = I2C_ReadDataBuffer(IOE_REG_TP_DATA_Z);
  HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_TP_DATA_Z, 1, &z, 1, 10);
//  printf("	z : %d\r\n", z);
  /* return z position value */
  if(z <= 0)
    z = 0;
  return (uint16_t)(z);
}

TP_STATE* IOE_TP_GetState(void)
{
	uint32_t xDiff, yDiff , x , y;
	static uint32_t _x = 0, _y = 0;
	uint8_t tp_detect = 0;

	/* Check if the Touch detect event happened */
//	TP_State.TouchDetected = (I2C_ReadDeviceRegister(IOE_REG_TP_CTRL) & 0x80);
	HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_TP_CTRL, 1, &tp_detect, 1, 10);
//	HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
//	printf("tp_detecd : %02x\r\n", tp_detect);
	TP_State.TouchDetected = (tp_detect & 0x80);
	if(TP_State.TouchDetected)
	{
//		printf("	touch detected\r\n");
		x = IOE_TP_Read_X();
//		printf("	tp_x : %d\r\n", x);
		y = IOE_TP_Read_Y();
//		printf("	tp_y : %d\r\n", y);
		xDiff = x > _x? (x - _x): (_x - x);
	    yDiff = y > _y? (y - _y): (_y - y);
	    if (xDiff + yDiff > 5)
	    {
	    	_x = x;
	    	_y = y;
	    }
//	    printf("	_x[%d], _y[%d]\r\n", _x, _y);
	}
	else
	{
		_x = 0;
		_y = 0;
	}
	/* Update the X position */
	TP_State.X = _x;

	/* Update the Y position */
	TP_State.Y = _y;
	/* Update the Z Pression index */
	TP_State.Z = IOE_TP_Read_Z();

	/* Clear the interrupt pending bit and enable the FIFO again */
//	I2C_WriteDeviceRegister(IOE_REG_FIFO_STA, 0x01);
	uint8_t sta_data = 0x01;
//	uint8_t temp_data = 0xff;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_FIFO_STA, 1, &sta_data, 1, 10);
//	HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_FIFO_STA, 1, &temp_data, 1, 10);
//	printf("	==> sta_data[0x01] : %02x\r\n", temp_data);

//	I2C_WriteDeviceRegister(IOE_REG_FIFO_STA, 0x00);
	sta_data = 0x0;
	HAL_I2C_Mem_Write(&hi2c3, TP_ADDR, IOE_REG_FIFO_STA, 1, &sta_data, 1, 10);
//	HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_FIFO_STA, 1, &temp_data, 1, 10);
//	printf("	==> sta_data[0x00] : %02x\r\n", temp_data);

	/* Return pointer to the updated structure */
	return &TP_State;
}

void delay(uint32_t nCount)
{
  uint32_t index = 0;
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}
