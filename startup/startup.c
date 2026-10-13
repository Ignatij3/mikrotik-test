#define NULL 0
typedef unsigned int uint32_t;
extern uint32_t _ebss,_sbss,_etext,_estack,_sidata,_sdata, _edata;
    
extern int main(void);

void Reset_Handler(void);
void NMI_Handler(void)              __attribute__((alias("Default_Handler")));
void HardFault_Handler(void)        __attribute__((alias("Default_Handler")));
void MemManage_Handler(void)        __attribute__((alias("Default_Handler")));
void BusFault_Handler(void)         __attribute__((alias("Default_Handler")));
void UsageFault_Handler(void)       __attribute__((alias("Default_Handler")));
void SVC_Handler(void)              __attribute__((alias("Default_Handler")));
void DebugMon_Handler(void)         __attribute__((alias("Default_Handler")));
void PendSV_Handler(void)           __attribute__((alias("Default_Handler")));
void SysTick_Handler(void)          __attribute__((alias("Default_Handler")));

uint32_t *const MSP_Value = (uint32_t *)&_estack;

uint32_t *Vector_Table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *)MSP_Value,                          
    (uint32_t *)Reset_Handler,                    
    (uint32_t *)NMI_Handler,                    
    (uint32_t *)HardFault_Handler,                
    (uint32_t *)MemManage_Handler,                 
    (uint32_t *)BusFault_Handler,                 
    (uint32_t *)UsageFault_Handler, 
    0,
    0,
    0,
    0, 
    (uint32_t *)SVC_Handler,
    (uint32_t *)DebugMon_Handler,
    0,
    (uint32_t *)PendSV_Handler,
    (uint32_t *)SysTick_Handler
};

static void System_Intitialization(void){
    /* Clock intitialization */

}

void Reset_Handler(void){
    uint32_t Sec_size = 0;    
    uint32_t *MemSrc = NULL;
    uint32_t *MemDes = NULL;
    /*Copy .data from flash to ram*/
    Sec_size = &_edata - &_sdata;
    MemSrc = (uint32_t *)&_sidata;
    MemDes = (uint32_t *)&_sdata;
    for(uint32_t counter = 0; counter < Sec_size; counter++)
    {
        *MemDes = *MemSrc;
        *MemDes++;
        *MemSrc++;
    }
    /*Initialize .bss with zeros*/
    Sec_size = &_ebss - &_sbss;
    MemDes = (uint32_t *)&_sbss;
    for(uint32_t counter = 0; counter < Sec_size; counter++)
    {
        *MemDes = 0;
        *MemDes++;
    }
    /*Call sys init*/
    System_Intitialization();
    /*call main*/
  main();
}

void Default_Handler(void)
{

}