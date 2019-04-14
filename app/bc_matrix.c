#include <bc_matrix.h>

void _bc_matrix_init_task(void *param);

void bc_matrix_init(bc_matrix_t *self, bc_gpio_channel_t *out_gpio, uint8_t out_length, bc_gpio_channel_t *in_gpio, uint8_t in_length)
{
    memset(self, 0, sizeof(*self));

    self->_in_gpio = in_gpio;

    self->_in_length = in_length;

    self->_out_gpio = out_gpio;

    self->_out_length = out_length;

    self->_scan_interval = 100;

    uint8_t i;
    bc_gpio_channel_t channel;

    for (i = 0; i < self->_out_length; i++)
    {
        channel = self->_out_gpio[i];
        bc_gpio_init(channel);
        bc_gpio_set_output(self->_out_gpio[i], 1);
        bc_gpio_set_mode(channel, BC_GPIO_MODE_ANALOG);
    }

    for (i = 0; i < self->_in_length; i++)
    {
        channel = self->_in_gpio[i];
        bc_gpio_init(channel);
        bc_gpio_set_mode(channel, BC_GPIO_MODE_INPUT);
        bc_gpio_set_pull(channel, BC_GPIO_PULL_DOWN);
    }

    self->_task_id = bc_scheduler_register(_bc_matrix_init_task, self, 0);
}

void bc_matrix_set_event_handler(bc_matrix_t *self, void (*event_handler)(bc_matrix_t *, bc_matrix_event_t, void *), void *event_param)
{
    self->_event_handler = event_handler;
    self->_event_param = event_param;
}

void bc_matrix_set_scan_interval(bc_matrix_t *self, bc_tick_t scan_interval)
{
    self->_scan_interval = scan_interval;
}

uint64_t bc_matrix_get_state(bc_matrix_t *self)
{
    return self->_state;
}

void _bc_matrix_init_task(void *param)
{
    bc_matrix_t *self = (bc_matrix_t *) param;

    uint8_t i, j;
    bool pin_state;
    uint8_t bit_index;

    uint64_t inputs = 0;

    for (i = 0; i < self->_out_length; i++)
    {

        bc_gpio_set_mode(self->_out_gpio[i], BC_GPIO_MODE_OUTPUT);

        for (j = 0; j < self->_in_length; j++)
        {
            bit_index = i * self->_in_length + j;

            pin_state = bc_gpio_get_input(self->_in_gpio[j]);

            inputs |= (uint64_t) pin_state << bit_index;
        }

        bc_gpio_set_mode(self->_out_gpio[i], BC_GPIO_MODE_ANALOG);
    }

    self->_state = inputs & self->_debounce;

    self->_debounce = inputs;

    if (self->_state != self->_last_state)
    {
        if (self->_event_handler != NULL)
        {
            self->_event_handler(self, BC_MATRIX_EVENT_CHANGE, self->_event_param);
        }

        self->_last_state = self->_state;
    }
    else if (!self->_first)
    {
        if (self->_event_handler != NULL)
        {
            self->_event_handler(self, BC_MATRIX_EVENT_CHANGE, self->_event_param);
        }
    }

    self->_first = true;

    bc_scheduler_plan_current_relative(self->_scan_interval);
}
