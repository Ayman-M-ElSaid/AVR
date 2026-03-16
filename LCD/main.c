#include "lcd.h"

int main()
{
  LCD lcd;
  lcd_init_4bit(&lcd, 'A');

  while (1)
  {
    for (uint8_t column = 0; column <= 16; column++)
    {
      lcd_clear(&lcd);
      lcd_goto(&lcd, 0, column);
      lcd_print(&lcd, "Eid");
      lcd_goto(&lcd, 1, column + 1);
      lcd_print(&lcd, "Mubarak!");
      _delay_ms(100);
    }
  }
  return 0;
}
