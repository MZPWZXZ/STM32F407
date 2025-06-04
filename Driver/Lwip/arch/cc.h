#ifndef __CC_H__
#define __CC_H__


#include <stdlib.h>
#include <stdio.h>


typedef int sys_prot_t;



#if defined (__GNUC__) & !defined (__CC_ARM)
    #define LWIP_TIMEVAL_PRIVATE 0
    #include <sys/time.h>
#endif

//当定义了 OS_CRITICAL_METHOD 时就说明使用了 UCOS II
#if OS_CRITICAL_METHOD == 1
#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev) CPU_INT_DIS()
#define SYS_ARCH_UNPROTECT(lev) CPU_INT_EN()
#endif


#if OS_CRITICAL_METHOD == 3 
#define SYS_ARCH_DECL_PROTECT(lev) u32_t lev
//UCOS II 中进入临界区,关中断
#define SYS_ARCH_PROTECT(lev) lev = OS_CPU_SR_Save() 
//UCOS II 中退出 A 临界区，开中断
#define SYS_ARCH_UNPROTECT(lev) OS_CPU_SR_Restore(lev)
#endif


/* define compiler specific symbols */
#if defined (__ICCARM__)
    #define PACK_STRUCT_BEGIN
    #define PACK_STRUCT_STRUCT 
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(x) x
    #define PACK_STRUCT_USE_INCLUDES
#elif defined (__GNUC__)
    #define PACK_STRUCT_BEGIN
    #define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(x) x
#elif defined (__CC_ARM)
    #define PACK_STRUCT_BEGIN __packed
    #define PACK_STRUCT_STRUCT
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(x) x
#elif defined (__TASKING__)
    #define PACK_STRUCT_BEGIN
    #define PACK_STRUCT_STRUCT
    #define PACK_STRUCT_END
    #define PACK_STRUCT_FIELD(x) x
#endif


//LWIP 用 printf 调试时使用到的一些类型
#define U16_F "4d"
#define S16_F "4d"
#define X16_F "4x"
#define U32_F "8ld"
#define S32_F "8ld"
#define X32_F "8lx"


#ifndef LWIP_PLATFORM_ASSERT
    /* Define random number generator function */
    #define LWIP_PLATFORM_ASSERT(x) do{printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); } while(0)
#endif

#define LWIP_RAND() ((u32_t)rand())

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)
#endif


#endif /* __CC_H__ */
