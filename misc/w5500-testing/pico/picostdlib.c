
#include<picostdlib.h>

void gpio_init(uint pin)
{
    printf("GPIO %d INIT\n", pin);
}

void gpio_put(uint pin, bool value)
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

void gpio_set_dir(uint pin, bool out)
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