/*Base register adddress header file*/
#include "stm32l1xx.h"
/*Library related header files*/
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"

void SystemClock_Config(void);
void GPIO_Config(void);
void TIMBase_Config(void);
void showCount(uint8_t c);
void reset7Seg();

#define ON 1
#define OFF 0

uint8_t i;
uint8_t disp[3];
uint8_t Green = ON;
uint8_t Yellow = OFF;
uint8_t Red = OFF;

uint32_t seg[10]={LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14,
									LL_GPIO_PIN_10 | LL_GPIO_PIN_11,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_15 | LL_GPIO_PIN_13 | LL_GPIO_PIN_12,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_15 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12,
									LL_GPIO_PIN_14 | LL_GPIO_PIN_15 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15 | LL_GPIO_PIN_13 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,
									LL_GPIO_PIN_2 | LL_GPIO_PIN_14 | LL_GPIO_PIN_10 | LL_GPIO_PIN_15 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12};

uint32_t digit[4] = {LL_GPIO_PIN_0 , LL_GPIO_PIN_1 , LL_GPIO_PIN_2 , LL_GPIO_PIN_3};

int main()
{
	SystemClock_Config();
	GPIO_Config();
	TIMBase_Config();
	
	while(1)
	{
		if (Green == ON)
		{
			// Green LED ON
			while(LL_GPIO_IsInputPinSet(GPIOD, LL_GPIO_PIN_2));
			Green = OFF;
			Yellow = ON;
			i = 6;
		}
		
		if (Yellow == ON)
		{
			// Yellow LED ON
			showCount(i-1);
			if (i == 6)
			{
				LL_TIM_EnableCounter(TIM2);
			}
			if(i == 0)
			{
				Yellow = OFF;
				Red = ON;
				i = 61;
			}
		}
		
		if (Red == ON)
		{
			// Red LED ON
			showCount(i-1);
			if(i == 0)
			{
				LL_TIM_DisableCounter(TIM2);
				reset7Seg();
				Red = OFF;
				Green = ON;
			}
		}
	}
}

void showCount(uint8_t c)
{
	for(uint8_t d = 0 ; d<4 ; ++d)
	{
		reset7Seg();
		if(d==0)
		{
			
		}
		else if(d==1)
		{
			if(c/100 != 0)
			{
				LL_GPIO_SetOutputPin(GPIOB, seg[c/100]);
				LL_GPIO_SetOutputPin(GPIOC, digit[d]);
			}
		}
		else if(d==2)
		{
			if(!(c/10%10 == 0 && c/100 == 0))
			{
				LL_GPIO_SetOutputPin(GPIOB, seg[c/10%10]);
				LL_GPIO_SetOutputPin(GPIOC, digit[d]);
			}
		}
		else if(d==3)
		{
			LL_GPIO_SetOutputPin(GPIOB, seg[c%10]);
			LL_GPIO_SetOutputPin(GPIOC, digit[d]);
		}
	}
}

void reset7Seg()
{
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15);
}

void GPIO_Config()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	
	LL_GPIO_InitTypeDef GPIO_Init;
	LL_GPIO_InitTypeDef ltc4727_initstruct;
	
	GPIO_Init.Mode = LL_GPIO_MODE_INPUT;
	GPIO_Init.Pin = LL_GPIO_PIN_2;
	GPIO_Init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOD, &GPIO_Init);
	
	GPIO_Init.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_Init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_Init.Pin = LL_GPIO_PIN_6;
	GPIO_Init.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_Init);
	
	ltc4727_initstruct.Mode = LL_GPIO_MODE_OUTPUT;
	ltc4727_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ltc4727_initstruct.Pull = LL_GPIO_PULL_NO;
	ltc4727_initstruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	ltc4727_initstruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;

	LL_GPIO_Init(GPIOB, &ltc4727_initstruct);

	ltc4727_initstruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &ltc4727_initstruct);
}

void TIMBase_Config(void)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = 1000-1;
	timbase_initstructure.Prescaler = 32000-1;
	
	LL_TIM_Init(TIM2, &timbase_initstructure);
	
	LL_TIM_EnableIT_UPDATE(TIM2);
	
	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	if ((LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET))
	{
		LL_TIM_ClearFlag_UPDATE(TIM2);
		i--;
	}
}

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */
  LL_FLASH_Enable64bitAccess();;
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
 
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
 
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
 

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
 
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
 
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
 
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}
