

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/float.h"  // Required for using single-precision variables.
#include "pico/double.h" // Required for using double-precision variables.

/*
 * @brief EXAMPLE - HELLO_C
 *        Simple example to initialise the IOs and then 
 *        print a "Hello World!" message to the console.
 * 
 * @return int  Application return code (zero for success).
 */

float Wallis_float(int count)
{
     float val = 1.0;
     float temp=1.0;
     float temp1=1.0;
     float temp2=1.0;
     for (float i = 1; i <= count; i++)
     {
         
        temp1 = ((2 * i) / ((2 * i) - 1));  //breaking down the general term into 2 parts to get the desired error
        temp2 = ((2 * i) / ((2 * i) + 1));
        temp*=temp1*temp2;
     }
     val = 2 * temp;  
                                                          //as val return the the value of pi/2
     return val;
}

double Wallis_double(int count)
{
    double val = 1.0;
    double temp=1.0;
    for (double i = 1; i <=count; i++)
    {
        temp *= ((2 * i) / ((2 * i) - 1)) * ((2 * i) / ((2 * i) + 1));   // the expression for general term of wallis product
        
    }
    val = 2 * temp;                                                     //as the temp computes the value of pi/2
    return val;
}

int main()
{ 
    #ifndef WOKWI
    // Initialise the IO as we will be using the UART
    // Only required for hardware and not needed for Wokwi
    stdio_init_all();
    #endif
    
    double defaultVal= 3.14159265359;
    int count = 100000;

    float finalFloat = Wallis_float(count);
    float errorFloat = ((defaultVal - finalFloat)/defaultVal)*100;
    printf("The Calculate value for PI  using single-precision representation is = %f \n", finalFloat);
    printf("The approximation error for the single-precision representation is = %f \n", errorFloat);

    double finalDouble = Wallis_double(count);
    double errorDouble = ((defaultVal - finalDouble)/defaultVal)*100;

    
    
    printf("The Calculate value for PI  using double-precision representation is = %f \n", finalDouble);
    printf("The approximation error for the double-precision representation is = %f \n", errorDouble);

    
    return 0;
}