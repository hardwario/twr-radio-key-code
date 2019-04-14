#include <application.h>
#include <bc_matrix.h>

#define MULTIPLEKEYS -1


// LED instance
bc_led_t led;

// Matrix keyboard state
uint64_t matrix_state;
// Matrix keyboard instance
bc_matrix_t matrix;
// Layout of your keyboard
char keys[] = 
{
    '1','2','3','A',
    '4','5','6','B',
    '7','8','9','C',
    '*','0','#','D'
};

// Set output and input ports
bc_gpio_channel_t out_gpio[]  = {BC_GPIO_P2, BC_GPIO_P3, BC_GPIO_P4, BC_GPIO_P5};
bc_gpio_channel_t in_gpio[]  = {BC_GPIO_P6, BC_GPIO_P7, BC_GPIO_P8, BC_GPIO_P9};

char codeBuffer[10];

#define OUT_GPIO_LENGTH (sizeof(out_gpio)/sizeof(bc_gpio_channel_t))
#define IN_GPIO_LENGTH (sizeof(in_gpio)/sizeof(bc_gpio_channel_t))

void matrix_event_handler(bc_matrix_t *self, bc_matrix_event_t event, void *event_param)
{
    char pressed_key[2];

    // Get current pressed key
    matrix_state = bc_matrix_get_state(&matrix);
    if (!matrix_state)
    {
        return;
    }   
    else if (matrix_state & (1 << 14))
    {
        bc_radio_pub_string("code", codeBuffer);
        memset(codeBuffer, 0, sizeof(codeBuffer));
        return;
    }
    else if (matrix_state & (1 << 12))
    {
        memset(codeBuffer, 0, sizeof(codeBuffer));
        return;
    }
    bc_led_pulse(&led, 30);

    int keyIndex = getKey(matrix_state);

    if (keyIndex == MULTIPLEKEYS)
    {
        return;
    }
    pressed_key[0] = keys[keyIndex];
    pressed_key[1] = '\0';
    strncat(codeBuffer, pressed_key, sizeof(codeBuffer));
}

int getKey(uint64_t keyCode)
{
    uint32_t total_zero_count = 0;
    int relative_position = __CLZ((uint32_t)keyCode);

    total_zero_count = (__CLZ((uint32_t)(keyCode) << (relative_position + 1)) + relative_position) + 1;

    if (total_zero_count < (sizeof(uint32_t) * 8))
    {
        return MULTIPLEKEYS;
    }
    else
    {
        return 31 - relative_position;
    }
}

void application_init(void)
{
    // Initialize matrix keyboard and clear code buffer that is storing password
    bc_matrix_init(&matrix, out_gpio, OUT_GPIO_LENGTH, in_gpio, IN_GPIO_LENGTH);
    bc_matrix_set_event_handler(&matrix, matrix_event_handler, NULL);
    memset(codeBuffer, 0, sizeof(codeBuffer));


    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_pulse(&led, 30);

    bc_radio_init(BC_RADIO_MODE_NODE_SLEEPING);
    bc_radio_pairing_request("code-terminal", VERSION);
}