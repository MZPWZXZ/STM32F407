#ifndef __MALLOC_H
#define __MALLOC_H


#include "stm32f4xx.h" 


#ifndef NULL
#define NULL 0
#endif


#define SRAM_BANK 	3	
#define SRAM_IN	 	0		
#define SRAM_EX   	1		
#define SRAM_CCM  	2		


#define MEM1_MAX_SIZE			30 * 1024  						
#define MEM1_BLOCK_SIZE			64  	  						
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	

#define MEM2_MAX_SIZE			1 * 1024  	
#define MEM2_BLOCK_SIZE			64  	  									
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	

#define MEM3_MAX_SIZE			60 * 1024  					
#define MEM3_BLOCK_SIZE			64  	  							
#define MEM3_ALLOC_TABLE_SIZE	MEM3_MAX_SIZE/MEM3_BLOCK_SIZE 	
		 


struct stru_mem_dev{

	void      (*init)(uint8_t);	
    
	uint16_t  (*perused)(uint8_t);
    
	uint8_t   *membase[SRAM_BANK];	
    
	uint32_t  *memmap[SRAM_BANK]; 	
    
	uint8_t    memrdy[SRAM_BANK]; 
    
};
extern struct stru_mem_dev g_mem_dev;	



void    my_mem_set(void *s,uint8_t c,uint32_t count);	
void        my_mem_cpy(void *des,void *src,uint32_t n);
void     my_mem_init(uint8_t memx);				
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);	
uint8_t my_mem_free(uint8_t memx,uint32_t offset);		
uint16_t my_mem_perused(uint8_t memx) ;			


void my_free(uint8_t memx,void *ptr);  			
void *my_malloc(uint8_t memx,uint32_t size);			
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);



#endif


