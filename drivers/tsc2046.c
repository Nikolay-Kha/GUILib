#include "tsc2046.h"
#include "spi.h"
#include "halProxy.h"
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>

// data from calibration
#define TS_ERROR 65535
#define MEASURE_COUNT 10
#define TOUCH_UPDATE_FREQUENCY 10 // Hz
#define TS_LONG_TOUCH_TIME 1000 // ms
#define TS_LONG_TOUCH_COORD_DELTA 15

TS_CALIBRATION_DATA mCalibrationData = {30960, 1816, -29692, 29684};
SPI_TypeDef* TS_SPIx = 0;

void TS_Init()
{
	TS_SPIx = TS_HAL_Init();
	spi_init(TS_SPIx, 32);

	// timer for request touch coords time by time
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseInitTypeDef timer_struct;
	TIM_TimeBaseStructInit(&timer_struct);
	timer_struct.TIM_Period = 999;
	timer_struct.TIM_Prescaler = (clocks.SYSCLK_Frequency/TOUCH_UPDATE_FREQUENCY/(timer_struct.TIM_Period+1)) - 1; // 10 Hz
	TIM_TimeBaseInit(TIM5, &timer_struct);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM5, DISABLE);

	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_SetPriority(TIM5_IRQn, TS_IRQ_PRIORITY);
}

/*void TS_SPI_Config(void)
{ 
	//ports SPI (A and B config)
	RCC->APB2ENR    |= RCC_APB2ENR_IOPAEN;   // Port A
	RCC->APB2ENR    |= RCC_APB2ENR_IOPBEN;   // Port B
	//in-out config
	//PA5 - SCL     - Alternative func. push-pull     (A)
	//PB7 - CS - GPIO - soft        (4)
	//PA6       - MOSI Alternative func.  push-pull- OUT  (A)
	//PA7       - MISO Input floating / Input pull-up - IN
	//PB6 - Interrupt        - input
	GPIOA->CRL = (GPIOA->CRL & 0xFFFFF) | 0xA4A00000;        // Port A (Low)  Bit 5,6  - alt func    7 - input
	GPIOB->CRL = (GPIOB->CRL & 0xFFFFFF) | 0x24000000;        // Port B (Low) bit 7 - out, bit 6 - interrupt in

	//SPI1 CR1 configure
	  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;     // Enable oscil SPI1
	  SPI1->CR1 |= SPI_CR1_BR;                // Baud rate = Fpclk/256
	  SPI1->CR1 &= ~SPI_CR1_CPOL;             // Polarity cls signal CPOL = 0;
	  SPI1->CR1 &= ~SPI_CR1_CPHA;             // Phase cls signal    CPHA = 0;
	  SPI1->CR1 &= ~SPI_CR1_DFF;              // 8 bit data
	  SPI1->CR1 &= ~SPI_CR1_LSBFIRST;         // MSB will be first
	  SPI1->CR1 |= SPI_CR1_SSM;               // Program mode NSS
	  SPI1->CR1 |= SPI_CR1_SSI;               // the same, when NSS 1
	  SPI1->CR2 |= SPI_CR2_SSOE;              // NSS -  slave select
	  SPI1->CR1 |= SPI_CR1_MSTR;              // Mode Master
	  SPI1->CR1 |= SPI_CR1_SPE;               // Enable SPI1

	//--------------------------
	SPI1->CR2 = 0x00000000;  //No interrupt enable


	RCC->APB2ENR|= RCC_APB2ENR_AFIOEN;

	AFIO->EXTICR [6>>0x02] |= AFIO_EXTICR2_EXTI6_PB;
    EXTI->IMR |= EXTI_IMR_MR6;
    EXTI->FTSR |= EXTI_FTSR_TR6;
    EXTI->RTSR |= EXTI_RTSR_TR6;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, TS_IRQ_PRIORITY);

    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseInitTypeDef timer_struct;
    TIM_TimeBaseStructInit(&timer_struct);
    timer_struct.TIM_Period = 999;
    timer_struct.TIM_Prescaler = (clocks.SYSCLK_Frequency/TOUCH_UPDATE_FREQUENCY/(timer_struct.TIM_Period+1)) - 1; // 10 Hz
    TIM_TimeBaseInit(TIM5, &timer_struct);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM5, DISABLE);

    NVIC_EnableIRQ(TIM5_IRQn);
    NVIC_SetPriority(TIM5_IRQn, TS_IRQ_PRIORITY);

}*/

int ts_lastX=-1, ts_lastY=-1;
int tslp_lastX, tslp_lastY;
TouchState pressed = TS_NotPressed;
TouchEventReturn interstingEvents = TS_InterestingMovementAndUp;
int ts_touchTime = 0;

void TS_TouchIrq()
{
	u16 x, y;
	if(TSState()==TS_Pressed) {
		int m = 1;
		if(ts_touchTime>=0) {
			x = TSMeasureX();
			y = TSMeasureY();
			m = 0;
			if(ts_touchTime==0) {
				tslp_lastX = x;
				tslp_lastY = y;
				ts_touchTime += 1000/TOUCH_UPDATE_FREQUENCY;
			} else if(tslp_lastX-x>TS_LONG_TOUCH_COORD_DELTA || tslp_lastX-x<-TS_LONG_TOUCH_COORD_DELTA || tslp_lastY-y>TS_LONG_TOUCH_COORD_DELTA || tslp_lastY-y<-TS_LONG_TOUCH_COORD_DELTA ) {
				ts_touchTime = -1;
			} else if(ts_touchTime>=TS_LONG_TOUCH_TIME) {
				interstingEvents = touchEvent(tslp_lastX,tslp_lastY,TS_LongPress);
				ts_touchTime = -1;
			} else {
				ts_touchTime += 1000/TOUCH_UPDATE_FREQUENCY;
			}

		}

		if(pressed==TS_NotPressed || interstingEvents == TS_InterestingMovementAndUp) {
			if(m) {
				x = TSMeasureX();
				y = TSMeasureY();
			}
			EXTI->PR |= (1<<6);
			if(x==TS_ERROR||y==TS_ERROR || TSState()==TS_NotPressed) {
				if(pressed == TS_Pressed && interstingEvents!=TS_NotInteresting)
					touchEvent(ts_lastX,ts_lastY,TS_Up);
				pressed = TS_NotPressed;
				interstingEvents = TS_InterestingMovementAndUp;
				return;
			}
			if(pressed==TS_NotPressed)
				interstingEvents = touchEvent(x,y,TS_Down);
			else if (x!=ts_lastX ||y!=ts_lastY)
				interstingEvents = touchEvent(x, y, TS_Move);
			ts_lastX = x, ts_lastY = y;
			if(interstingEvents==TS_InterestingMovementAndUp)
				TIM5->CR1 |= TIM_CR1_CEN|TIM_CR1_OPM; // start delay timer
			pressed = TS_Pressed;
		}
	} else {
		if(pressed==TS_Pressed && interstingEvents!=TS_NotInteresting) {
			touchEvent(ts_lastX,ts_lastY,TS_Up);
		}
		pressed = TS_NotPressed;
		interstingEvents = TS_InterestingMovementAndUp;
		ts_touchTime = 0;
	}
}

void TIM5_IRQHandler()
{
	TIM5->SR = (uint16_t)~TIM_IT_Update;
	TS_TouchIrq();
}

void TSDelay(unsigned int DelayCnt)
{
	unsigned int i;
	for(i=0;i<DelayCnt;i++);
}

u16 TSGet(unsigned char data)
{ 
	int d_in; //zero data in
	TS_HAL_CS(); //chip select TOUCH
	TSDelay (10);
	spi_write(TS_SPIx, data); //send command
	d_in = spi_wait_read(TS_SPIx); //ignore this enter
	spi_write(TS_SPIx, 0); //send zero bytes - continue receiving data from module
	d_in = spi_wait_read(TS_SPIx); //receive high byte
	d_in <<= 8; //move it 8
	spi_write(TS_SPIx, 0); //send zero bytes - cont receiving data from module
	d_in |= spi_wait_read(TS_SPIx); //receive low byte
	TS_HAL_DCS();
	return d_in;
}  

u16 delta(u16 u1, u16 u2)
{
	if(u1>u2)
		return u1-u2;
	else
		return u2-u1;
}

#define FILTER_THRESHOLD 3
u16 filter(u16 u1, u16 u2, u16 u3) {
	if(delta(u1, u2)<FILTER_THRESHOLD) return ((u32)u1+(u32)u2)/(u32)2;
	if(delta(u1, u3)<FILTER_THRESHOLD) return ((u32)u1+(u32)u3)/(u32)2;
	if(delta(u2, u3)<FILTER_THRESHOLD) return ((u32)u2+(u32)u3)/(u32)2;
	return 0;
}

inline u16 TSReadXNative(void)
{ // http://e2e.ti.com/support/other_analog/touch/f/750/p/202636/720247.aspx#720247
   u16 r1 = TSGet(0x99);
   u16 r2 = TSGet(0x99);
   u16 r3 = TSGet(0x98);
   return filter(r1,r2,r3);
}

u16 TSReadX(void)
{
   s32 f = TSReadXNative();
   if(f==0)
	   return TS_ERROR;
   s32 r = (f-mCalibrationData.ADC_X_MIN)*LCD_XSIZE/(mCalibrationData.ADC_X_WIDTH)-1;
   if(r<0)
	   return 0;
   if(r>LCD_XSIZE-1)
	   return LCD_XSIZE-1;
   return r;
}

inline u16 TSReadYNative(void)
{// http://e2e.ti.com/support/other_analog/touch/f/750/p/202636/720247.aspx#720247
	u16 r1 = TSGet(0xD9);
	u16 r2 = TSGet(0xD9);
	u16 r3 = TSGet(0xD8);
	return filter(r1,r2,r3);
}

u16 TSReadY(void)
{
	s32 f = TSReadYNative();
	if(f==0)
	   return TS_ERROR;
	s32 r = (f-mCalibrationData.ADC_Y_MIN)*LCD_YSIZE/(mCalibrationData.ADC_Y_WIDTH)-1;
	if(r<0)
		return 0;
	if(r>LCD_YSIZE-1)
		return LCD_YSIZE-1;
	return r;
}

typedef u16 (*READ)();
u16 TSMeasure( READ func, u32 count) {
	u16 tmp = TS_ERROR, i;
	for(i=0 ; i<count; i++)	{
		u16 ne = func();
		if(TSState() == TS_NotPressed)
			return TS_ERROR;
		if(ne==TS_ERROR) {
			continue;
		}
		if(tmp==TS_ERROR)
			tmp = ne;
		else
			tmp = (tmp+ne)/2;
		TSDelay (10);
	}
	return tmp;
}

u16 TSMeasureX(void){
	return TSMeasure(TSReadX, MEASURE_COUNT);
}

u16 TSMeasureY(void) {
	return TSMeasure(TSReadY, MEASURE_COUNT);
}

inline TouchState TSState(void)
{ //TS_Pressed if touched, TS_NotPressed if no touched
	if (TS_HAL_Sense() == 0)
		return TS_Pressed;
	return TS_NotPressed;
}

int mTsCalibrationInProgress;
#define TS_CALIBRATION_POINTS_COUNT 4
#define TS_CALIBRATION_OFFSET 20
#define TS_CALIBRATION_POINTS_X {TS_CALIBRATION_OFFSET, LCD_XSIZE-TS_CALIBRATION_OFFSET, LCD_XSIZE-TS_CALIBRATION_OFFSET, TS_CALIBRATION_OFFSET}
#define TS_CALIBRATION_POINTS_Y {TS_CALIBRATION_OFFSET, TS_CALIBRATION_OFFSET, LCD_YSIZE-TS_CALIBRATION_OFFSET, LCD_YSIZE-TS_CALIBRATION_OFFSET}
#define TS_CALIBRATION_MEASURE_COUNT 200
int TSCalibrate()
{
	int waitUnpress = 1;
	int waitPress = 0;
	int pointNumber = 0;
	const u16 xpoints[TS_CALIBRATION_POINTS_COUNT] = TS_CALIBRATION_POINTS_X;
	const u16 ypoints[TS_CALIBRATION_POINTS_COUNT] = TS_CALIBRATION_POINTS_Y;
	s32 xpointsRes[TS_CALIBRATION_POINTS_COUNT];
	s32 ypointsRes[TS_CALIBRATION_POINTS_COUNT];
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	mTsCalibrationInProgress = 1;

	touchEvent(xpoints[pointNumber], ypoints[pointNumber], TS_WaitTouchPointForCalibration);
	while(mTsCalibrationInProgress) {
		if(waitUnpress && TSState()==TS_NotPressed) {
			waitUnpress = 0;
			waitPress = 1;
			TSDelay (10000);
		} else if (waitPress && TSState()==TS_Pressed) {
			xpointsRes[pointNumber] = TSMeasure(TSReadXNative, TS_CALIBRATION_MEASURE_COUNT);
			if(xpointsRes[pointNumber]==TS_ERROR)
				continue;
			ypointsRes[pointNumber] = TSMeasure(TSReadYNative, TS_CALIBRATION_MEASURE_COUNT);
			if(ypointsRes[pointNumber]==TS_ERROR)
				continue;
			pointNumber++;
			if(pointNumber>=TS_CALIBRATION_POINTS_COUNT) {
				double xAdcPerPoint = ((double)(xpointsRes[2]-xpointsRes[0])/(double)(xpoints[2]-xpoints[0]) + (double)(xpointsRes[3]-xpointsRes[1])/(double)(xpoints[3]-xpoints[1]))/2.0L;
				double yAdcPerPoint = ((double)(ypointsRes[3]-ypointsRes[0])/(double)(ypoints[3]-ypoints[0]) + (double)(ypointsRes[2]-ypointsRes[1])/(double)(ypoints[2]-ypoints[1]))/2.0L;
				mCalibrationData.ADC_X_MIN = (xpointsRes[0]-xpoints[0]*xAdcPerPoint + xpointsRes[1]-xpoints[1]*xAdcPerPoint + xpointsRes[2]-xpoints[2]*xAdcPerPoint + xpointsRes[3]-xpoints[3]*xAdcPerPoint)/4;
				mCalibrationData.ADC_X_WIDTH = xAdcPerPoint*LCD_XSIZE;
				mCalibrationData.ADC_Y_MIN = (ypointsRes[0]-ypoints[0]*yAdcPerPoint + ypointsRes[1]-ypoints[1]*yAdcPerPoint + ypointsRes[2]-ypoints[2]*yAdcPerPoint + ypointsRes[3]-ypoints[3]*yAdcPerPoint)/4;
				mCalibrationData.ADC_Y_WIDTH = yAdcPerPoint*LCD_YSIZE;
				touchEvent(0, 0, TS_CalibrationDone);
				while(TSState()==TS_Pressed);
				EXTI->PR |= (1<<6);
				NVIC_EnableIRQ(EXTI9_5_IRQn);
				return 1;
			} else {
				touchEvent(xpoints[pointNumber], ypoints[pointNumber], TS_WaitTouchPointForCalibration);
			}
			waitPress = 0;
			waitUnpress = 1;
			TSDelay (10000);
		}
	}

	touchEvent(0, 0, TS_CalibrationDone);
	EXTI->PR |= (1<<6);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	return 0;
}

void TSInteruptCallibration()
{
	mTsCalibrationInProgress = 0;
}

TS_CALIBRATION_DATA * TSGetCalibrationDataPtr()
{
	return &mCalibrationData;
}
