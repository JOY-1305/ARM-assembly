

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/float.h"  // Required for using single-precision variables.
#include "pico/double.h" // Required for using double-precision variables.
// Must declare the main assembly entry point before use.
// void main_asm();
/*
 * @brief EXAMPLE - HELLO_C
 *        Simple example to initialise the IOs and then 
 *        print a "Hello World!" message to the console.
 * 
 * @return int  Application return code (zero for success).
 */

 double count = 3.14159265359;
  int defaultVal = 100000;

  // Function to get the enable status of the XIP cache
bool get_xip_cache_en()
{

    uint32_t *addr = (uint32_t *)(XIP_CTRL_BASE);
    return addr[0];
}

// Function to set the enable status of the XIP cache
bool set_xip_cache_en(bool cache_en)
{

    uint32_t *addr = (uint32_t *)(XIP_CTRL_BASE);
    addr[0] = cache_en;
    return addr[0];
}

float Wallis_float(int count)
{
     float val = 1.0;
     float temp=1.0;
     float temp1=1.0;
     float temp2=1.0;
     

     for (float i = 1; i <= count; i++){
         
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
float fin_res = 0.0;
void core1_entry()
{
    while (1)
    {
        // passes the function pointer through fifo
        // saves the time before the function is called
        uint64_t core_stTime = time_us_64();

        // One incoming int as a parameter and return float

        float (*func)() = (float (*)())multicore_fifo_pop_blocking();
        int parameter = multicore_fifo_pop_blocking();

        // value returned by float is pushed back in the fifo stack
        float result = (*func)(parameter);
        fin_res = result;
        // saves time after executing function
        uint64_t core_stpTime = time_us_64();
        // calculates execution time of function
        float t = (core_stpTime - core_stTime);
        // pushes the return value of function in the fifo stack 
        // the calculated value of time is pushed on fifo stack
        multicore_fifo_push_blocking(t);
    }
}


int main()
{ 
    
    #ifndef WOKWI                   // Initialises the IO since UART is used 
    
    stdio_init_all();
    #endif
    set_xip_cache_en(0);
    uint64_t start_time = time_us_64();         //initial time to calculate sequential execution time 
    uint64_t start_single = time_us_64();       //initial time to calculate single precision wallis
    float reg1 = Wallis_float(defaultVal);

    uint64_t stop_single = time_us_64();        //final time at which the single precision ends
    float reg2 = (stop_single - start_single);  //calculates  the single precision executing time

    double err_sp = fabs(count - reg1)/count;   //calculates the error for single precision
    err_sp *= 100;

    printf("Execution Time : %lu\n", (unsigned long)reg2);
    printf("Percentage Single-precision Error : %0.11lf\n", err_sp);

    uint64_t start_double = time_us_64();   // starting time to calculate sequential double precision
    double reg3 = Wallis_double(defaultVal);       

    uint64_t stop_double = time_us_64();        //time at which the sequential double precision stops

    float reg4 = (stop_double - start_double);         // calculates error  for double precision
    double err_dp = fabs(count - reg3)/count ; 

    err_dp *= 100;

    printf("double precision float value  : %lf\n", reg3);
    printf("Execution Time : %lu\n", (unsigned long)reg4);
    printf("double precision percentage error : %0.11lf\n", err_dp);

    uint64_t seq_stop = time_us_64();   //final time at which sequential execution time is calculated
    float tot_seq_time = (seq_stop - start_time);
    printf ("runtime using single-core : %lu\n", tot_seq_time);

    uint64_t runT_strt_pl_core = time_us_64 ();  //starting time for parallel processing program

    multicore_launch_core1(core1_entry); 

    multicore_fifo_push_blocking((uintptr_t)&Wallis_float);       //pushes the function pointer onto the stack

    multicore_fifo_push_blocking(defaultVal);           //pushes the parameter of function on the stack

                                                // pops  the time from core 1 fifo stack

                                                // store return values of single precision wallis product
    start_double = time_us_64();
    reg2 = Wallis_double(defaultVal);
    stop_double = time_us_64();

    reg4 = (stop_double - start_double);
    err_dp = fabs(count - reg2)/count;

    err_dp *= 100 ; 

    printf("double precision float val : %lf\n", reg2);
    printf("Execution Time : %lu\n", (unsigned long)reg4);
    printf("double precision float error : %0.11lf\n", err_dp);

    uint64_t runT_stp_pl_core = time_us_64() ;    // final time at which parallel core program fires up 
    float time = (float)multicore_fifo_pop_blocking();      // calculates the time for prallel core 
    printf("Execution Time of single-precision multicore: %lu\n", (unsigned long)time);

    float tot_time = (runT_stp_pl_core - runT_strt_pl_core);
    printf("Total runtime using parallel-cores : %lu\n", tot_time);

    //enabling the XIP

    set_xip_cache_en(1);
    start_time = time_us_64();         //initial time to calculate sequential execution time 
    start_single = time_us_64();       //initial time to calculate single precision wallis
    reg1 = Wallis_float(defaultVal);

    stop_single = time_us_64();        //final time at which the single precision ends
    reg2 = (stop_single - start_single);  //calculates  the single precision executing time

    err_sp = fabs(count - reg1)/count;   //calculates the error for single precision
    err_sp *= 100;

    printf("Execution Time when cache is enabled : %lu\n", (unsigned long)reg2);
    printf("Percentage Single-precision Error : %0.11lf\n", err_sp);

    start_double = time_us_64();   // starting time to calculate sequential double precision
    reg3 = Wallis_double(defaultVal);       

    stop_double = time_us_64();        //time at which the sequential double precision stops

    reg4 = (stop_double - start_double);         // calculates error  for double precision
    err_dp = fabs(count - reg3)/count ; 

    err_dp *= 100;

    printf("double precision float value  : %lf\n", reg3);
    printf("Execution Time when cache is enabled: %lu\n", (unsigned long)reg4);
    printf("double precision percentage error : %0.11lf\n", err_dp);

    seq_stop = time_us_64();   //final time at which sequential execution time is calculated
    tot_seq_time = (seq_stop - start_time);
    printf ("runtime using single-core when cache is enabled : %lu\n", tot_seq_time);

    runT_strt_pl_core = time_us_64 ();  //starting time for parallel processing program

    // multicore_launch_core1(core1_entry); 

    multicore_fifo_push_blocking((uintptr_t)&Wallis_float);       //pushes the function pointer onto the stack

    multicore_fifo_push_blocking(defaultVal);           //pushes the parameter of function on the stack

                                                // pops  the time from core 1 fifo stack

                                                // store return values of single precision wallis product
    start_double = time_us_64();
    reg2 = Wallis_double(defaultVal);
    stop_double = time_us_64();

    reg4 = (stop_double - start_double);
    err_dp = fabs(count - reg2)/count;

    err_dp *= 100 ; 

    printf("double precision float val : %lf\n", reg2);
    printf("Execution Time when cache is enabled : %lu\n", (unsigned long)reg4);
    printf("double precision float error : %0.11lf\n", err_dp);

    runT_stp_pl_core = time_us_64() ;    // final time at which parallel core program fires up 
    time = (float)multicore_fifo_pop_blocking();      // calculates the time for prallel core 
    printf("Execution Time of single-precision multicore when cache is enabled: %lu\n", (unsigned long)time);

    tot_time = (runT_stp_pl_core - runT_strt_pl_core);
    printf("Total runtime using parallel-cores when cache is enabled : %lu\n", tot_time);
    
    return 0;
}
