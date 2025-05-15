#include "debug.h"
#include "ch32v30x_rtc.h"

volatile char timeStr[20] = {0};

// RTC初始化
void RTC_Init (void) {
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd (ENABLE);

    if (BKP_ReadBackupRegister (BKP_DR1) != 0xA5A5) {
        RCC_LSEConfig (RCC_LSE_ON);
        while (RCC_GetFlagStatus (RCC_FLAG_LSERDY) == RESET)
            ;
        RCC_RTCCLKConfig (RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd (ENABLE);
        RTC_WaitForSynchro();
        RTC_SetPrescaler (32767);  // 1Hz时钟
        BKP_WriteBackupRegister (BKP_DR1, 0xA5A5);
    }
    RTC_WaitForSynchro();
}

// 获取时间字符串    
    void Get_TimeString1 (char *timeStr) {
        uint32_t time = RTC_GetCounter();
        uint8_t hours = time / 3600;
        uint8_t minutes = (time % 3600) / 60;
        uint8_t seconds = time % 60;
        sprintf (timeStr, "Time: %02d:%02d:%02d", hours, minutes, seconds);
    }

void TIM2_IRQHandler(void) {
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        Get_TimeString1(timeStr);  // 每秒更新一次时间
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void Timer_Init(void) {
    TIM_TimeBaseInitTypeDef tim;
    NVIC_InitTypeDef nvic;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    tim.TIM_Period = 1000 - 1;  // 1kHz时钟
    tim.TIM_Prescaler = SystemCoreClock/1000 - 1;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &tim);
    
    nvic.NVIC_IRQChannel = TIM2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void Get_TimeString(char *buf) {
    uint32_t time = RTC_GetCounter();
    uint8_t hours = time / 3600 % 24;  // 添加24小时限制
    uint8_t minutes = (time % 3600) / 60;
    uint8_t seconds = time % 60;
    snprintf(buf, 20, " 5/15 %02d:%02d:%02d\n", hours, minutes, seconds);
}