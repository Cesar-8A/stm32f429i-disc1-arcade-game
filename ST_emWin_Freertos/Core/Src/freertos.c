/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "Gyroscope_Utilities.h"
#include <math.h>
#include <stdlib.h>
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
/* USER CODE BEGIN Variables */
uint8_t SPI_DATA[6]; //X,Y,Z values
uint8_t SPI_ADDRS[3]; //X,Y,Z register Addresses
QueueHandle_t dataX;
int16_t DataX;
int16_t DataY;
int16_t DataZ;
fig_info figura;
uint8_t winner=0;
int16_t game_suspend=1;
int16_t start=1;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);

extern void GRAPHICS_MainTask(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
__weak void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
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
	dataX = xQueueCreate(1,sizeof(int));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityIdle, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityIdle, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
uint32_t color;
uint8_t check_collision(int16_t Xpos, int16_t Ypos){
	
	uint8_t collision = 0;	
	int x = -9, y = 0, err = 2-2*9, e2;	
	int i;
	for(i=9;i>6;i--){
		x=-i;y=0;err=2-2*i;e2=0;
		do {		
				color = GUI_GetPixelIndex(Xpos-x,Ypos+y);
				color = GUI_Index2Color(color);
				if(color == GUI_WHITE){
					collision = 1;
					break;
				}if(color == 0xFF848284){
					winner=1;
					break;
				}
				color = GUI_GetPixelIndex(Xpos+x,Ypos+y);			
				color = GUI_Index2Color(color);
				if(color == GUI_WHITE){
					collision = 1;
					break;
				}if(color == 0xFF848284){
					winner=1;
					break;
				}
				color = GUI_GetPixelIndex(Xpos+x,Ypos-y);			
				color = GUI_Index2Color(color);
				if(color == GUI_WHITE){
					collision = 1;
					break;
				}if(color == 0xFF848284){
					winner=1;
					break;
				}
				color = GUI_GetPixelIndex(Xpos-x,Ypos-y);			
				color = GUI_Index2Color(color);
				if(color == GUI_WHITE){
					collision = 1;
					break;
				}if(color == 0xFF848284){
					winner=1;
					break;
				}
				e2 = err;
				if (e2 <= y) {
						err += ++y*2+1;
						if (-x == y && e2 <= x) e2 = 0;
				}
				if (e2 > x) err += ++x*2+1;
				
		}
		while (x <= 0);			
	}
	return collision;
}

void redraw_ifNotCollided(void){
	uint8_t collision = 0;		
	figura.direction_x +=DataY/5;
	figura.direction_y +=DataX/5;

	
			
	if(!check_collision(figura.direction_x,figura.direction_y)){
		GUI_SetColor(GUI_BLACK);
		GUI_FillCircle(figura.current_x,figura.current_y,9);
		figura.current_x = figura.direction_x;
		figura.current_y =figura.direction_y;
		GUI_SetColor(GUI_BLUE);
		GUI_FillCircle(figura.current_x,figura.current_y,9);
	}else{
		GUI_SetColor(GUI_BLUE);
		GUI_FillCircle(figura.current_x,figura.current_y,9);
	}
	
	figura.direction_x = figura.current_x;
	figura.direction_y =figura.current_y;
}

void StartDefaultTask(void const * argument)
{
/* Graphic application */
  //GRAPHICS_MainTask();
//	GUI_MEMDEV_Handle background;
//	background = GUI_MEMDEV_Create(0,0,240,320);
	//GUI_MEMDEV_CreateAuto(&);
  /* USER CODE BEGIN StartDefaultTask */
/* Infinite loop */

	figura.current_x = 120;
	figura.current_y = 292;
	figura.direction_x = 120;
	figura.direction_y = 292;
	int i=0;
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	draw_map();
	while(start){
		GUI_SetFont(&GUI_Font20B_1);
		GUI_DispStringAt("Pulsa START para iniciar",0,150);
	}
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	draw_map();	
  for(;;)
  {
		if(game_suspend < 0){
			GUI_SetFont(&GUI_Font32B_1);
			GUI_DispStringAt("	PAUSA 	",60,150);
			osDelay(600);
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();
			draw_map();
		}else if(winner){
			GUI_SetFont(&GUI_Font20B_1);
			GUI_DispStringAt("	 GANASTE	",60,150);
			GUI_DispStringAt("Pulsa START",60,180);
			figura.current_x = 120;
			figura.current_y = 292;
			figura.direction_x = 120;
			figura.direction_y = 292;			
		}else{
			redraw_ifNotCollided();
			osDelay(56);
		}			
		
		
		//redraw_ifNotCollided();
		
//		GUI_GotoX(0);
//		GUI_GotoY(0);
//		uint32_t m;
//		m = GUI_GetPixelIndex(figura.current_x-10,figura.current_y);
//		m = GUI_Index2Color(m);
//		GUI_SetColor(GUI_BLUE);	
//		GUI_DispHexAt(m,80,80,8);
//		GUI_SetFont(&GUI_Font8x16);
//		GUI_GotoX(0);
//		GUI_GotoY(0);
//		GUI_DispString("X-Axis: ");
//		GUI_DispDecMin(DataX);
//		GUI_GotoX(0);
//		GUI_GotoY(10);
//		GUI_DispString("Y-Axis: ");
//		GUI_DispDecMin(DataY);
//		GUI_GotoX(0);
//		GUI_GotoY(20);
//		GUI_DispString("Z-Axis: ");
//		GUI_DispDecMin(DataZ);
		//GUI_MEMDEV_CopyToLCD(background);

    osDelay(30);
		i++;
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
	//HAL_SPI_RxCpltCallback();
  /* Infinite loop */
//	uint32_t collision_left,collision_right,collision_top,collision_bottom;
  for(;;)
  {
//		collision_left = GUI_GetPixelIndex(figura.current_x-10,figura.current_y);
//		collision_left = GUI_Index2Color(collision_left);
//		collision_right = GUI_GetPixelIndex(figura.current_x+10,figura.current_y);
//		collision_right = GUI_Index2Color(collision_right);
//		
//		collision_top = GUI_GetPixelIndex(figura.current_x,figura.current_y-10);
//		collision_top = GUI_Index2Color(collision_top);
//		collision_bottom = GUI_GetPixelIndex(figura.current_x,figura.current_y+10);
//		collision_bottom = GUI_Index2Color(collision_bottom);		
    osDelay(500);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */

void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
		BSP_LED_Toggle(LED4);
  }
	
	uint8_t CTRL_REG5_ADDRS = 0x24;
	uint8_t CTRL_REG5 = 0x1A;//0b00011010; //BOOT FIFO_EN -- HPen INT1_Sel1 INT1_Sel0 Out_Sel1 Out_Sel0
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);	//Start communucation
	HAL_SPI_Transmit(&hspi5, &CTRL_REG5_ADDRS, 1, 10);
	HAL_SPI_Transmit(&hspi5, &CTRL_REG5, 1, 10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	//End communucation		
	
	uint8_t CTRL_REG1_ADDRS = 0x20;
	uint8_t CTRL_REG1 = 0x0F;//0b00001111; // DR1 DR0 BW1 BW0 PD Zen Yen Xen
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);	//Start communucation
	HAL_SPI_Transmit(&hspi5, &CTRL_REG1_ADDRS, 1, 10);
	HAL_SPI_Transmit(&hspi5, &CTRL_REG1, 1, 10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	//End communucation		
	
	uint8_t CTRL_REG4_ADDRS = 0x23;
	uint8_t CTRL_REG4 = 0x30;//0b00110000; // 0 BLE FS1 FS0 - ST1 ST0 SIM
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);	//Start communucation
	HAL_SPI_Transmit(&hspi5, &CTRL_REG4_ADDRS, 1, 10);
	HAL_SPI_Transmit(&hspi5, &CTRL_REG4, 1, 10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	//End communucation		


	
	//extern unsigned char SPI_DATA[3]; //X,Y,Z values
	//extern unsigned char SPI_ADDRS[3]; //X,Y,Z register Addresses
	SPI_ADDRS[0] = 0xC0 |0x28; //XL address 0x28 | 0xC0 for reading and address upgrading
	int datax,datay,dataz;
	int16_t PrevDataX,PrevDataY,PrevDataZ;
  /* Infinite loop */
  for(;;)
  {
		//Escribir el Who am I con 158
		int i;
		datax=0;
		datay=0;
		dataz=0;
		for(i=0;i<5;i++){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);//Activate sensor
			HAL_SPI_Transmit(&hspi5, &SPI_ADDRS[0], 1, 10);
			HAL_SPI_Receive(&hspi5, SPI_DATA, 6, 10);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	 //End communucation		
			datax += (int16_t)((SPI_DATA[1]<<8) | SPI_DATA[0]);
			datay += (int16_t)((SPI_DATA[3]<<8) | SPI_DATA[2]);
			dataz += (int16_t)((SPI_DATA[5]<<8) | SPI_DATA[4]);
		}
		
		datax/=5;
		datay/=5;
		dataz/=5;
		DataX=datax*0.03;
		DataY=datay*0.03;
		DataZ=dataz*0.03;
		

		PrevDataX = DataX;
		PrevDataY = DataY;
		PrevDataZ = DataZ;
		BSP_LED_Toggle(LED3);
		
		osDelay(50);
  }
  /* USER CODE END StartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
