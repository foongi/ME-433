#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");

    volatile float f1, f2;
    volatile float f_add, f_sub, f_mult, f_div;
    
    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    
    absolute_time_t t1 = get_absolute_time();

    for(int i = 0; i < 1000; i++){
        f_add = f1+f2;
    }

    absolute_time_t t2 = get_absolute_time();
    
    for(int i = 0; i < 1000; i++){
        f_sub = f1-f2;
    }

    absolute_time_t t3 = get_absolute_time();
    
    for(int i = 0; i < 1000; i++){
        f_mult = f1*f2;
    }
    absolute_time_t t4 = get_absolute_time();
    
    for(int i = 0; i < 1000; i++){
        f_div = f1/f2;
    }

    absolute_time_t t5 = get_absolute_time();

    uint64_t start = to_us_since_boot(t1);
    uint64_t post_add = to_us_since_boot(t2);
    uint64_t post_sub = to_us_since_boot(t3);
    uint64_t post_mult = to_us_since_boot(t4);
    uint64_t post_div = to_us_since_boot(t5);
    
    
    printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);

    printf("Microseconds per operation (1000x): \n");
    printf("Addition: %llu\n", post_add-start);
    printf("Subtraction: %llu\n", post_sub-post_add);
    printf("Multiplication: %llu\n", post_mult-post_sub);
    printf("Division: %llu\n", post_div-post_mult);
    printf("\n");
    printf("Clock cycles per operation (roughly - including overhead): \n"); // Divide by 1000 to convert to time per 1 operation
    printf("Addition: %lf\n", (post_add-start)/6.667); // then divide again by 6.667ns to get to cycles 
    printf("Subtraction: %lf\n", (post_sub-post_add)/6.667); // Since we were in us, though, this nets out
    printf("Multiplication: %lf\n", (post_mult-post_sub)/6.667); // to just /6.667 b/c the 1000s cancel
    printf("Division: %lf\n", (post_div-post_mult)/6.667);\

    while(true) {
        ;
    }

}
