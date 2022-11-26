
#include <../pico/stdlib.h>

void gpio_init(unsigned int pin)
{
    printf("GPIO %d INIT\n", pin);
}

void gpio_put(unsigned int pin, bool value)
{
    if(value)
    {
        printf("GPIO %d HIGH\n", pin);
    }
    else
    {
        printf("GPIO %d LOW\n", pin);
    }
}

void gpio_set_dir(unsigned int pin, bool out)
{
   if(out)
    {
        printf("GPIO %d OUT\n", pin);
    }
    else
    {
        printf("GPIO %d IN\n", pin);
    }
}