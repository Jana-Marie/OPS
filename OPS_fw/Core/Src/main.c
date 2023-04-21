#include "main.h"
#include "MP8862.h"
//#include "usbd_cdc_if.h"
//#include "usb_device.h"
//#include "scpi/scpi.h"
//#include "scpi-def.h"

ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);

static uint32_t adc[8];
struct MP8862_t MP8862;
uint8_t adc_ready_flag = 0;

struct ops ops;
uint8_t addr[128];    

//extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
//extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
//uint8_t is_new_data_ready;
//uint16_t new_data_length;

//uint16_t sine[60] = {0x251c,0x28fd,0x2cd3,0x3094,0x3434,0x37aa,0x3aec,0x3df1,0x40b0,0x4322,0x453f,0x4703,0x4867,0x4968,0x4a04,0x4a38,0x4a04,0x4968,0x4867,0x4703,0x453f,0x4322,0x40b0,0x3df1,0x3aec,0x37aa,0x3434,0x3094,0x2cd3,0x28fd,0x251c,0x213b,0x1d65,0x19a4,0x1604,0x128e,0xf4c,0xc47,0x988,0x716,0x4f9,0x335,0x1d1,0xd0,0x34,0x0,0x34,0xd0,0x1d1,0x335,0x4f9,0x716,0x988,0xc47,0xf4c,0x128e,0x1604,0x19a4,0x1d65,0x213b};
uint16_t sine[60] = {4500,4970,5436,5891,6330,6750,7145,7511,7844,8141,8397,8611,8780,8902,8975,9000,8975,8902,8780,8611,8397,8141,7844,7511,7145,6750,6330,5891,5436,4970,4500,4030,3564,3109,2670,2250,1855,1489,1156,859,603,389,220,98,25,0,25,98,220,389,603,859,1156,1489,1855,2250,2670,3109,3564,4030};


uint8_t reg1,reg2,go,stat,vlo,vhi,ir;

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  //MX_USB_DEVICE_Init();

  HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin, 1);
  HAL_GPIO_WritePin(GPIOB, LED_RED_Pin, 1);

  HAL_Delay(100);


  MP8862_init(&MP8862, &hi2c2, MP8862_ADDR_0x69);
  MP8862_setVoltageSetpoint_mV(&MP8862, 400);
  MP8862_setCurrentLimit_mA(&MP8862, 6000);
  HAL_Delay(50);
  uint8_t isReady = MP8862_setEnable(&MP8862, 1);

  if(isReady){
    HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin, 0);
  } else {
    HAL_GPIO_WritePin(GPIOB, LED_RED_Pin, 0);
    HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin, 0);
  }

  HAL_ADC_Start_DMA(&hadc, adc, 8);
  HAL_TIM_Base_Start_IT(&htim2);

  //SCPI_Init(&scpi_context,
  //  scpi_commands,
  //  &scpi_interface,
  //  scpi_units_def,
  //  SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
  //  scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
  //  scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

  while (1)
  {

    //HAL_Delay(25);
    //MP8862_read(&MP8862, MP8862_REG_CTL1 , &reg1, 1 );
    //MP8862_read(&MP8862, MP8862_REG_CTL2 , &reg2, 1 );
    //MP8862_read(&MP8862, MP8862_REG_VOUT_GO , &go, 1 );
    //MP8862_read(&MP8862, MP8862_REG_STATUS , &stat, 1 );
    //MP8862_read(&MP8862, MP8862_REG_VOUT_L , &vlo, 1 );
    //MP8862_read(&MP8862, MP8862_REG_VOUT_H , &vhi, 1 );
    //MP8862_readCurrentLimit_mA(&MP8862, &ir);
    //MP8862_read(&MP8862, MP8862_REG_INTERRUPT , &ir, 1 );
    //MP8862_write(&MP8862, MP8862_REG_INTERRUPT , 0xff, 1 );
    //char _err[150] = {0};
    //sprintf(_err, "DBG\tR1=%d\tR2=%d\tgo=%d\tstat=%d\tlo=%d\thi=%d\tir=%d\n\r", reg1,reg2,go,stat,vlo,vhi,ir);
    //CDC_Transmit_FS(_err, 150);

    // Sine-Ramp for testing
    for(uint8_t i = 0; i < 60; i++){
      MP8862_setVoltageSetpoint_mV(&MP8862, sine[i]);
      HAL_GPIO_TogglePin(GPIOB, LED_ACT_Pin);
      HAL_Delay(25);
    }

    if (adc_ready_flag) {
      ops.vbus = adc[2]/4095.0*3.3*7.471;
      ops.vout = adc[1]/4095.0*3.3*7.471;
      ops.iout = (adc[0]/4095.0*3.3*1.659)/(0.01*(2370/33));
      ops.tref = (((adc[4]/4095.0)*3.3)-0.5)/0.01;
      ops.tc   = ((adc[3]-120)*92)/1000+ops.tref;

    }
      for (uint8_t i=1; i<=127; i++) {
    HAL_StatusTypeDef result;
    result = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(i<<1), 4, 4);
    if (result == HAL_OK) {
      // If in this scope, i == device on bus, do whatever you want or need
      addr[i] = 1;
      //print(i);
    }
    HAL_Delay(1);
  }


    //if (is_new_data_ready) {
    //  HAL_GPIO_WritePin(GPIOB, LED_ACT_Pin, 1);
    //  CDC_Transmit_FS(UserRxBufferFS, new_data_length);
    //  SCPI_Input(&scpi_context, UserRxBufferFS, new_data_length);
    //  is_new_data_ready = 0;
    //}
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

static void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  HAL_ADC_Init(&hadc);

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_1;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_2;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_3;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_4;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_6;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  sConfig.Channel = ADC_CHANNEL_7;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);
}

static void MX_I2C2_Init(void)
{
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x2010091A;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c2);
  HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE);
  HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0);
}

static void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 16;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1024;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);
}

static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 9999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 479;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}

static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  HAL_NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, OUT_EN_1_Pin|OUT_EN_2_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, LED_GREEN_Pin|LED_RED_Pin|LED_ACT_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = INT_STUSB_Pin|INT_MP_1_Pin|INT_MP_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = OUT_EN_1_Pin|OUT_EN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_GREEN_Pin|LED_RED_Pin|LED_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
}
