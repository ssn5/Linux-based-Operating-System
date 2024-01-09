#if !defined(RTC_H)
#define RTC_H

#include "types.h"

#define MAX_RTC_FREQ    1024
#define MAX_RTC_FREQ_BM 0x6
#define MIN_RTC_FREQ    2
#define MIN_RTC_FREQ_BM 0xF

volatile unsigned int rtc_interrupt_occurred;
volatile unsigned int rtc_initialized;
volatile unsigned int rtc_frequency;
volatile unsigned int rtc_frequency_counter;
volatile unsigned int rtc_frequency_counter_limit;

extern void rtc_init();
extern void rtc_handler();
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf_arg, int32_t n);
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
volatile unsigned int rtc_overall_tick_counter;

#endif
