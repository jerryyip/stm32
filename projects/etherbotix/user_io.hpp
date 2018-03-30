/*
 * Copyright (c) 2014-2018, Michael E. Ferguson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ETHERBOTIX_USER_IO_HPP_
#define _ETHERBOTIX_USER_IO_HPP_

usart3_t usart3;

// State of user IO
uint8_t user_io_usart3_active_;
uint8_t user_io_spi2_active_;
uint8_t user_io_tim9_active_;
uint8_t user_io_tim12_active_;

inline void user_io_init()
{
  user_io_usart3_active_ = 0;
  user_io_spi2_active_ = 0;
  user_io_tim9_active_ = 0;
  user_io_tim12_active_ = 0;
}

inline void user_io_set_output()
{
  if (registers.digital_out & 0x01)
    a0_sense::high();
  else
    a0_sense::low();

  if (user_io_spi2_active_ == 0)
  {
    // A1 is also SPI2_MISO
    if (registers.digital_out & 0x02)
      a1_sense::high();
    else
      a1_sense::low();

    // A2 is also SPI2_MOSI
    if (registers.digital_out & 0x04)
      a2_sense::high();
    else
      a2_sense::low();
  }

  if ((user_io_spi2_active_ == 0) &&
      (user_io_usart3_active_ == 0))
  {
    // D3 is also USART3_TX, SPI2_SCK
    if (registers.digital_out & 0x08)
      d3::high();
    else
      d3::low();
  }

  if (user_io_usart3_active_ == 0)
  {
    // D4 is also USART3_RX
    if (registers.digital_out & 0x10)
      d4::high();
    else
      d4::low();
  }

  if (user_io_tim12_active_ == 0)
  {
    // D5 is also TIM12_CH2
    if (registers.digital_out & 0x20)
      d5::high();
    else
      d5::low();
  }

  if (user_io_tim9_active_ == 0)
  {
    // D6 is also TIM9_CH1
    if (registers.digital_out & 0x40)
      d6::high();
    else
      d6::low();

    // D7 is also TIM9_CH2
    if (registers.digital_out & 0x80)
      d7::high();
    else
      d7::low();
  }
}

inline void user_io_set_direction()
{
  if (registers.digital_dir & 0x01)
    a0_sense::mode(GPIO_OUTPUT);
  else
    a0_sense::mode(GPIO_INPUT_ANALOG);

  if (user_io_spi2_active_ == 0)
  {
    // A1 is also SPI2_MISO
    if (registers.digital_dir & 0x02)
      a1_sense::mode(GPIO_OUTPUT);
    else
      a1_sense::mode(GPIO_INPUT_ANALOG);

    // A2 is also SPI2_MOSI
    if (registers.digital_dir & 0x04)
      a2_sense::mode(GPIO_OUTPUT);
    else
      a2_sense::mode(GPIO_INPUT_ANALOG);
  }

  if ((user_io_spi2_active_ == 0) &&
      (user_io_usart3_active_ == 0))
  {
    // D3 is also SPI2_SCK, USART3_TX
    if (registers.digital_dir & 0x08)
      d3::mode(GPIO_OUTPUT);
    else
      d3::mode(GPIO_INPUT);
  }

  if (user_io_usart3_active_ == 0)
  {
    // D4 is also USART3_TX
    if (registers.digital_dir & 0x10)
      d4::mode(GPIO_OUTPUT);
    else
      d4::mode(GPIO_INPUT);
  }

  if (user_io_tim12_active_ == 0)
  {
    // D5 is also TIM12_CH2
    if (registers.digital_dir & 0x20)
      d5::mode(GPIO_OUTPUT);
    else
      d5::mode(GPIO_INPUT);
  }

  if (user_io_tim9_active_ == 0)
  {
    // D6 is also TIM9_CH1
    if (registers.digital_dir & 0x40)
      d6::mode(GPIO_OUTPUT);
    else
      d6::mode(GPIO_INPUT);

    // D7 is also TIM9_CH2
    if (registers.digital_dir & 0x80)
      d7::mode(GPIO_OUTPUT);
    else
      d7::mode(GPIO_INPUT);
  }
}

/******************************************************************************
 * USER USART
 */

/** @brief Start the USART */
inline bool user_io_usart3_init()
{
  // Use dynamixel defs of baud
  uint32_t baud = 0;
  if (registers.usart3_baud == 1)
    baud = 1000000;
  else if (registers.usart3_baud == 3)
    baud = 500000;
  else if (registers.usart3_baud == 16)
    baud = 115200;
  else if (registers.usart3_baud == 34)
    baud = 57600;
  else if (registers.usart3_baud == 103)
    baud = 19200;
  else if (registers.usart3_baud == 207)
    baud = 9600;
  else if (registers.usart3_baud == 250)
    baud = 2250000;
  else if (registers.usart3_baud == 251)
    baud = 2500000;
  else if (registers.usart3_baud == 252)
    baud = 3000000;
  else
  {
    // Unsupported baud, turn off usart3
    registers.usart3_baud = 0;
    d3::mode(GPIO_INPUT);
    d4::mode(GPIO_INPUT);
    user_io_usart3_active_ = 0;
    return false;
  }

  RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
  d3::mode(GPIO_ALTERNATE | GPIO_AF_USART3);
  d4::mode(GPIO_ALTERNATE | GPIO_AF_USART3);
  usart3.init(baud, 8);
  user_io_usart3_active_ = 1;
  return true;
}

/** @brief Write a buffer of data to the USART3 */
inline void user_io_usart3_write(uint8_t * data, uint8_t len)
{
  if (user_io_usart3_active_ == 0)
    return;

  uint16_t d[len];
  for (int i = 0; i < len; ++i)
    d[i] = data[i];

  usart3.write(d, len);
}

/** @brief Read a buffer of data from the USART3 */
inline uint8_t user_io_usart3_read(uint8_t * data, uint8_t max_len)
{
  if (user_io_usart3_active_ == 0)
    return 0;

  if (max_len == 0)
    max_len = 255;

  int len = 0;
  while (max_len--)
  {
    int16_t d = usart3.read();
    if (d == -1)
      return len;
    data[len++] = d;
  }
  return len;
}

/******************************************************************************
 * USER TIMER 12
 */

inline bool user_io_tim12_init()
{
  if (registers.tim12_mode == 1)  // Count on external clock
  {
    // Turn on clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;

    d5::mode(GPIO_ALTERNATE | GPIO_AF_TIM12);
    TIM12->CR1 &= (uint16_t) ~TIM_CR1_CEN;
    TIM12->ARR = 65535;  // Max range
    TIM12->SMCR |= TIM_TS_TI2FP2;  // Use filtered TI2
    TIM12->SMCR |= TIM_SlaveMode_External1;  // Use external clock mode 1 (count rising edges)
    // TODO Filtering with IC2F
    TIM12->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    // Disable
    registers.tim12_mode = 0;
    d5::mode(GPIO_INPUT);
  }
  user_io_tim12_active_ = registers.tim12_mode;
  return true;
}

inline uint16_t user_io_tim12_get_count()
{
  if (user_io_tim12_active_ > 0)
    return TIM12->CNT;
  else
    return 0;
}


/******************************************************************************
 * Feedback
 */

inline void user_io_update()
{
  registers.digital_in =
    (a0_sense::value() ? (1<<0) : 0) +
    (a1_sense::value() ? (1<<1) : 0) +
    (a2_sense::value() ? (1<<2) : 0) +
    (d3::value() ? (1<<3) : 0) +
    (d4::value() ? (1<<4) : 0) +
    (d5::value() ? (1<<5) : 0) +
    (d6::value() ? (1<<6) : 0) +
    (d7::value() ? (1<<7) : 0);

  registers.tim12_count = user_io_tim12_get_count();
}



#endif  // _ETHERBOTIX_USER_IO_HPP_
