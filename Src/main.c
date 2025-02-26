/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "liquidcrystal_i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DT_PIN GPIO_PIN_0   // Định nghĩa chân DT kết nối với GPIO_PIN_0
#define DT_PORT GPIOA       // Định nghĩa cổng DT kết nối với GPIOA
#define SCK_PIN GPIO_PIN_1  // Định nghĩa chân SCK kết nối với GPIO_PIN_1
#define SCK_PORT GPIOA      // Định nghĩa cổng SCK kết nối với GPIOA

#define SAMPLES_COUNT 50    // Số lượng mẫu để lọc trung bình
#define NOISE_MARGIN 200    // Biên độ nhiễu cho giá trị 0 (tăng lên)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;    // Cấu trúc điều khiển giao diện I2C

TIM_HandleTypeDef htim1;    // Cấu trúc điều khiển Timer

UART_HandleTypeDef huart2;  // Cấu trúc điều khiển UART

/* USER CODE BEGIN PV */
uint32_t tare = 9037565;    // Giá trị ADC gửi về STM khi không có gì trên cân
float knownOriginal = 50;   // Khối lượng tham chiếu 50 gram
float knownHX711 = 34009;   // Giá trị ADC tương ứng với 50g

int weight;                 // Biến lưu trữ khối lượng đo được
char weight_lcd[10];        // Chuỗi để hiển thị khối lượng lên LCD
uint32_t last_weight = 0;   // Biến lưu trữ giá trị trọng lượng trước đó
/* USER CODE END PV */

/* Private function prototypes ------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void microDelay(uint16_t delay)
{
  // Thiết lập bộ đếm của Timer về 0
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  // Đợi cho đến khi bộ đếm đạt đến giá trị delay
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

int32_t getHX711(void)
{
  uint32_t data = 0;            // Biến lưu trữ dữ liệu đọc được
  uint32_t startTime = HAL_GetTick();  // Lưu thời gian bắt đầu đọc dữ liệu
  // Đợi cho đến khi chân DT ở mức thấp (khoảng 200ms)
  while(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
  {
    if(HAL_GetTick() - startTime > 200)
      return 0;   // Nếu quá thời gian chờ, trả về 0
  }
  // Đọc 24 bit dữ liệu từ HX711
  for(int8_t len=0; len<24 ; len++)
{
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);   // Đặt chân SCK lên mức cao
    microDelay(1);   // Đợi 1 microsecond
    data = data << 1;   // Dịch bit sang trái
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET); // Đặt chân SCK về mức thấp
    microDelay(1);   // Đợi 1 microsecond
    // Kiểm tra chân DT và tăng giá trị data nếu DT ở mức cao
    if(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
      data ++;
  }
  data = data ^ 0x800000; // Thực hiện phép xor với 0x800000
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);     // Đặt chân SCK lên mức cao
  microDelay(1);   // Đợi 1 microsecond
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);   // Đặt chân SCK về mức thấp
  microDelay(1);   // Đợi 1 microsecond
  return data;     // Trả về giá trị data đọc được
}

int weigh()
{
  int32_t total = 0;   // Tổng giá trị đọc được
  int32_t samples = 30; // Số lượng mẫu đọc
  int gram;            // Khối lượng tính được
  float coefficient;   // Hệ số chuyển đổi
  for(uint16_t i=0 ; i<samples ; i++)
  {
      total += getHX711(); // Đọc giá trị từ HX711 và cộng vào tổng
  }
  int32_t average = (int32_t)(total / samples); // Tính giá trị trung bình
  coefficient = knownOriginal / knownHX711; // Tính hệ số chuyển đổi
  gram = (int)((average - tare) * coefficient); // Tính khối lượng từ giá trị trung bình và hệ số

  // Kiểm tra nếu trọng lượng nhỏ hơn ngưỡng nhiễu
  if (abs(average - tare) < NOISE_MARGIN)
  {
      gram = 0; // Đặt giá trị khối lượng về 0 nếu nhỏ hơn ngưỡng
  }

  return gram; // Trả về giá trị khối lượng
}
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HD44780_Init(2);  // Khởi tạo LCD với 2 dòng
  HD44780_Clear();  // Xóa màn hình LCD
  HAL_TIM_Base_Start(&htim1); // Khởi động Timer
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET); // Đặt chân SCK lên mức cao
  HAL_Delay(10);   // Đợi 10ms
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET); // Đặt chân SCK về mức thấp
  HAL_Delay(10);   // Đợi 10ms

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      weight = weigh(); // Đo khối lượng

      // Chỉ cập nhật LCD nếu giá trị thay đổi đáng kể
      if (weight != last_weight) {
          last_weight = weight; // Cập nhật giá trị khối lượng cuối cùng

          // Hiển thị khối lượng lên LCD
          HD44780_Clear(); // Xóa màn hình LCD
          HD44780_SetCursor(0, 0); // Đặt con trỏ tại dòng 0, cột 0
          HD44780_PrintStr("khoi luong = "); // In chuỗi lên LCD
          sprintf(weight_lcd, "%d g", weight); // Định dạng khối lượng vào chuỗi
          HD44780_SetCursor(0, 1); // Đặt con trỏ tại dòng 1, cột 0
          HD44780_PrintStr(weight_lcd); // In khối lượng lên LCD
      }

      HAL_Delay(500); // Giảm thời gian trễ để nhận giá trị nhanh hơn
    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  huart2.Init.BaudRate = 115200;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
