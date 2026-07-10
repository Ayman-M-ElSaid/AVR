#include "lcd.h"
#include "keypad.h"
#include "dht11.h"
#include "oled.h"
#include <avr\interrupt.h>

#define NEXT_BUTTON PD2
#define CONFIRM_BUTTON PD3
#define BUZZER PD4
#define RED PD5
#define GREEN PD6
#define LIGHT1 PB0
#define LIGHT2 PB1
#define LIGHT3 PB2
#define MOTOR PB3

LCD lcd;
Keypad keypad;
OLED oled;
DHT11 dht;
DHT11_Data dht_data;
typedef enum
{
    MENU,
    TEMP,
    LIGHT,
    FAN,
    PASSWORD,
} State;

bool is_locked = true;
bool auto_fan = false;
char PSWD[] = {'1', '1', '0', '4'};
char pswd[4] = {0};
char *ptr = pswd;
char buf[20];
uint8_t login_attempts = 3;
uint8_t set_temp = 30;

volatile uint8_t flag_2sec = 0;
State current_state = TEMP;
uint8_t cursor_positions[] = {2, 3, 4, 6};
uint8_t *cursor_position = cursor_positions;

const uint8_t sun[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x20, 0x70, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
  0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x01, 0xe0, 0xf0, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xfe, 0xfc, 0xfc, 0xf0, 0xe0, 0x01, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x80, 0x07, 0x0f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x0f, 0x07, 0x80, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x0e, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8,
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0e, 0x04, 0x00, 0x00, 0x00, 0x00,
};
const uint8_t cloudy[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x06, 0x06, 0x00, 0xe0, 0xf8, 0xfc, 0xfe, 0xfe, 0xff, 0x7f, 0x3f, 0x1f, 0xcf, 0xc6, 0xe6,
  0xe4, 0xf0, 0xf0, 0xf0, 0xe6, 0xe6, 0xc3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x20, 0x30, 0x30, 0x00, 0x03, 0x0f, 0x0f, 0xc7, 0xe3, 0xf3, 0xf8, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0xf8, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
  0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 0x0f, 0x07, 0x03,
};
const uint8_t cactus[] PROGMEM = {
  0x00, 0x00, 0x80, 0xe0, 0xf0, 0xf0, 0xf0, 0xe0, 0xc0, 0x00, 0x00, 0xf8, 0xfc, 0xfe, 0xff, 0xff,
  0xff, 0xff, 0xfe, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xf8, 0xfe, 0xfe, 0xff, 0xfe, 0xfe, 0xf8, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x0f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xc1, 0xc1, 0xc1, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void enter_pswd();
void check_pswd();
void timer1_init();
void check_button_presses();
void move_cursor();
void change_state(State state);
void update_oled();
const uint8_t *match_icon();
void change_pswd();

int main()
{
    /* Outputs */
    DDRD |= (1 << BUZZER) | (1 << RED) | (1 << GREEN);
    DDRB |= (1 << LIGHT1) | (1 << LIGHT2) | (1 << LIGHT3) | (1 << MOTOR);
    /* Inputs */
    DDRD &= ~((1 << NEXT_BUTTON) | (1 << CONFIRM_BUTTON));
    PORTD |= ((1 << NEXT_BUTTON) | (1 << CONFIRM_BUTTON));

    lcd_init_I2C(&lcd, 0x27);
    keypad_init(&keypad, 'A');
    oled_init(&oled, 0x3C);
    lcd_print(&lcd, "Password:");
    dht11_init(&dht, 'C', 2);
    timer1_init();

    while (1)
    {

        if (is_locked)
        {
            PORTD = (PORTD | 1 << RED) & ~(1 << GREEN);

            if (login_attempts)
                enter_pswd();
            else
            {
                lcd_clear(&lcd);
                lcd_print(&lcd, "Alarm System On");
                PORTD |= (1 << BUZZER) | (1 << RED);
                _delay_ms(350);
                PORTD &= ~((1 << BUZZER) | (1 << RED));
                _delay_ms(350);
            }
        }
        else
        {
            PORTD = (PORTD | 1 << GREEN) & ~(1 << RED);

            check_button_presses();

            if (flag_2sec && current_state == TEMP)
            {
                dht11_read(&dht, &dht_data);
                update_oled();
                flag_2sec = 0;
            }

            if (auto_fan)
            {
                if (dht_data.temperature > set_temp)
                    PORTB |= 1 << MOTOR;
                else
                    PORTB &= ~(1 << MOTOR);
            }
        }
    }
    return 0;
}

void enter_pswd()
{
    uint8_t key;
    if ((key = keypad_get_pressed(&keypad)))
    {
        if (key == 0x3D)
        {
            check_pswd();
        }
        else if (key == 0x08)
        {
            if (ptr > pswd)
            {
                ptr--;
                *ptr = 0;
                /* LCD backspace */
                lcd_I2C_write(&lcd, false, 0x10); // cursor left
                lcd_I2C_write(&lcd, true, ' ');   // write empty char
                lcd_I2C_write(&lcd, false, 0x10); // cursor left
            }
        }
        else
        {
            lcd_I2C_write(&lcd, true, '*');
            *ptr = key;
            ptr++;
        }
    }
}

void check_pswd()
{
    if (sizeof(pswd) != sizeof(PSWD))
        goto wrong_pswd;

    for (uint8_t i = 0; i < 4; i++)
    {
        if (pswd[i] != PSWD[i])
            goto wrong_pswd;
    }

    // Correct Password
    is_locked = false;
    current_state = TEMP;
    lcd_clear(&lcd);
    lcd_print(&lcd, "System Disarmed");
    memset(pswd, 0, sizeof(pswd));
    ptr = pswd;
    return;

wrong_pswd:
    lcd_clear(&lcd);
    lcd_print(&lcd, "Incorrect");
    lcd_goto(&lcd, 1, 0);
    lcd_print(&lcd, "Alert in: ");
    lcd_I2C_write(&lcd, true, --login_attempts + 0x30);
    _delay_ms(250);
    PORTD |= 1 << BUZZER;
    _delay_ms(1000);
    PORTD &= ~(1 << BUZZER);
    if (login_attempts)
    {
        lcd_clear(&lcd);
        lcd_print(&lcd, "Password:");
    }
    memset(pswd, 0, sizeof(pswd));
    ptr = pswd;
    return;
}

void timer1_init()
{
    TCNT1 = 49911;
    TIMSK |= (1 << TOIE1);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    sei();
}

void check_button_presses()
{

    if (!(PIND & ((1 << NEXT_BUTTON))))
    {
        if (current_state == TEMP)
            change_state(MENU);
        else if (current_state != PASSWORD)
        {
            move_cursor();
            _delay_ms(100);
            while (!(PIND & ((1 << NEXT_BUTTON))))
                ;
        }
    }

    if (!(PIND & (1 << CONFIRM_BUTTON)))
    {
        switch (current_state)
        {
        case TEMP:
            change_state(MENU);
            break;

        case MENU:
            if (*cursor_position != 6)
                change_state(*cursor_position);
            else
                change_state(TEMP);
            break;

        case LIGHT:
            if (*cursor_position != 6)
                PORTB ^= (1 << ((*cursor_position) - 2));
            else
                change_state(MENU);
            break;

        case FAN:
            switch (*cursor_position)
            {
            case 2:
                PORTB ^= (1 << MOTOR);
                break;
            case 3:
                auto_fan = !auto_fan;
                snprintf(buf, sizeof(buf), "Auto | %d", auto_fan);
                oled_write_string(&oled, 3, 8, buf);
                break;
            case 4:
                set_temp += 5;
                snprintf(buf, sizeof(buf), "Set Temperature|%d", set_temp);
                oled_write_string(&oled, 4, 8, buf);
                if (set_temp > 50)
                    set_temp = 0;
                break;
            case 6:
                change_state(MENU);
                break;
            }
            break;

        case PASSWORD:
            change_state(MENU);
            break;
        }
        _delay_ms(100);
        while (!(PIND & (1 << CONFIRM_BUTTON)))
            ;
    }
}

void move_cursor()
{
    oled_write_char(&oled, *cursor_position, 0, ' ');
    cursor_position++;
    if (cursor_position > cursor_positions + 3)
        cursor_position = cursor_positions;
    oled_write_char(&oled, *cursor_position, 0, '>');
}

void change_state(State state)
{
    oled_clear(&oled);
    current_state = state;
    cursor_position = cursor_positions;
    update_oled();
    oled_write_char(&oled, 2, 0, '>');
}

void update_oled()
{
    switch (current_state)
    {
    case TEMP:
        oled_write_string(&oled, 0, 0, "Welcome Home");
        oled_write_string(&oled, 1, 0, "___________________");
        snprintf(buf, sizeof(buf), "Humidty: %u%%RH", dht_data.humidity);
        oled_write_string(&oled, 2, 0, buf);
        snprintf(buf, sizeof(buf), "Temperature: %uC", dht_data.temperature);
        oled_write_string(&oled, 3, 0, buf);
        oled_draw_bitmap(&oled, 4, 48, 32, 32, match_icon());
        break;

    case MENU:
        oled_write_string(&oled, 0, 0, "Main Menu");
        oled_write_string(&oled, 1, 0, "___________________");
        oled_write_string(&oled, 2, 8, "Lighting");
        oled_write_string(&oled, 3, 8, "Fan");
        oled_write_string(&oled, 4, 8, "Change Password");
        oled_write_string(&oled, 6, 8, "Exit");
        break;

    case LIGHT:
        oled_write_string(&oled, 0, 0, "Lighting");
        oled_write_string(&oled, 1, 0, "___________________");
        oled_write_string(&oled, 2, 8, "Light 1");
        oled_write_string(&oled, 3, 8, "Light 2");
        oled_write_string(&oled, 4, 8, "Light 3");
        oled_write_string(&oled, 6, 8, "Exit");
        break;
    case FAN:
        oled_write_string(&oled, 0, 0, "Fan");
        oled_write_string(&oled, 1, 0, "___________________");
        oled_write_string(&oled, 2, 8, "ON/OFF");
        snprintf(buf, sizeof(buf), "Auto | %d", auto_fan);
        oled_write_string(&oled, 3, 8, buf);
        snprintf(buf, sizeof(buf), "Set Temperature|%d", set_temp);
        oled_write_string(&oled, 4, 8, buf);
        oled_write_string(&oled, 6, 8, "Exit");
        break;
    case PASSWORD:
        oled_write_string(&oled, 0, 0, "Change Password");
        oled_write_string(&oled, 1, 0, "___________________");
        oled_write_string(&oled, 2, 0, "Current Pswd:");
        oled_write_string(&oled, 3, 0, "New Pswd:");
        oled_write_string(&oled, 4, 0, "Confirm Pswd:");
        oled_write_string(&oled, 6, 8, "Exit");
        oled_write_char(&oled, 6, 0, '>');
        change_pswd();
        break;
    }
}

const uint8_t *match_icon()
{
    if (dht_data.humidity > 70)
        return cloudy;
    else if ((dht_data.temperature > 32) && (dht_data.humidity > 40))
        return sun;
    else if ((dht_data.temperature > 32) && (dht_data.humidity < 40))
        return cactus;
    else
        return sun;
}

void change_pswd()
{
    char old_pswd[4] = {0};
    char new_pswd[4] = {0};
    char confirm_new_pswd[4] = {0};
    char *bufs[3] = {old_pswd, new_pswd, confirm_new_pswd};
    char *ptr = old_pswd;
    uint8_t line_start[] = {13, 9, 13}; 
    uint8_t line_length[] = {0, 0, 0}; 
    int8_t i = 0;
    uint8_t key;

use_keypad:
    if ((key = keypad_get_pressed(&keypad)))
    {
        if (key == 0x3D)
        {
            if (i == 0)
                goto check_pswd;
            else if (i == 1) // ← was missing!
            {
                i = 2;
                ptr = confirm_new_pswd;
                goto use_keypad;
            }
            else if (i == 2)
                goto check_new_pswd;
        }
        else if (key == 0x08) 
        {
            // Clear display asterisks
            for (uint8_t j = 0; j < line_length[i]; j++)
                oled_write_char(&oled, i + 2, 6 * (line_start[i] + j), ' ');
            // Reset buffer and pointer
            memset(bufs[i], 0, 4);
            ptr = bufs[i];
            line_length[i] = 0; 
        }
        else
        {
            oled_write_char(&oled, i + 2, 6 * (line_start[i] + line_length[i]), '*');
            line_length[i]++;
            *ptr = key;
            ptr++;
        }
    }
    goto use_keypad;

check_pswd:
    for (uint8_t j = 0; j < 4; j++)
    {
        if (old_pswd[j] != PSWD[j])
        {
            oled_clear_page(&oled, 2);
            oled_clear_page(&oled, 3);
            oled_clear_page(&oled, 4);
            oled_write_string(&oled, 3, 0, "Wrong Password!");
            _delay_ms(1500);
            goto exit;
        }
    }
    i = 1;
    ptr = new_pswd;
    goto use_keypad;

check_new_pswd:
    for (uint8_t j = 0; j < 4; j++)
    {
        if (confirm_new_pswd[j] != new_pswd[j])
        {
            oled_clear_page(&oled, 2);
            oled_clear_page(&oled, 3);
            oled_clear_page(&oled, 4);
            oled_write_string(&oled, 3, 0, "Passwords don't match!");
            _delay_ms(1500);
            goto exit;
        }
    }
    for (uint8_t j = 0; j < 4; j++)
        PSWD[j] = confirm_new_pswd[j];
    oled_clear(&oled);
    oled_write_string(&oled, 3, 0, "Changed Successfully");
    _delay_ms(1500);

exit:
    change_state(MENU);
}

ISR(TIMER1_OVF_vect)
{
    TCNT1 = 49911;
    flag_2sec = 1;
}
