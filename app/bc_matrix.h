#ifndef _BC_MATRIX_H
#define _BC_MATRIX_H

#include <bc_gpio.h>
#include <bc_scheduler.h>

typedef enum
{
    BC_MATRIX_EVENT_CHANGE = 0

} bc_matrix_event_t;

typedef struct bc_matrix_t bc_matrix_t;

struct bc_matrix_t{
    bc_gpio_channel_t *_in_gpio;
    uint8_t _in_length;
    bc_gpio_channel_t *_out_gpio;
    uint8_t _out_length;
    bc_scheduler_task_id_t _task_id;
    void (*_event_handler)(bc_matrix_t *, bc_matrix_event_t, void *);
    void *_event_param;
    uint64_t _debounce;
    uint64_t _state;
    uint64_t _last_state;
    bc_tick_t _scan_interval;
    bool _first;

};

void bc_matrix_init(bc_matrix_t *self, bc_gpio_channel_t *out_gpio, uint8_t out_length, bc_gpio_channel_t *in_gpio, uint8_t in_length);

void bc_matrix_set_event_handler(bc_matrix_t *self, void (*event_handler)(bc_matrix_t *, bc_matrix_event_t, void *), void *event_param);

void bc_matrix_set_scan_interval(bc_matrix_t *self, bc_tick_t scan_interval);

uint64_t bc_matrix_get_state(bc_matrix_t *self);

int getKey(uint64_t keyCode);


#endif // _BC_MATRIX_H
