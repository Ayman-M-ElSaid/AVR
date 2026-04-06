#include "lcd.h"
#include "keypad.h"
#include <string.h>

LCD lcd;
Keypad keypad;

void lcd_backspace()
{
  lcd_I2C_write(&lcd, false, 0x10);
  lcd_I2C_write(&lcd, true, ' ');
  lcd_I2C_write(&lcd, false, 0x10);
}

float calc(float a, float b, char op)
{
  switch (op)
  {
  case '+':    return a + b;    break;
  case '-':    return a - b;    break;
  case 'x':    return a * b;    break;
  case '/':    return a / b;    break;
  default:
    lcd_goto(&lcd, 1, 3);
    lcd_print(&lcd, "Syntax Error");
    break;
  }
}

void display_answer(float result)
{
  char ans[16];
  sprintf(ans, "%g", result);
  lcd_goto(&lcd, 1, 16 - strlen(ans));
  lcd_print(&lcd, ans);
  return;
}

int main()
{
  lcd_init_I2C(&lcd, 0x27);
  keypad_init(&keypad, 'A');
  lcd_I2C_write(&lcd, false, 0x0E);

  char exp[16] = {0};
  uint8_t count = 0;
  uint8_t key;
  while (1)
  {

    if ((key = keypad_get_pressed(&keypad)))
    {
      if (key == 0x3D)
      {
        // Hide Cursor 
        lcd_I2C_write(&lcd, false, 0x0C);

        // Sort Numbers and Operator
        float nums[10]={0};
        char ops[10];
        char *ptr = exp;
        uint8_t i = 0;
        while (ptr < exp+count)
        {
          if (*ptr >= 0x30 && *ptr < 0x40)
            nums[i] = nums[i] * 10 + (*ptr - 0x30);
          else
            ops[i++] = *ptr;
          ptr++;
        }

        // Calculate the Answer
        for (uint8_t j = 0; j < i; j++)
        {
          nums[0] = calc(nums[0], nums[1], ops[j]);
          nums[1] = nums[j + 2];
        }
        display_answer(nums[0]);

        // Wait for Button Press and Reset
        do
        {
          key = keypad_get_pressed(&keypad);
        } while (key == 0 || key == 0x3D || key == 0x08);
        memset(exp, 0, sizeof(exp));
        lcd_clear(&lcd);
        // Show Curser
        lcd_I2C_write(&lcd, false, 0x0E);

        count=0;
        lcd_I2C_write(&lcd, true, key);
        exp[count++] = key;
      }
      else if (key == 0x08)
      {
        exp[count--] = 0;
        lcd_backspace();
      }
      else
      {
        lcd_I2C_write(&lcd, true, key);
        exp[count++] = key;
      }
    }
  }

  return 0;
}
