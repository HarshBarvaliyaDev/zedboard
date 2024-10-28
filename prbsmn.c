/*
 * prbsmn.c: PRBS test
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 */

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xscutimer.h"


#define PRBSREG(k)  *(volatile unsigned int *)(XPAR_PRBS8BIT_0_S00_AXI_BASEADDR+4*k)


// ---- interrupt controller -----
static XScuGic  Intc;					// interrupt controller instance
static XScuGic_Config  *IntcConfig;		// configuration instance

// ---- scu timer -----
static XScuTimer  pTimer;				// private Timer instance
static XScuTimer_Config  *pTimerConfig;	// configuration instance
#define TIMER_LOAD_VALUE  166666500		// should be 2 Hz

static volatile int  ISR_LEDs, ISR_Count;
static volatile int  Do_Display;

#define CMD_LEN 64

/*
 * ------------------------------------------------------------
 * Interrupt handler (ZYNQ private timer)
 * ------------------------------------------------------------
 */
static void TimerIntrHandler(void *CallBackRef)
{
	XScuTimer *TimerInstance = (XScuTimer *)CallBackRef;

	XScuTimer_ClearInterruptStatus(TimerInstance);
	ISR_Count++;
	Do_Display = 1;
	if (ISR_LEDs != 0) {
		PRBSREG(0) = ISR_LEDs;
		if (ISR_LEDs == 1) {
			ISR_LEDs = 0x08;
		} else {
			ISR_LEDs >>= 1;
		}
	}
}


int main()
{
	unsigned int xx, pval;
	char cmd_buf[CMD_LEN], *chp;
	int terminate, stlen, isr_run, pcnt;

    init_platform();
    print("--- PRBS IVP V0.a ---\n\r");
    PRBSREG(0) = 0x081;

    ISR_LEDs = 1;  ISR_Count = 0; 	Do_Display = 0;
    print(" * initialize exceptions...\n\r");
    Xil_ExceptionInit();

    print(" * lookup config GIC...\n\r");
    IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
    print(" * initialize GIC...\n\r");
    XScuGic_CfgInitialize(&Intc, IntcConfig, IntcConfig->CpuBaseAddress);

	// Connect the interrupt controller interrupt handler to the hardware
    print(" * connect interrupt controller handler...\n\r");
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler, &Intc);

    print(" * lookup config scu timer...\n\r");
    pTimerConfig = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_DEVICE_ID);
    print(" * initialize scu timer...\n\r");
    XScuTimer_CfgInitialize(&pTimer, pTimerConfig, pTimerConfig->BaseAddr);
    print(" * Enable Auto reload mode...\n\r");
	XScuTimer_EnableAutoReload(&pTimer);
    print(" * load scu timer...\n\r");
    XScuTimer_LoadTimer(&pTimer, TIMER_LOAD_VALUE);

    print(" * set up timer interrupt...\n\r");
    XScuGic_Connect(&Intc, XPAR_SCUTIMER_INTR, (Xil_ExceptionHandler)TimerIntrHandler,
    				(void *)&pTimer);
    print(" * enable interrupt for timer at GIC...\n\r");
    XScuGic_Enable(&Intc, XPAR_SCUTIMER_INTR);
    print(" * enable interrupt on timer...\n\r");
    XScuTimer_EnableInterrupt(&pTimer);

	// Enable interrupts in the Processor.
    print(" * enable processor interrupts...\n\r");
	Xil_ExceptionEnable();
	Do_Display = 0;
    isr_run = 0;

    terminate = 0;
    do {
    	print(">> ");
    	fgets(cmd_buf, CMD_LEN, stdin);
    	cmd_buf[CMD_LEN-1] = '\0';
    	chp = cmd_buf;
    	while ((*chp!='\0') && (*chp!='\n') && (*chp!='\r'))  chp++;
    	*chp = '\0';
    	if (cmd_buf[0] == 'x') {
    		terminate = 1;
    	} else if (!strcmp(cmd_buf, "pbsw")) {
    	    printf("o Do_Display: %d  |  ISR_Count: %d\n\r", Do_Display, ISR_Count);
    	    xx = PRBSREG(0);
    	    printf(" o bt|sw: %x\n\r", xx);
    	} else if (!strncmp(cmd_buf, "init", 4)) {
    		if ((stlen = strlen(cmd_buf)) < 5) {
    			printf("*** illegal cmd length (%d).\n\r", stlen);
    		} else {
    			xx = 99;
    			if (sscanf(&cmd_buf[4], "%x", &xx) != 1) {
        			printf("*** illegal value.\n\r");
    			} else if (xx > 0x0ff) {
        			printf("*** illegal prbs load value (%x).\n\r", xx);
    			} else {
    				PRBSREG(1) = xx;
    			}
    		}
    	} else if (!strncmp(cmd_buf, "test", 4)) {
    		if (isr_run == 0) {
    			printf(" *** Interrupts must be enabled.\n\r");
    		} else if ((stlen = strlen(cmd_buf)) < 5) {
    			printf("*** illegal cmd length (%d).\n\r", stlen);
    		} else {
    			xx = 99;
    			if (sscanf(&cmd_buf[4], "%d", &xx) != 1) {
        			printf("*** illegal value.\n\r");
    			} else if (xx > 999) {
        			printf("*** illegal prbs load value (%x).\n\r", xx);
    			} else {
    				ISR_LEDs = 0;


    				// Test the PRBS generator

    			PRBSREG(1) = 0x0ff;
    			Do_Display= 0;
    			for (pcnt = 0 ; pcnt < xx ; pcnt++){
    				PRBSREG(2) = 254;
    				do{

    				}while(Do_Display == 0);
    				Do_Display= 0;


    				pval = PRBSREG(1);
    				PRBSREG(0) = pval;
    				printf("%4d. %4d [ %x ]\n\r" , pcnt , pval, pval);
    			}

    	    		ISR_LEDs = 1;
    			}
    		}
    	} else if (cmd_buf[0] == 's') {
    		PRBSREG(2) = 0;
    		xx = PRBSREG(1);
    		printf(" o PRBS: %2x  (%3d)\n\r", xx, xx);
    	} else if (!strcmp(cmd_buf, "isr")) {
    		if (isr_run == 0) {
    		    // start scu timer
    		    print(" * start timer...\n\r");
    		    XScuTimer_Start(&pTimer);
    		    isr_run = 1;
    		} else {
    		    // stop scu timer
    		    print(" * stop timer...\n\r");
    		    XScuTimer_Stop(&pTimer);
    			isr_run = 0;
    		}
    		xil_printf("ISR count: %d\n\r", ISR_Count);
    	} else {
    	    printf(" *** unknown command: [%s]\n\r", cmd_buf);
    	}
    } while (terminate == 0);

    print("shutting down...\n\r");
    XScuTimer_Stop(&pTimer);
	Xil_ExceptionDisable();
    XScuTimer_DisableInterrupt(&pTimer);
    XScuGic_Disable(&Intc, XPAR_SCUTIMER_INTR);
    // all transistors off;
    PRBSREG(0) = 0;
    print("Thank you for using PRBS IVP V0.a\n\r");
    cleanup_platform();
    return 0;
}
