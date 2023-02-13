/*
 * gps.cpp
 *
 *  Created on: 5 Jul 2022
 *      Author: Luca
 */

#include "gps.h"
#include <stdio.h>

uint8_t c1;
uint32_t lr1;

static uint8_t ublox_request_115200_baud[] = {
    0xb5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xd0, 0x08, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00, 0x07, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x96, 0xb5, 0x62,
    0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};
static uint8_t ublox_request_10Hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06,
                                       0x00, 0x64, 0x00, 0x01, 0x00,
                                       0x01, 0x00, 0x7A, 0x12};
TinyGPSPlus gps;

extern UART_HandleTypeDef huart1;

void gpsSetup() {

  uint8_t command[20];

  // Check hardware version
  if (HAL_GPIO_ReadPin(HARDWARE_ID_GPIO_Port, HARDWARE_ID_Pin)) {
    // Flight computer
    HAL_UART_Transmit(&huart1, ublox_request_115200_baud,
                      sizeof(ublox_request_115200_baud), 100);
  } else {
    // Groundstation
    /* Request UART speed of 115200 */
    sprintf((char *)command, "$PCAS01,5*19\r\n");
    HAL_UART_Transmit(&huart1, command, 14, 100);
  }

  HAL_Delay(200);

  /* Change bus speed to 115200 */
  USART1->CR1 &= ~(USART_CR1_UE);
  USART1->BRR = 417; // Set baud to 115200
  USART1->CR1 |= USART_CR1_UE;

  HAL_Delay(200);

  // Check hardware version
  if (HAL_GPIO_ReadPin(HARDWARE_ID_GPIO_Port, HARDWARE_ID_Pin)) {
    // Flight computer
    HAL_UART_Transmit(&huart1, ublox_request_10Hz, sizeof(ublox_request_10Hz),
                      100);
  } else {
    // Groundstation
    /* Request 10Hz update rate */
    sprintf((char *)command, "$PCAS02,100*1E\r\n");
    HAL_UART_Transmit(&huart1, command, 16, 100);

    HAL_Delay(10);

    /* Request airbourne <4g mode*/
    sprintf((char *)command, "$PCAS11,7*1A\r\n");
    HAL_UART_Transmit(&huart1, command, 14, 100);
  }
}
