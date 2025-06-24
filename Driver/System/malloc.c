#include "malloc.h"	   


__align(32) uint8_t mem1base[MEM1_MAX_SIZE];													
__align(32) uint8_t mem2base[MEM2_MAX_SIZE] __attribute__((at(0XC01F4000)));					
__align(32) uint8_t mem3base[MEM3_MAX_SIZE] __attribute__((at(0X10000000)));					

uint32_t mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													
uint32_t mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0XC01F4000+MEM2_MAX_SIZE)));	
uint32_t mem3mapbase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(0X10000000+MEM3_MAX_SIZE)));	
   
const uint32_t memtblsize[SRAM_BANK]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE,MEM3_ALLOC_TABLE_SIZE};	
const uint32_t memblksize[SRAM_BANK]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE,MEM3_BLOCK_SIZE};					
const uint32_t memsize[SRAM_BANK]={MEM1_MAX_SIZE,MEM2_MAX_SIZE,MEM3_MAX_SIZE};							


/* 内存设备 */
struct stru_mem_dev g_mem_dev =
{
	my_mem_init,						
	my_mem_perused,						
	mem1base,mem2base,mem3base,			
	mem1mapbase,mem2mapbase,mem3mapbase,
	0,0,0,  		 					
};



/**
  * @brief  内存复制：将源内存地址中的值复制到目的内存地址中
  * @param  [*des]:目的内存地址
  * @param  [*src]:源内存地址
  * @param  [n]:字节大小
  * @retval None
  */
void my_mem_cpy(void *des, void *src, uint32_t n)  
{  
    uint8_t *xdes = des;
	uint8_t *xsrc = src; 
    
    while(n--)
    {
        *xdes++ = *xsrc++;  
    }
}  



/**
  * @brief  内存设置：修改内存地址中的值
  * @param  [*s]:内存地址
  * @param  [c]:修改后的值
  * @param  [count]:字节大小
  * @retval None
  */
void my_mem_set(void *s, uint8_t c, uint32_t count)  
{  
    uint8_t *xs = s; 
    
    while(count--)
    {
        *xs++ = c;  
    }
}	



/**
  * @brief  内存初始化
  * @param  [memx]:初始化的内存区
  * @retval None
  */
void my_mem_init(uint8_t memx)  
{  
    my_mem_set(g_mem_dev.memmap[memx], 0, memtblsize[memx] * 4);	
 	g_mem_dev.memrdy[memx] = 1;								
}  



/**
  * @brief  获取内存占用
  * @param  [memx]:需要获取占用的内存区
  * @retval 占用大小
  */
uint16_t my_mem_perused(uint8_t memx)  
{  
    uint32_t used = 0;  
    uint32_t i; 
    
    for(i=0; i<memtblsize[memx]; i++)  
    {  
        if(g_mem_dev.memmap[memx][i])
        {
            used++; 
        }
    } 
    return (used*1000)/(memtblsize[memx]);  
}  



/**
  * @brief  内存申请
  * @param  [memx]:需要申请内存的内存区
  * @param  [size]:需要申请内存大小
  * @retval 申请结果，若为0xFFFFFFFF，申请失败，否则为申请地址
  */
uint32_t my_mem_malloc(uint8_t memx, uint32_t size)  
{  
    signed long offset=0;  
    uint32_t nmemb;	 
	uint32_t cmemb=0;
    uint32_t i;  
    
    if(!g_mem_dev.memrdy[memx])
    {
        g_mem_dev.init(memx);
    }
    
    if(size==0)
    {
        return 0XFFFFFFFF;
    }
    
    nmemb = size / memblksize[memx]; 
    
    if(size%memblksize[memx])
    {
        nmemb++;  
    }
    for(offset=memtblsize[memx]-1; offset>=0; offset--)
    {     
		if(!g_mem_dev.memmap[memx][offset])
        {
            cmemb++;
        }
		else 
        {
            cmemb=0;	
        }
        
		if(cmemb==nmemb)							
		{
            for(i=0;i<nmemb;i++)  					
            {  
                g_mem_dev.memmap[memx][offset+i]=nmemb;  
            }  
            return (offset*memblksize[memx]);
		}
    }  
    return 0XFFFFFFFF;
}  



/**
  * @brief  内存释放
  * @param  [memx]:需要释放内存的内存区
  * @param  [offset]:需要释放的内存大小
  * @retval 释放结果
  */
uint8_t my_mem_free(uint8_t memx, uint32_t offset)  
{  
    int i; 
    
    if(!g_mem_dev.memrdy[memx])
	{
		g_mem_dev.init(memx);    
        return 1;
    }  
    
    if(offset < memsize[memx])
    {  
        int index = offset / memblksize[memx];			
        int nmemb = g_mem_dev.memmap[memx][index];	
        for(i=0;i<nmemb;i++)  						
        {  
            g_mem_dev.memmap[memx][index+i] = 0;  
        }  
        return 0;  
    }
    else 
    {
        return 2;
    }
}  



/**
  * @brief  内存释放
  * @param  [memx]:需要释放内存的内存区
  * @param  [offset]:需要释放的地址
  * @retval NONE
  */
void my_free(uint8_t memx, void *ptr)  
{  
	uint32_t offset;  
    
	if(ptr == NULL)
    {
        return; 
    }        
    
 	offset = (uint32_t)ptr - (uint32_t)g_mem_dev.membase[memx];     
    my_mem_free(memx, offset);	 
}  



/**
  * @brief  申请内存
  * @param  [memx]:需要申请内存的内存区
  * @param  [offset]:需要申请的内存大小
  * @retval 申请的地址
  */
void *my_malloc(uint8_t memx, uint32_t size)  
{  
    uint32_t offset;   
	offset = my_mem_malloc(memx,size);  
    
    if(offset == 0XFFFFFFFF)
    {
        return NULL;  
    }
    else
    {
        return (void*)((uint32_t)g_mem_dev.membase[memx] + offset);  
    }
}  



/**
  * @brief  申请
  * @param  [memx]:需要申请内存的内存区
  * @param  [offset]:需要申请的内存大小
  * @retval 申请的地址
  */
void *myrealloc(uint8_t memx,void *ptr,uint32_t size)  
{  
    uint32_t offset;    
    offset = my_mem_malloc(memx,size);   	
    if(offset==0XFFFFFFFF)
    {
        return NULL; 
    }        
    else  
    {  									   
	    my_mem_cpy((void*)((uint32_t)g_mem_dev.membase[memx]+offset), ptr,size);	
        my_free(memx, ptr);  											  		
        return (void*)((uint32_t)g_mem_dev.membase[memx]+offset);              
    }  
}












