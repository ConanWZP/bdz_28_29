#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "termoresistor.h"
#include "leds.h"
#include "initTim4.h"
#include "initShim.h"
#include "pwmSetPulse.h"

//#define START_MESSURE() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET)
//#define FINISH_MESSURE() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET)

#define IS_MESSURING HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)


//#define CHANGE_ACCURACY() HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8)


#define IS_NUMBER_AFTER_COMMA HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)

#define IS_NEW_UNIT HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9)



void SystemClock_Config(void);
void SysTick_Handler(void);

void changeUnit(uint16_t unit, double* currentArray, double* displayValue);
void changeAccuracy(uint16_t accuracy, double* currentArray, double* initialArray, uint16_t unit, double* displayValue);

int main (void) {
	
	HAL_Init();

	SystemClock_Config();
	
	allLedsHalInit();
	
	// init gpio_pin_14 (diod) like alterate func
	initShim();
	
	
	initTim4();
	
	
	// for termodatchik nado init PC1 kak analog vxod
	// ACP (ADC1) in neprerivniy regim, razryadnost 12 bit
	
	
	// podklychenie modylya klaviatyri sleva
	// dlya keyboard EXTI nado 
	
	
	
	// initialTemperature - ispolzuetsya dlya vichislenya tekyshei temperatyri posle smeni tochnosti ili smeni edinitsi izmereniya
	//double initialTemperature = 25.6;
	// {celcia, farengeit, kelvin}
	double initialTemperatureArray[3] = {25.6, 78.1, 298.6};
	
	
	// currentTemperature ispolzyetsya dlya vivoda na ekran i imenno v ety peremennyiy zapisivaytsya perevodov tochnosti i smnei ediniz izmereniya
	//double currentTemperature = 25.6;
	// {celcia, farengeit, kelvin}
	double currentTemperatureArray[3] = {25.6, 78.1, 298.6};
	
	double displayValue = 25.6;
	
	// posle smeni edinitsiy izmerenya a zatem izmeneniya tochnosti mogut bit' rassinchroni, poetomy nado libo pisat' uslovia vnutri functii
	
	uint16_t wasStart = 0;
	
	// 0 - bez zapyatoy, 1 - s zapyatoy
	uint16_t currentAccuracy = 1;
	uint16_t accuracyArray[2] = {0, 1};
	
	// 0 - celci; 1 - farengeit; 2 - kelvin;
	uint16_t currentUnit = 0;
	uint16_t unitOfMeasurement[3] = {0, 1, 2};
	
	uint16_t varFlag = 0;
	uint16_t cnt = 0;


	while(1){


		if (IS_MESSURING == 1) {
			wasStart = 1;
		}
		
		// logika s otobrageniem na displee i chteniem knopok po tipy IS_NUMBER_AFTER_COMMA dolgen bit' vne yslovia    if (wasStart). 
		// wasStart nygen tolko dlya morgania dioda i polychenia znacheniya s termoresistora   i zapisi etogo znachenia v kakye-to peremennyi
		// prichem nado dve peremennii - odno - otobragaemoe; vtoroe - iznachalnoe. Absolutno kak v calculatore na rabote
		
		if (wasStart) {
			
			LinearSearch();
			
			displayValue = result;
			// result - v celciax s zapyatoy
			initialTemperatureArray[0] = result;
			currentTemperatureArray[0] = result;
			
			initialTemperatureArray[1] = (result * 9 / 5) + 32;
			currentTemperatureArray[1] = (result * 9 / 5) + 32;
			
			initialTemperatureArray[2] = result + 273;
			currentTemperatureArray[2] = result + 273;
			
			
			if (varFlag) {
				pwmSetPulse(cnt, timerFour);
			} else {
				pwmSetPulse(1000-cnt, timerFour);
			}
		
			cnt++;
		
			if (cnt == 1000) {
				varFlag = varFlag ? 0 : 1;
				cnt = 0;
			}
			HAL_Delay(1);
			/*
			// logika s accuracyArray[currentAccuracy]
			changeAccuracy(accuracyArray[currentAccuracy]);
			
			if (IS_NUMBER_AFTER_COMMA == 1) {
				currentAccuracy = (currentAccuracy + 1) % 2;
			}
			*/
			
		
			
			wasStart = 0;
		}
		
		
		if (IS_NUMBER_AFTER_COMMA == 1) {
			currentAccuracy = (currentAccuracy + 1) % 2; // 2 - razmer massiva
			
			// logika s accuracyArray[currentAccuracy] - vozmogno vizov funcii nado gde
			changeAccuracy(accuracyArray[currentAccuracy], currentTemperatureArray, initialTemperatureArray, currentUnit, &displayValue);
		}
		
		// logika s accuracyArray[currentAccuracy] - vozmogno vizov funcii nado gde
		//changeAccuracy(accuracyArray[currentAccuracy]);
		
		
		if (IS_NEW_UNIT == 1) {
			currentUnit = (currentUnit + 1) % 3; // // 3 - razmer massiva
			changeUnit(unitOfMeasurement[currentUnit], currentTemperatureArray, &displayValue);
		}
		
		
		otobrazit' na displee tyt LED_BLA_BLA('kakay-ta stroka')
		

	}
}

void changeUnit(uint16_t unit, double* currentArray, double* displayValue) {
	// vot tak nado!!!!!!!!!!!!!!
	*displayValue = currentArray[unit];
	
	
	/*
	if (unit == 2) {
		// kelvin = celcia + 273;
		// vivodim na display novoy temperatyry i menyem bykvy
	} else if (unit == 1) {
		// farengeit = (celcia * 9 / 5) + 32;
		// vivodim na display novoy temperatyry i menyem bykvy
	} else {
		// celcia = celcia
		// vivodim na display novoy temperatyry i menyem bykvy
	}
	*/
}

// dolgno rabotat'
void changeAccuracy(uint16_t accuracy, double* currentArray, double* initialArray, uint16_t unit, double* displayValue) {
	if (accuracy) {
		
		currentArray[0] = initialArray[0];
		currentArray[1] = initialArray[1];
		currentArray[2] = initialArray[2];
		// na displee ykazivaem chislo s zapyatoy (to est' s samim chislom temperatyry nichego ne delaem), a prosto peredaem ego na display
		
	} else {
		
		// okruglenie
		currentArray[0] = rint(currentArray[0]);
		currentArray[1] = rint(currentArray[1]);
		currentArray[2] = rint(currentArray[2]);
		//ili prosto otbrosit drobnyi chast  currentArray[0] = (uint16_t) currentArray[0];
		
		
		// otobragaem chislo bez zapyatoy. (to est' nado okruglit')
		
	}

	*displayValue = currentArray[unit];
}

void SystemClock_Config(void)
{
		RCC_OscInitTypeDef RCC_OscInitStruct = {0};
		RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

		/** Configure the main internal regulator output voltage 
		*/
		__HAL_RCC_PWR_CLK_ENABLE();
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		/** Initializes the CPU, AHB and APB busses clocks 
		*/
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
		RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
		RCC_OscInitStruct.PLL.PLLM = 8;
		RCC_OscInitStruct.PLL.PLLN = 50;
		RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
		RCC_OscInitStruct.PLL.PLLQ = 7;
		HAL_RCC_OscConfig(&RCC_OscInitStruct);

		/** Initializes the CPU, AHB and APB busses clocks 
		*/
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
		HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);		
}

void SysTick_Handler(void) {	
    HAL_IncTick();
}
