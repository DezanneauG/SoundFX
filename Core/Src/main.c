/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/*
 Library:				STM32F4 PX505_FX_Synthe - CS43L22
 Written by:				Tony DOMINGUEZ
 Date Written:			08/11/2020
 Last modified:			08/11/2020
 Description:			This is an STM32 device driver library for the CS43L22 Audio Codec, using STM HAL libraries
 References:
 1) Cirrus Logic CS43L22 datasheet
 https://www.mouser.com/ds/2/76/CS43L22_F2-1142121.pdf
 2) ST opensource CS43L22 Audio Codec dsp drivers.
 3) Sourcing by an internet youtube video
 * Copyright (C) 2020 - T.DOMINGUEZ
 Work in progress
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "My_CS43L22.h"
#include <math.h>

//Synths Functions
#include "Envelope.h"
#include "Oscillator.h"
#include "Operator.h"
#include "Mixer.h"
#include "Note.h"
#include "FM.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

#define PI 3.14159f
// Sample rate
#define F_SAMPLE	50000.0f
//Tunig frequency A4 = 440Hz
#define F_TUNING	440.0f
//Output Frequency (for test)
#define F_OUT		440.0f
//Output Gain set such that ears dont bleed
#define G_OUT		50.0f
#define TRANCHES_MAX 1

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t myDacValue;

uint16_t I2S_dummy[4];

/* GLOBAL INTERRUPTION FLAG */
int Exec_flg;
/* Global Note Value */
int Note_Val;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* Initialize LEDS */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_DAC_Init();
	MX_I2C1_Init();
	MX_I2S3_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */

	/* 			INITIALISATION CS43L22				*/
	// DAC
	CS43_Init(hi2c1, MODE_ANALOG); // configuration des registres du composant CS43L22 	: 	select mode ANALOG
	CS43_SetVolume(G_OUT);// configuration des registres du composant CS43L22 	: 	choose Volume level
	CS43_Enable_RightLeft(CS43_RIGHT_LEFT);	// configuration des registres du composant CS43L22 	: 	choose Audio mode == stereo
	CS43_Start();

	/* 			INITIALISATION I2S STM32				*/

	HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*) I2S_dummy, 4);

	/* 			INITIALISATION DAC STM32				*/

	HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);// Start DAC	:	Select DAC_CHANNEL_1

	/* 			INITIALISATION TIM2				*/

	HAL_TIM_Base_Start_IT(&htim2);		// Start DAC	:	Select DAC_CHANNEL_1

	/* USER CODE END 2 */

	//Init led temoin
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);		//BP
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);		//Init
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);		//Loop off
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);		//Loop on

	//Rend la phase d'init visible sur les leds
	HAL_Delay(500);

	//Init code Midi Notes to Hz (may take long to exec)
	//
	/*
	 int i;
	 float F;
	 midiNote_t Midi_Tab[127];
	 for (i = 0; i < 128; i++) {
	 F = pow(2, ((i - 69) / 12)) * F_TUNING;
	 Midi_Tab[i].frequency = F / F_SAMPLE;
	 Midi_Tab[i].period = F_SAMPLE / F;
	 }
	 */
	//
	//float sample_dt;
	//uint16_t sample_N;
	float frequency, frequency1, frequency2;
	int BP;

	//Synthesizer variables
	float OutputValue1, OutputValue2, OutputValue3;

	//Init Exec_flg
	Exec_flg = 0;

	//Init  Note_Val
	//Note_Val = 100;
	//Init DAC Value
	myDacValue = 0;

	//sample_dt = F_OUT / F_SAMPLE;
	//sample_N = F_SAMPLE / F_OUT;
	frequency = F_OUT / F_SAMPLE;
	frequency1 = frequency / 10;

	//Parameters and Memory for Operator1
	//Memoire
	OperatorMem_t OP1Mem;
	//Oscillator
	OP1Mem.CompteurOsc = 0;
	OP1Mem.Frequency = frequency1;
	//Envelope
	OP1Mem.CompteurEnv = 0;
	OP1Mem.Phase = OFF;
	OP1Mem.Note = 0;

	//Params
	OperatorParam_t OP1Param;
	//Oscillator
	OP1Param.WaveType = SINE;
	OP1Param.Alpha = 1;
	//Enveloppe
	OP1Param.Attack = 10000;
	OP1Param.Decay = 500;
	OP1Param.Sustain = 0.5;
	OP1Param.Release = 20000;

	//Parameters and Memory for Operator2
	//Memoire
	OperatorMem_t OP2Mem;
	//Oscillator
	OP2Mem.CompteurOsc = 0;
	OP2Mem.Frequency = frequency1;
	//Envelope
	OP2Mem.CompteurEnv = 0;
	OP2Mem.Phase = OFF;
	OP2Mem.Note = 0;

	//Params
	OperatorParam_t OP2Param;
	//Oscillator
	OP2Param.WaveType = SINE;
	OP2Param.Alpha = 0.9;
	//Enveloppe
	OP2Param.Attack = 100;
	OP2Param.Decay = 50;
	OP2Param.Sustain = 0.1;
	OP2Param.Release = 200;

	//Tranche Structures
	Tranche_t Tranches[TRANCHES_MAX];
	Tranches[0].Gain = 0.9;

	//Init end led toggle
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);

	while (1) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	//Loop
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);	//Loop

		//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/*
		 HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
		 HAL_Delay(1000);
		 */
		// Read button push state
		BP = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, BP);

		if (Exec_flg == 1) {
			//RaZ Exec_flg
			Exec_flg = 0;

			//		2 Operator Code		//
			//Code a exec
			//frequency = Midi_Tab[Note_Val].frequency;

			/*
			OutputValue1 = Operator(BP, frequency1, OP1Param, &OP1Mem);

			frequency2 = FM(OutputValue1, frequency, 1);

			OutputValue2 = Operator(BP, frequency2, OP2Param, &OP2Mem);

			Tranches[0].Value = OutputValue2;

			OutputValue3 = Mixer(Tranches, TRANCHES_MAX);

			*/
			//		1 Operator Sine Code 	//

			//Parameters and Memory for Operator1
			//Params
			OperatorParam_t OP1Param;
			//Oscillator
			OP1Param.WaveType = SQUARE;
			OP1Param.Alpha = 0.5;
			//Enveloppe
			OP1Param.Attack = 10;
			OP1Param.Decay = 10;
			OP1Param.Sustain = 1;
			OP1Param.Release = 10;
			OutputValue3 = Operator(BP, frequency, OP1Param, &OP1Mem);

			// conversion float to decimal pour le DAC (prise en compte next interruption)
			//1/50000 Hz de retard de phase (négligeable)
			myDacValue = (OutputValue3 + 1) * 127;
		}

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 50;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief DAC Initialization Function
 * @param None
 * @retval None
 */
static void MX_DAC_Init(void) {

	/* USER CODE BEGIN DAC_Init 0 */

	/* USER CODE END DAC_Init 0 */

	DAC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN DAC_Init 1 */

	/* USER CODE END DAC_Init 1 */
	/** DAC Initialization
	 */
	hdac.Instance = DAC;
	if (HAL_DAC_Init(&hdac) != HAL_OK) {
		Error_Handler();
	}
	/** DAC channel OUT1 config
	 */
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN DAC_Init 2 */

	/* USER CODE END DAC_Init 2 */

}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief I2S3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S3_Init(void) {

	/* USER CODE BEGIN I2S3_Init 0 */

	/* USER CODE END I2S3_Init 0 */

	/* USER CODE BEGIN I2S3_Init 1 */

	/* USER CODE END I2S3_Init 1 */
	hi2s3.Instance = SPI3;
	hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
	hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
	hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	hi2s3.Init.CPOL = I2S_CPOL_LOW;
	hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2S3_Init 2 */

	/* USER CODE END I2S3_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 84 - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 20 - 1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD,
	GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_4,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : PD12 PD13 PD14 PD15
	 PD4 */
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
			| GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	UNUSED(htim);

	// build sine function for the audio sound
	if (htim->Instance == TIM2) {

		//Output the sample to the SIMDAC
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_R, myDacValue);

		//Mise a 1 Exec_flg
		Exec_flg = 1;
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
