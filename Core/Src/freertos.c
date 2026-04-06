/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "queue.h"
#include "Serial.h"
#include "event_groups.h"
#include "Motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
QueueHandle_t serial_rx_queueBit;
QueueHandle_t Main_queue;
QueueHandle_t WIFI_Control_queue;
EventGroupHandle_t xEventGroup;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Serial_RX_Task(void* params);
void Main_Task(void* params);
void WIFI_Control_Task(void* params);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  if (serial_rx_queueBit == NULL) {
    OLED_WriteString(0, 0, "Queue Err");
    while(1); // 队列创建失败，停在这里
  }
  
  if (Main_queue == NULL) {
    OLED_WriteString(0, 0, "Main Queue Err");
    while(1);
  }
  Serial_Init();


  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  Main_queue = xQueueCreate(10, sizeof(Message_t));
  serial_rx_queueBit = xQueueCreate(64, sizeof(uint8_t)); // 创建一个长度为64的队列，每个元素大小为1字节
  WIFI_Control_queue = xQueueCreate(5, sizeof(Message_t));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  if (defaultTaskHandle == NULL) {
    OLED_WriteString(0, 0, "Default Task Err");
    while(1);
  }
  if (xTaskCreate(Serial_RX_Task, "SerialRx", 256, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
    OLED_WriteString(0, 0, "RX Task Err");
    while(1);
  }
  if (xTaskCreate(Main_Task, "MainTask", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
    OLED_WriteString(1, 0, "Main Task Err");
    while(1);
  }
  if (xTaskCreate(WIFI_Control_Task, "WIFIControl", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
    OLED_WriteString(2, 0, "WiFi Control Task Err");
    while(1);
  }
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  xEventGroup = xEventGroupCreate();
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Serial_RX_Task(void* params)
{
  uint8_t received_byte;
  uint8_t rx_index = 0;
  uint8_t frame_started = 0;
  Message_t xMessage;

  (void)params;
    while (1)
    {
        if (xQueueReceive(serial_rx_queueBit, &received_byte, portMAX_DELAY) == pdPASS)
        {
      if (received_byte == '@')
      {
        frame_started = 1;
        rx_index = 0;
        OLED_WriteString(1, 0, "Receiving...");
        continue;
      }

      if (frame_started == 0)
      {
        continue;
      }

      if (received_byte == '\r')
      {
        continue;
      }

      if (received_byte == '\n')
      {
        xMessage.ucData[rx_index] = '\0';
        if (rx_index > 0)
        {
          (void)xQueueSend(Main_queue, &xMessage, portMAX_DELAY);
          OLED_WriteString(2, 0, "Received Msg:");
        }
        frame_started = 0;
        rx_index = 0;
        continue;
      }

      if (rx_index < (SERIAL_MSG_MAX_LEN - 1U))
      {
        xMessage.ucData[rx_index++] = (char)received_byte;
      }
      else
      {
        frame_started = 0;
        rx_index = 0;
      }
        }
    }
}

void Main_Task(void* params)
{
  (void)params;
  Message_t xReceivedMessage;
  uint8_t bits=0;
  while (1)
  {
    if (xQueueReceive(Main_queue, &xReceivedMessage, portMAX_DELAY) == pdPASS)
    {
      OLED_WriteString(0, 0, "                      ");
      OLED_WriteString(0, 0, xReceivedMessage.ucData);
      if(strcmp(xReceivedMessage.ucData, "Exit") == 0){
        Motor_Stop();
        bits=0;
      }
      if(bits==1){
        (void)xQueueSend(WIFI_Control_queue, &xReceivedMessage, portMAX_DELAY);
      }
      if(strcmp(xReceivedMessage.ucData, "WifiControl") == 0){
        bits=1;
      }
    }
  }
}

void WIFI_Control_Task(void* params)
{
  (void)params;
  Message_t xReceivedMessage;
  while (1)
  {
    if (xQueueReceive(WIFI_Control_queue, &xReceivedMessage, portMAX_DELAY) == pdPASS)
    {
      // 在这里处理来自 Main_Task 的消息
      // OLED_WriteString(3, 0, "WiFi Control Msg:");
      // OLED_WriteString(4, 0, xReceivedMessage.ucData);
      // 根据消息内容执行相应的操作
      if(strcmp(xReceivedMessage.ucData, "Forward") == 0){
        Motor_Set(200, 200, 200, 200);
      }
      else if(strcmp(xReceivedMessage.ucData, "Backward") == 0){
        Motor_Set(-200, -200, -200, -200);
      }
      else if(strcmp(xReceivedMessage.ucData, "Left") == 0){
        Motor_Set(-100, 100, -100, 100);
      }
      else if(strcmp(xReceivedMessage.ucData, "Right") == 0){
        Motor_Set(100, -100, 100, -100);
      }
      else if(strcmp(xReceivedMessage.ucData, "Stop") == 0){
        Motor_Stop();
      }
    }
  }
}
/* USER CODE END Application */

