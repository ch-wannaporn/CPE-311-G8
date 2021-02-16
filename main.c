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

#define ON 1
#define OFF 0

#define E_O6					(uint16_t)1318
#define MUTE					(uint16_t) 2

/*for 10ms update event*/
#define TIMx_PSC			2 

/*Macro function for ARR calculation*/
#define ARR_CALCULATE(N) ((32000000) / ((TIMx_PSC) * (N)))

uint8_t i;
uint8_t disp[3];
uint8_t Green = OFF;
uint8_t Yellow = OFF;
uint8_t Red = ON;
uint8_t input = 1;

uint16_t rise_timestamp = 0;
uint16_t fall_timestamp = 0;
uint16_t fall = 0;
uint16_t rise = 0;
uint16_t downtime = 0;
uint16_t ptime = 0;

uint8_t state = 0;
float period = 0;
uint32_t TIM3CLK;
uint32_t PSC;
uint32_t IC1PSC;

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

void SystemClock_Config(void);
void GPIO_Config(void);
void TIMBase_Config(void);
void TIM_BASE_NOTE_Config(void);
void TIM_OC_Config(void);
void showCount(uint8_t c);
void reset7Seg(void);
void setSpeaker(void);
void TIM_IC_Config(void);
void setInput(void);

int main()
{
	SystemClock_Config();
	GPIO_Config();
	TIMBase_Config();
	TIM_OC_Config();
	TIM_IC_Config();
	
	while(1)
	{
		setInput();
		
		if (Red == ON)
		{
			LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_12);
			if(!input)
			{
				Red = OFF;
				Yellow = ON;
				i = 6;
			}
		}
		
		if (Yellow == ON)
		{
			LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_11);
			showCount(i-1);
			setSpeaker();
			if (i == 6)
			{
				LL_TIM_EnableCounter(TIM2);
			}
			if(i == 0)
			{
				Yellow = OFF;
				Green = ON;
				i = 61;
				
			}
		}
		
		if (Green == ON)
		{
			LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_12);
			if (i<=10)
			{
				if (LL_TIM_GetCounter(TIM2)<500)
					LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_11);
				else
					LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_11);
			}
			showCount(i-1);
			setSpeaker();
			if(i == 0)
			{
				LL_TIM_DisableCounter(TIM2);
				LL_TIM_DisableCounter(TIM4);
				reset7Seg();
				Green = OFF;
				Red = ON;
				LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12);
				i = 6;
				input = 1;
			}
		}
	}
}

void setInput(void){
		if(i==0)
		{
			LL_TIM_DisableCounter(TIM2);
			input = 0;
		}
}

void setSpeaker(void){
	if(LL_TIM_GetCounter(TIM2)<500)
		{
			LL_TIM_EnableCounter(TIM4);
		}
	else
		{
			LL_TIM_DisableCounter(TIM4);
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

void reset7Seg(void)
{
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15);
}

void GPIO_Config(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	
	LL_GPIO_InitTypeDef GPIO_Init;
	LL_GPIO_InitTypeDef ltc4727_initstruct;
	LL_GPIO_InitTypeDef rgb_led;
	
	GPIO_Init.Mode = LL_GPIO_MODE_INPUT;
	GPIO_Init.Pin = LL_GPIO_PIN_2;
	GPIO_Init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOD, &GPIO_Init);
	
	ltc4727_initstruct.Mode = LL_GPIO_MODE_OUTPUT;
	ltc4727_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ltc4727_initstruct.Pull = LL_GPIO_PULL_NO;
	ltc4727_initstruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	ltc4727_initstruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOB, &ltc4727_initstruct);

	ltc4727_initstruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &ltc4727_initstruct);
	
	rgb_led.Mode = LL_GPIO_MODE_OUTPUT;
	rgb_led.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	rgb_led.Pin = LL_GPIO_PIN_10 |LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
	rgb_led.Pull = LL_GPIO_PULL_NO;
	rgb_led.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOC, &rgb_led);
	
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12);
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

void TIM_BASE_NOTE_Config(void)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	//Time-base configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = ARR_CALCULATE(E_O6) - 1;
	timbase_initstructure.Prescaler =  TIMx_PSC - 1;
	LL_TIM_Init(TIM4, &timbase_initstructure);
}

void TIM_OC_Config(void)
{
	TIM_BASE_NOTE_Config();
	
	LL_GPIO_InitTypeDef gpio_initstructure;
	LL_TIM_OC_InitTypeDef tim_oc_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_initstructure.Alternate = LL_GPIO_AF_2;
	gpio_initstructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstructure.Pin = LL_GPIO_PIN_6;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstructure);
	
	tim_oc_initstructure.OCState = LL_TIM_OCSTATE_DISABLE;
	tim_oc_initstructure.OCMode = LL_TIM_OCMODE_PWM1;
	tim_oc_initstructure.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	tim_oc_initstructure.CompareValue = LL_TIM_GetAutoReload(TIM4) / 2;
	LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH1, &tim_oc_initstructure);
	/*Interrupt Configure*/
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_EnableIRQ(TIM4_IRQn);
	LL_TIM_EnableIT_CC1(TIM4);
	
	/*Start Output Compare in PWM Mode*/
	LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1);
}

void TIM4_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_CC1(TIM4) == SET)
	{
		LL_TIM_ClearFlag_CC1(TIM4);
	}
}

void TIM_IC_Config(void)
{
	LL_GPIO_InitTypeDef timic_gpio;
	LL_TIM_IC_InitTypeDef timic;
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

	timic_gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	timic_gpio.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
	timic_gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	timic_gpio.Alternate = LL_GPIO_AF_2;
	LL_GPIO_Init(GPIOC, &timic_gpio);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = 10000-1;
	timbase_initstructure.Prescaler = 32000-1;
	
	LL_TIM_Init(TIM3, &timbase_initstructure);

	//TIM_IC Configure CH3
	timic.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	timic.ICFilter = LL_TIM_IC_FILTER_FDIV1;
	timic.ICPolarity = LL_TIM_IC_POLARITY_RISING;
	timic.ICPrescaler = LL_TIM_ICPSC_DIV1;
	LL_TIM_IC_Init(TIM3, LL_TIM_CHANNEL_CH3, &timic);

	//TIM_IC Configure CH4
	timic.ICPolarity = LL_TIM_IC_POLARITY_FALLING;
	LL_TIM_IC_Init(TIM3, LL_TIM_CHANNEL_CH4, &timic);

	NVIC_SetPriority(TIM3_IRQn, 0);
	NVIC_EnableIRQ(TIM3_IRQn);

	LL_TIM_EnableIT_CC3(TIM3);
	LL_TIM_EnableIT_CC4(TIM3);

	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH4);

	LL_TIM_EnableCounter(TIM3);
}

void TIM3_IRQHandler(void)
{  
	if(LL_TIM_IsActiveFlag_CC3(TIM3) == SET)
	{
			LL_TIM_ClearFlag_CC3(TIM3);
			LL_TIM_DisableCounter(TIM2);
	}
	if(LL_TIM_IsActiveFlag_CC4(TIM3) == SET)
	{
			LL_TIM_ClearFlag_CC4(TIM3);
			LL_TIM_EnableCounter(TIM2);
			fall_timestamp = LL_TIM_GetCounter(TIM2);
			i = 6;
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
