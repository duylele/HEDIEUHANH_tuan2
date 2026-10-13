# bai2_hedieuhanhtuan2 - FreeRTOS STM32F103: EXTI + Queue + UART

Bài tập 02 môn Hệ điều hành: chương trình FreeRTOS trên vi điều khiển STM32F103.

## Yêu cầu

- Cấu hình 1 nút nhấn ngắt ngoài (EXTI, cạnh xuống / falling edge)
- Cấu hình giao tiếp UART1 truyền thông nối tiếp với máy tính
- ISR EXTI: tăng biến đếm số lần nhấn nút, gửi giá trị vào Queue (`xQueueSendFromISR`), chuyển ngữ cảnh ngay sang Task nhận nếu cần (`portYIELD_FROM_ISR`)
- 1 Task UART: ở trạng thái Blocked chờ dữ liệu Queue, định dạng giá trị thành chuỗi và gửi lên Serial Terminal của máy tính

## Phần cứng

| Linh kiện | Chân STM32 |
|---|---|
| Nút nhấn (một đầu) | PA0 (EXTI0, pull-up nội) |
| Nút nhấn (đầu còn lại) | GND |
| USB-UART RX | PA9 (USART1 TX) |
| USB-UART TX | PA10 (USART1 RX) |
| USB-UART GND | GND |

Nạp code bằng ST-Link V2 (SWDIO, SWCLK, GND, 3.3V).

## Cấu trúc thư mục

```
.
├── main.c                 # code chính: EXTI, UART, Queue, task
├── startup.c              # bảng vector ngắt, Reset_Handler
├── linker.ld              # bản đồ bộ nhớ Flash/RAM
├── FreeRTOSConfig.h       # cấu hình FreeRTOS
├── stm32f1xx_hal_conf.h   # cấu hình module HAL
├── Makefile
├── FreeRTOS-Kernel/       # thư viện FreeRTOS
├── stm32f1xx_hal_driver/  # thư viện HAL
├── cmsis_device_f1/       # CMSIS thanh ghi STM32F1
└── cmsis_core/            # CMSIS lõi Cortex-M
```

## Nguyên lý hoạt động

1. Nhấn nút PA0 (cạnh xuống) sinh ngắt `EXTI0_IRQHandler`.
2. ISR xóa cờ ngắt, chống dội phím 200 ms, tăng biến đếm, gửi giá trị vào Queue bằng `xQueueSendFromISR`.
3. `portYIELD_FROM_ISR` chuyển ngữ cảnh ngay sang `vUartTask` nếu task này có ưu tiên cao hơn task đang chạy.
4. `vUartTask` ở trạng thái Blocked trên `xQueueReceive(..., portMAX_DELAY)`. Khi nhận được số, task đổi sang chuỗi và gửi qua USART1 lên Serial Terminal.

## Build và nạp

```bash
sudo apt install -y gcc-arm-none-eabi make stlink-tools picocom
make
make flash
```

## Chạy thử

```bash
picocom -b 9600 /dev/ttyUSB0
```

Nhấn nút, terminal in ra:

```
So lan nhan nut: 1
So lan nhan nut: 2
So lan nhan nut: 3
...
```

Thoát picocom: `Ctrl+A` rồi `Ctrl+X`.

## Demo video

<dán link video vào đây>
