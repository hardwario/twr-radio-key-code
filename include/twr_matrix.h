#ifndef _TWR_MATRIX_H
#define _TWR_MATRIX_H

#include <twr_gpio.h>
#include <twr_scheduler.h>

typedef enum
{
    TWR_MATRIX_EVENT_CHANGE = 0

} twr_matrix_event_t;

typedef struct twr_matrix_t twr_matrix_t;

struct twr_matrix_t{
    twr_gpio_channel_t *_in_gpio;
    uint8_t _in_length;
    twr_gpio_channel_t *_out_gpio;
    uint8_t _out_length;
    twr_scheduler_task_id_t _task_id;
    void (*_event_handler)(twr_matrix_t *, twr_matrix_event_t, void *);
    void *_event_param;
    uint64_t _debounce;
    uint64_t _state;
    uint64_t _last_state;
    twr_tick_t _scan_interval;
    bool _first;

};

void twr_matrix_init(twr_matrix_t *self, twr_gpio_channel_t *out_gpio, uint8_t out_length, twr_gpio_channel_t *in_gpio, uint8_t in_length);

void twr_matrix_set_event_handler(twr_matrix_t *self, void (*event_handler)(twr_matrix_t *, twr_matrix_event_t, void *), void *event_param);

void twr_matrix_set_scan_interval(twr_matrix_t *self, twr_tick_t scan_interval);

uint64_t twr_matrix_get_state(twr_matrix_t *self);

int getKey(uint64_t keyCode);


#endif // _TWR_MATRIX_H
