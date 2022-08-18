#include <application.h>
#include <twr_matrix.h>

#define MULTIPLEKEYS -1

// LED instance
twr_led_t led;

// Matrix keyboard state
uint64_t matrix_state;
// Matrix keyboard instance
twr_matrix_t matrix;
// Layout of your keyboard
char keys[] =
{
    '1','2','3','A',
    '4','5','6','B',
    '7','8','9','C',
    '*','0','#','D'
};

// Set output and input ports
twr_gpio_channel_t out_gpio[]  = {TWR_GPIO_P2, TWR_GPIO_P3, TWR_GPIO_P4, TWR_GPIO_P5};
twr_gpio_channel_t in_gpio[]  = {TWR_GPIO_P6, TWR_GPIO_P7, TWR_GPIO_P8, TWR_GPIO_P9};

char codeBuffer[10];

#define OUT_GPIO_LENGTH (sizeof(out_gpio)/sizeof(twr_gpio_channel_t))
#define IN_GPIO_LENGTH (sizeof(in_gpio)/sizeof(twr_gpio_channel_t))

void matrix_event_handler(twr_matrix_t *self, twr_matrix_event_t event, void *event_param)
{
    char pressed_key[2];

    // Get current pressed key
    matrix_state = twr_matrix_get_state(&matrix);
    if (!matrix_state)
    {
        return;
    }
    else if (matrix_state & (1 << 14))
    {
        twr_radio_pub_string("code", codeBuffer);
        memset(codeBuffer, 0, sizeof(codeBuffer));
        return;
    }
    else if (matrix_state & (1 << 12))
    {
        memset(codeBuffer, 0, sizeof(codeBuffer));
        return;
    }
    twr_led_pulse(&led, 30);

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
    // Initialize Battery Module
    twr_module_battery_init();

    // Initialize matrix keyboard and clear code buffer that is storing password
    twr_matrix_init(&matrix, out_gpio, OUT_GPIO_LENGTH, in_gpio, IN_GPIO_LENGTH);
    twr_matrix_set_event_handler(&matrix, matrix_event_handler, NULL);
    memset(codeBuffer, 0, sizeof(codeBuffer));

    // Initialize LED
    twr_led_init(&led, TWR_GPIO_LED, false, false);
    twr_led_pulse(&led, 30);

    twr_radio_init(TWR_RADIO_MODE_NODE_SLEEPING);
    twr_radio_pairing_request("code-terminal", FW_VERSION);
}
