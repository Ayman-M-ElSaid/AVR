#include "lcd.h"

int main()
{
  LCD lcd;
  lcd_init_4bit(&lcd, 'A');

  lcd_goto(&lcd, 0, 0);
  lcd_print(&lcd, "Hello");
  lcd_goto(&lcd, 1, 0);
  lcd_print(&lcd, "World!");

  while (1)
  {
  }
  return 0;
}