/**
  ******************************************************************************
  * @file    BSP/Src/mems.c 
  * @author  MCD Application Team
  * @brief   This example code shows how to use MEMS features.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mems.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t UserPressButton;

/* Variable used to replay audio sample (from play or record test)*/
extern uint32_t AudioTest; // *MR*
/* Variables used in normal mode to manage audio file during DMA transfer  *MR*  */
extern uint32_t AudioTotalSize; /* This variable holds the total size of the audio file */
extern uint32_t AudioRemSize;   /* This variable holds the remaining data in audio file */
extern uint16_t *CurrentPos;   /* This variable holds the current position of audio pointer */
extern uint16_t WrBuffer[WR_BUFFER_SIZE];

/* Init af threshold to detect acceleration on MEMS */
int16_t ThresholdHigh = 1000;
int16_t ThresholdLow = -1000;

uint32_t alarm_trigger = 0;
uint32_t trigger_neg_x = 0;
uint32_t trigger_pos_x = 0;
uint32_t trigger_neg_y = 0;
uint32_t trigger_pos_y = 0;

/* Private function prototypes -----------------------------------------------*/
static void ACCELERO_ReadAcc(void);
// static void GYRO_ReadAng(void);  DELETED GYRO SECTION *MR*

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Test ACCELERATOR MEMS Hardware.
  *         The main objective of this test is to check acceleration on 2 axes X and Y
  * @param  None
  * @retval None
  */
void ACCELERO_MEMS_Test(void)
{
  /* Init Accelerometer MEMS */
  if(BSP_ACCELERO_Init() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
  // ADD PLAY FROM MEMORY *MR*
  /* Play in the loop the recorded file */

  /* Set variable to indicate play from record buffer */
  AudioTest = 1;

  /* Set variable used to stop player before starting */
  UserPressButton = 0;

  /* Initialize audio IN at REC_FREQ */
  BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 70, DEFAULT_AUDIO_IN_FREQ);

  /* Set the total number of data to be played */
  AudioTotalSize = AUDIODATA_SIZE * WR_BUFFER_SIZE;
  /* Update the remaining number of data to be played */
  AudioRemSize = 0;
  /* Update the WrBuffer audio pointer position */
  CurrentPos = (uint16_t *)(WrBuffer);

  /* Play the recorded buffer */
//  BSP_AUDIO_OUT_Play(WrBuffer , AudioTotalSize);
  
  UserPressButton = 0;
  while(!UserPressButton && !alarm_trigger)
  {
    ACCELERO_ReadAcc();
  }

  /* Play the recorded buffer */
  BSP_AUDIO_OUT_Play(WrBuffer , AudioTotalSize);
  UserPressButton = 0;
  while(!UserPressButton && alarm_trigger)
  {
  }

  alarm_trigger = 0;
  trigger_neg_x = 0;
  trigger_pos_x = 0;
  trigger_neg_y = 0;
  trigger_pos_y = 0;

// *MR*
  /* Stop Player before close Test */
  if (BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW) != AUDIO_OK)
  {
    /* Audio Stop error */
    Error_Handler();
  }
// *MR*
}  

/**
  * @brief  Read Acceleration data.
  * @param  None
  * @retval None
  */
static void ACCELERO_ReadAcc(void)
{
  int16_t buffer[3] = {0};
  int16_t xval, yval = 0x00;
  
  /* Read Acceleration */
  BSP_ACCELERO_GetXYZ(buffer);

  xval = buffer[0];
  yval = buffer[1];

  if((ABS(xval))>(ABS(yval)))
  {
    if(xval > ThresholdHigh)
    { 
      /* LED5 On */
      BSP_LED_On(LED5);
      HAL_Delay(10);
      trigger_pos_x = 1;
    }
    else if(xval < ThresholdLow)
    { 
      /* LED4 On */
      BSP_LED_On(LED4);      
      HAL_Delay(10);
      trigger_neg_x = 1;
    }
    else
    { 
      HAL_Delay(10);
    }
  }
  else
  {
    if(yval < ThresholdLow)
    {
      /* LED6 On */
      BSP_LED_On(LED6);
      HAL_Delay(10);
      trigger_neg_y = 1;
    }
    else if(yval > ThresholdHigh)
    {
      /* LED3 On */
      BSP_LED_On(LED3);
      HAL_Delay(10);
      trigger_pos_y = 1;
    } 
    else
    { 
      HAL_Delay(10);
    }
  } 
  if(trigger_neg_x == 1 && trigger_pos_x == 1 && trigger_neg_y == 1 && trigger_pos_y == 1)
  {
	  alarm_trigger = 1;
  }

  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
  BSP_LED_Off(LED5);
  BSP_LED_Off(LED6);
}
//  DELETED GYRO SECTION *MR*


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
