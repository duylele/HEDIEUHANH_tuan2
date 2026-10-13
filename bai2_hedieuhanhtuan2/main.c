#include <stdint.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static UART_HandleTypeDef huart1;
static QueueHandle_t xCountQueue;
static volatile uint32_t ulPressCount = 0;
static TickType_t xLastPressTick = 0;

/* HAL dung tick cua FreeRTOS, khong tu cau hinh SysTick */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    (void)TickPriority;
    return HAL_OK;
}

uint32_t HAL_GetTick(void)
{
    return (uint32_t)xTaskGetTickCount();
}

static void GPIO_Init(void)
{
    GPIO_InitTypeDef g = {0};

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* PA0: ngat ngoai canh xuong, pull-up */
    g.Pin  = GPIO_PIN_0;
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &g);

    /* PA9: USART1 TX */
    g.Pin   = GPIO_PIN_9;
    g.Mode  = GPIO_MODE_AF_PP;
    g.Pull  = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &g);

    /* PA10: USART1 RX */
    g.Pin  = GPIO_PIN_10;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &g);
}

static void UART1_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 9600;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    HAL_UART_Init(&huart1);
}

static void EXTI0_NVIC_Init(void)
{
    /* Uu tien 10 (0xA0) thap hon configMAX_SYSCALL_INTERRUPT_PRIORITY (0x50) */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

static void UART_Print(const char *s)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)s, (uint16_t)strlen(s), 100);
}

static void UIntToString(uint32_t v, char *out)
{
    char tmp[11];
    int i = 0;
    if (v == 0) tmp[i++] = '0';
    while (v > 0) { tmp[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i > 0) *out++ = tmp[--i];
    *out = '\0';
}

/* ISR ngat ngoai EXTI0 */
void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

        TickType_t now = xTaskGetTickCountFromISR();
        if ((now - xLastPressTick) >= pdMS_TO_TICKS(200))   /* chong doi phim */
        {
            xLastPressTick = now;
            ulPressCount++;                                  /* tang bien dem */
            uint32_t value = ulPressCount;
            xQueueSendFromISR(xCountQueue, &value, &xHigherPriorityTaskWoken);
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);            /* chuyen ngu canh ngay */
}

/* Task UART: Blocked cho den khi Queue co du lieu */
static void vUartTask(void *pvParameters)
{
    (void)pvParameters;
    uint32_t count;
    char numStr[12];

    for (;;)
    {
        if (xQueueReceive(xCountQueue, &count, portMAX_DELAY) == pdPASS)
        {
            UIntToString(count, numStr);
            UART_Print("So lan nhan nut: ");
            UART_Print(numStr);
            UART_Print("\r\n");
        }
    }
}

int main(void)
{
    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);   /* bat buoc cho FreeRTOS */

    UART1_Init();
    GPIO_Init();

    xCountQueue = xQueueCreate(10, sizeof(uint32_t));
    if (xCountQueue == NULL) { for (;;) { } }

    xTaskCreate(vUartTask, "UART", 128, NULL, 2, NULL);

    EXTI0_NVIC_Init();         /* bat ngat sau khi da tao queue */

    vTaskStartScheduler();
    for (;;) { }
}
