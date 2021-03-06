/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/******************************************************************************
 * The includes for this project are:
 * math.h :-> for floor, ceil, and cos functions
 * string.h :-> for string conversion and splits
 * stdio.h :-> for general purposes and data types
 * stdlib.h :-> for general purposes and data types
 ******************************************************************************/
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/******************************************************************************
 * The structs for this project are:
 * LEDBehavior :-> used to store data about the wave function to be feed in the
 * respective LED.
 ******************************************************************************/
typedef struct
{
  int samples;
  int phase;
  int maximum;
  int shape[1000];
} LEDBehavior;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/******************************************************************************
 * The handlers for this project are:
 * hi2c1 :-> not used
 * hi2s3 :-> not used
 * hspi1 :-> not used
 * htim4 :-> used for the PWM output in the LED GPIO pins
 * huart2 :-> used for communication with the ESP32
 ******************************************************************************/
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/******************************************************************************
 * The includes for this project are:
 * configString :-> string that holds the configuration string to be read and
 * applied to the LED's wave functions
 * yellow, orange, red, blue :-> the variables that hold the wave functions
 * parameters for each LED.
 ******************************************************************************/

uint8_t configString[] = "Y-0500-0125-S@O-0500-0250-S@R-0500-0375-S@B-0500-0000-S";

LEDBehavior yellow, orange, red, blue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART2_UART_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
/******************************************************************************
 * The functions for this project are:
 * DeserializeConfiguration :-> gets a string and splits in blocks of
 * parameters. Then, for the specified LED, it sets the correct value for
 * each field in the wave form.
 * WaveSetup :-> Gets the received parameters and passes to the LED structures
 * GenerateHigh :-> generates a constant high level signal to the respective
 * wave parameters
 * GenerateLow :-> generates a constant low level signal to the respective
 * wave parameters
 * GenerateRamp :-> generates a ramp wave signal to the respective wave
 * parameters
 * GenerateCossine :-> generates a cosine wave signal to the respective wave
 * parameters
 * GenerateSquare :-> generates a square wave signal to the respective wave
 * parameters
 * GenerateParabola :-> generates a parabola wave signal to the respective wave
 * parameters
 ******************************************************************************/
void DeserializeConfiguration(uint8_t *configurationString, LEDBehavior *yellow, LEDBehavior *orange, LEDBehavior *red, LEDBehavior *blue);
void WaveSetup(LEDBehavior led, char *samples, char *phase, int maximum, char *shape);
void GenerateHigh(int *fixed, int samples, int maximum);
void GenerateLow(int *fixed, int samples, int maximum);
void GenerateRamp(int *ramp, int samples, int maximum);
void GenerateCossine(int *senoid, int samples, int maximum);
void GenerateSquare(int *square, int samples, int maximum);
void GenerateParabola(int *square, int samples, int maximum);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_I2C1_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_USB_HOST_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Initializes all PWM for all LEDs
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // Calls configuration function for default initialization setup
  DeserializeConfiguration(configString, &yellow, &orange, &red, &blue);

  // Variables responsible for phase delay in wave functions
  int yellowPhase, orangePhase, redPhase, bluePhase;
  int globalCounter = 0;

  // Setup interrupt reception for UART1 port
  // (Calls for handling after reception is completed)
  HAL_UART_Receive_IT(&huart2, configString, 55);

  while (1)
  {
	// Counter to keep general samples to be split among phases
	globalCounter++;

	// Phase for each wave considering the respective phase delays for each function
	yellowPhase = floor((globalCounter+yellow.phase)%yellow.samples);
	orangePhase = floor((globalCounter+orange.phase)%orange.samples);
	redPhase = floor((globalCounter+red.phase)%red.samples);
	bluePhase = floor((globalCounter+blue.phase)%blue.samples);

	// Setting up each PWM level for each LED
	TIM4->CCR1 = yellow.shape[yellowPhase];
	TIM4->CCR2 = orange.shape[orangePhase];
	TIM4->CCR3 = red.shape[redPhase];
	TIM4->CCR4 = blue.shape[bluePhase];

	// Setup the pace for the execution
	HAL_Delay(5);
	HAL_UART_Receive_IT(&huart2, configString, 55);

    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

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
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
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
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 8;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Audio_RST_Pin */
  GPIO_InitStruct.Pin = Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Audio_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// Called by the end of UART interruption. Deserializes the received string
// and resets reception again
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (configString[0]!='\0')
	{
		// Calls configuration function for default initialization setup for
		// the received string
		DeserializeConfiguration(configString, &yellow, &orange, &red, &blue);
		// Setup reception
		HAL_UART_Receive_IT(&huart2, configString, 55);
	}
}

// Handles configuration string and passes the parameters for
// the wave generator to the respective LEDs
void DeserializeConfiguration(uint8_t *configurationString, LEDBehavior *yellow, LEDBehavior *orange, LEDBehavior *red, LEDBehavior *blue){
	// Variables to hold the parameters while the string is being parsed
	char* ledConfiguration[4];
	char* configurationParameters[4];

	// Tokens for the string split
	char* token = strtok((char *)configurationString, "@");
	ledConfiguration[0] = token;

	// Splitting the string
	for(int index = 1; token != NULL; index++){
		token = strtok(NULL, "@");
		ledConfiguration[index] = token;
	}

	// Splitting the string and passing the parameters to the function
	// that setups the values in the LED structure.
	for(int index = 0; index < 4; index++){
		token = strtok(ledConfiguration[index], "-");
		configurationParameters[0] = token;

		// Splitting the string
		for(int indexIn = 1; token != NULL; indexIn++){
			token = strtok(NULL, "-");
			configurationParameters[indexIn] = token;
		}
		// Yellow LED
		if (*configurationParameters[0] == 'Y'){
			yellow->samples = atoi(configurationParameters[1]);
			yellow->phase = atoi(configurationParameters[2]);
			yellow->maximum = (65535/512);
			if (*configurationParameters[3] == 'C'){
				GenerateCossine(yellow->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'H'){
				GenerateHigh(yellow->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'L'){
				GenerateLow(yellow->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'P'){
				GenerateParabola(yellow->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'R'){
				GenerateRamp(yellow->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'S'){
				GenerateSquare(yellow->shape, atoi(configurationParameters[1]), (65535/512));
			}
		}
		// Orange LED
		else if (*configurationParameters[0] == 'O'){
			orange->samples = atoi(configurationParameters[1]);
			orange->phase = atoi(configurationParameters[2]);
			orange->maximum = (65535/512);
			if (*configurationParameters[3] == 'C'){
				GenerateCossine(orange->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'H'){
				GenerateHigh(orange->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'L'){
				GenerateLow(orange->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'P'){
				GenerateParabola(orange->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'R'){
				GenerateRamp(orange->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'S'){
				GenerateSquare(orange->shape, atoi(configurationParameters[1]), (65535/512));
			}
		}
		// Red LED
		else if (*configurationParameters[0] == 'R'){
			red->samples = atoi(configurationParameters[1]);
			red->phase = atoi(configurationParameters[2]);
			red->maximum = (65535/512);
			if (*configurationParameters[3] == 'C'){
				GenerateCossine(red->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'H'){
				GenerateHigh(red->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'L'){
				GenerateLow(red->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'P'){
				GenerateParabola(red->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'R'){
				GenerateRamp(red->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'S'){
				GenerateSquare(red->shape, atoi(configurationParameters[1]), (65535/512));
			}
		}
		// Blue LED
		else if (*configurationParameters[0] == 'B'){
			blue->samples = atoi(configurationParameters[1]);
			blue->phase = atoi(configurationParameters[2]);
			blue->maximum = (65535/512);
			if (*configurationParameters[3] == 'C'){
				GenerateCossine(blue->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'H'){
				GenerateHigh(blue->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'L'){
				GenerateLow(blue->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'P'){
				GenerateParabola(blue->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'R'){
				GenerateRamp(blue->shape, atoi(configurationParameters[1]), (65535/512));
			}
			else if (*configurationParameters[3] == 'S'){
				GenerateSquare(blue->shape, atoi(configurationParameters[1]), (65535/512));
			}
		}
	}
}

// Generates cosine wave according to the received parameters
void GenerateCossine(int *cosinusoid, int samples, int maximum){
	float theta;
	float phases = 6.28/samples;
	for (int i = 0; i < samples; i++){
		theta = phases * i;
		cosinusoid[i] = floor(maximum*(cos(theta)+1));
	}
}

// Generates constant high according to the received parameters
void GenerateHigh(int *fixed, int samples, int maximum){
	for (int i = 0; i < samples; i++){
		fixed[i] = floor(maximum);
	}
}

// Generates constant low according to the received parameters
void GenerateLow(int *fixed, int samples, int maximum){
	for (int i = 0; i < samples; i++){
		fixed[i] = 0;
	}
}

// Generates ramp wave according to the received parameters
void GenerateRamp(int *ramp, int samples, int maximum){
	float theta = maximum/(float)samples;
	for (int i = 0; i < samples; i++){
		ramp[i] = floor(i*theta);
	}
}

// Generates square wave according to the received parameters
void GenerateSquare(int *square, int samples, int maximum){
	for (int i = 0; i < floor(samples/2); i++){
		square[i] = maximum;
	}
	for (int i = floor(samples/2); i < samples; i++){
		square[i] = 0;
	}
}

// Generates parabola wave according to the received parameters
void GenerateParabola(int *parabola, int samples, int maximum){
	float a = (4*maximum)/(pow(samples,2));
	float b = -(4*maximum)/(float)samples;
	float c = maximum;
	for (int i = 0; i < samples; i++){
		parabola[i] = floor(a*pow(i,2) + b*i + c);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
