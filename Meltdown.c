#include <stdlib.h>
#include <emmintrin.h> /*Needed for _mm_cflush*/
#include <unistd.h> /*Needed for fork*/


#define THRESHOLD_CYCLES 300


//Create 256 pages of size 4KB
static int probe_array[256 * 4096];
static int histogram[256];

void flush_probe_array()
{
    for(int i = 0; i < 256; i++)
        _mm_clflush(&probe_array[i * 4096]);
}


void meltdown_attack(unsigned long long kernel_address)
{
    pid_t proc = fork();

    if(proc == 0){
        //TODO MELTDOWN ATTACK ASSEMBLY 
    }
    else{
        //TODO REPT 
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

    //Make sure our probe_array does not contain gargabe
    for(int i = 0; i < 256; i++)
        probe_array[i * 4096] = 0;

    return 1;
}
