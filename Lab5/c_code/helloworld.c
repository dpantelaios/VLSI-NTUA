#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xparameters_ps.h"
#include "xaxidma.h"
#include "xtime_l.h"

//#define DMA_DEV_ID        XPAR_AXIDMA_0_DEVICE_ID
#define N 32
#define NUMBER_OF_TRANSFERS 10
#define MAX_PKT_LEN 1024

#define TX_DMA_ID                 XPAR_PS2PL_DMA_DEVICE_ID
#define TX_DMA_MM2S_LENGTH_ADDR  (XPAR_PS2PL_DMA_BASEADDR + 0x28) // Reports actual number of bytes transferred from PS->PL (use Xil_In32 for report)

#define RX_DMA_ID                 XPAR_PL2PS_DMA_DEVICE_ID
#define RX_DMA_S2MM_LENGTH_ADDR  (XPAR_PL2PS_DMA_BASEADDR + 0x58) // Reports actual number of bytes transferred from PL->PS (use Xil_In32 for report)

#define TX_BUFFER (XPAR_DDR_MEM_BASEADDR + 0x08000000) // 0 + 128MByte
#define RX_BUFFER (XPAR_DDR_MEM_BASEADDR + 0x10000000) // 0 + 256MByte
#define RED_BUFFER (XPAR_DDR_MEM_BASEADDR + 0x18000000) //0 + 384Mbyte
#define GREEN_BUFFER (XPAR_DDR_MEM_BASEADDR + 0x18200000) //0 + 386Mbyte
#define BLUE_BUFFER (XPAR_DDR_MEM_BASEADDR + 0x18400000) //0 + 388Mbyte

/* User application global variables & defines */
static int CheckData(void);

XAxiDma AxiDma_RX, AxiDma_TX;
//u8 GREEN_BUFFERPtr[MAX_PKT_LEN], BLUE_BUFFERPtr[MAX_PKT_LEN], RED_BUFFERPtr[MAX_PKT_LEN];

u8 *RED_BUFFERPtr, *GREEN_BUFFERPtr, *BLUE_BUFFERPtr;

int main()
{
    Xil_DCacheDisable();

    XTime preExecCyclesFPGA = 0;
    XTime postExecCyclesFPGA = 0;
    XTime preExecCyclesSW = 0;
    XTime postExecCyclesSW = 0;

    printf("HELLO 1\r\n");
    XAxiDma_Config *CfgPtr_RX;
    XAxiDma_Config *CfgPtr_TX;
    int Status, i, j;
    int Tries = NUMBER_OF_TRANSFERS;

    int Index, line_odd_even, pixel_odd_even;
    //u8 Green[N*N], Blue[N*N], Red[N*N];
    u8 UpRight, UpMiddle, UpLeft, Left, Right, DownRight, DownMiddle, DownLeft;
    u8 *TxBufferPtr;
    u8 *RxBufferPtr;


    TxBufferPtr = (u8 *)TX_BUFFER;
    RxBufferPtr = (u8 *)RX_BUFFER;

    RED_BUFFERPtr = (u8 *)RED_BUFFER;
    GREEN_BUFFERPtr = (u8 *)GREEN_BUFFER;
    BLUE_BUFFERPtr = (u8 *)BLUE_BUFFER;

    xil_printf("HELLO 2\r\n");
    init_platform();
    xil_printf("HELLO 2.5\r\n");

    // Step 1: Initialize TX-DMA Device (PS->PL)
    // Step 2: Initialize RX-DMA Device (PL->PS)

    CfgPtr_TX = XAxiDma_LookupConfig(TX_DMA_ID);
    if (!CfgPtr_TX) {
        xil_printf("No config found for %d\r\n", TX_DMA_ID);
        return XST_FAILURE;
    }
    xil_printf("HELLO 3\r\n");
    Status = XAxiDma_CfgInitialize(&AxiDma_TX, CfgPtr_TX);
    if (Status != XST_SUCCESS) {
        xil_printf("Initialization failed %d\r\n", Status);
        return XST_FAILURE;
    }
    xil_printf("HELLO 4\r\n");
    CfgPtr_RX = XAxiDma_LookupConfig(RX_DMA_ID);
    if (!CfgPtr_RX) {
        xil_printf("No config found for %d\r\n", RX_DMA_ID);
        return XST_FAILURE;
    }
    xil_printf("HELLO 5\r\n");
    Status = XAxiDma_CfgInitialize(&AxiDma_RX, CfgPtr_RX);
    if (Status != XST_SUCCESS) {
        xil_printf("Initialization failed %d\r\n", Status);
        return XST_FAILURE;
    }


    ////CHECK XAXIDMA_DEVICE_TO_DMA AND XAXIDMA_DMA_TO_DEVICE
    XAxiDma_IntrDisable(&AxiDma_TX, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&AxiDma_TX, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DMA_TO_DEVICE);

    XAxiDma_IntrDisable(&AxiDma_RX, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&AxiDma_RX, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DMA_TO_DEVICE);


    for(Index = 0; Index < N*N; Index ++) {
            TxBufferPtr[Index] = Index % 256;
            RxBufferPtr[Index] = 0x00;
    }



    XTime_GetTime(&preExecCyclesFPGA);
    // Step 3 : Perform FPGA processing
    //      3a: Setup RX-DMA transaction
    //      3b: Setup TX-DMA transaction
    //      3c: Wait for TX-DMA & RX-DMA to finish

    for(Index = 0; Index < Tries; Index ++) {

    	xil_printf("HELLO 5.5\r\n");

		Status = XAxiDma_SimpleTransfer(&AxiDma_RX,(UINTPTR) RxBufferPtr,
				4*MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);

		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		xil_printf("HELLO 5.6\r\n");

		Status = XAxiDma_SimpleTransfer(&AxiDma_TX,(UINTPTR) TxBufferPtr,
					MAX_PKT_LEN, XAXIDMA_DMA_TO_DEVICE);
		if (Status != XST_SUCCESS) {
				return XST_FAILURE;
		}
    }

//    Status = XAxiDma_SimpleTransfer(&AxiDma_TX,(UINTPTR) TxBufferPtr,
//                    MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);
//
//    if (Status != XST_SUCCESS) {
//            return XST_FAILURE;
//        }
//
//    xil_printf("HELLO 5.6\r\n");
//
//    Status = XAxiDma_SimpleTransfer(&AxiDma_RX,(UINTPTR) RxBufferPtr,
//                        MAX_PKT_LEN, XAXIDMA_DMA_TO_DEVICE);
//
//    if (Status != XST_SUCCESS) {
//        return XST_FAILURE;
//    }

    xil_printf("HELLO 5.7\r\n");



    while (XAxiDma_Busy(&AxiDma_TX,XAXIDMA_DMA_TO_DEVICE)) {}

    xil_printf("HELLO 5.8\r\n");

    while ((XAxiDma_Busy(&AxiDma_RX,XAXIDMA_DEVICE_TO_DMA))) {}


    //}

    XTime_GetTime(&postExecCyclesFPGA);

    XTime_GetTime(&preExecCyclesSW);

    xil_printf("HELLO 6\r\n");

    line_odd_even = 0;
    pixel_odd_even = 0;
    for(i=0; i < N; i++){
        for(j=0; j < N; j++){
            if(i==0 && j==0){
                UpRight = 0;
                UpMiddle = 0;
                UpLeft = 0;
                Left = 0;
                DownLeft = 0;
				Right = TxBufferPtr[i*N+j+1];
            	DownRight = TxBufferPtr[(i+1)*N+j+1];
            	DownMiddle = TxBufferPtr[(i+1)*N+j];
            	//printf("%x %x", Left, Right);
            }
            else if((i==0) && (j==N-1)){
				UpRight = 0;
				UpMiddle = 0;
				UpLeft = 0;
				Left = TxBufferPtr[i*N+j-1];;
				DownLeft = TxBufferPtr[(i+1)*N+j-1];
				Right = 0;
				DownRight = 0;
				DownMiddle = TxBufferPtr[(i+1)*N+j];
			}
            else if((i==N-1) && (j==0)){
				UpRight = TxBufferPtr[(i-1)*N+j+1];
				UpMiddle = TxBufferPtr[(i-1)*N+j];
				UpLeft = 0;
				Left = 0;
				DownLeft = 0;
				Right = TxBufferPtr[i*N+j+1];
				DownRight = 0;
				DownMiddle = 0;
			}
            else if(i==0){
                UpRight = 0;
                UpMiddle = 0;
                UpLeft = 0;
                Left = TxBufferPtr[i*N+j-1];
                DownLeft = TxBufferPtr[(i+1)*N+j-1];
				Right = TxBufferPtr[i*N+j+1];
            	DownRight = TxBufferPtr[(i+1)*N+j+1];
           		DownMiddle = TxBufferPtr[(i+1)*N+j];
            }
            else if(j==0){
                UpRight = TxBufferPtr[(i-1)*N+j+1];
                UpMiddle = TxBufferPtr[(i-1)*N+j];
                UpLeft = 0;
                Left = 0;
                DownLeft = 0;
				Right = TxBufferPtr[i*N+j+1];
            	DownRight = TxBufferPtr[(i+1)*N+j+1];
            	DownMiddle = TxBufferPtr[(i+1)*N+j];
            }
			else if ((i==N-1) && (j=N-1)) {
				UpRight = 0;
                UpMiddle = TxBufferPtr[(i-1)*N+j];
                UpLeft = TxBufferPtr[(i-1)*N+j-1];
                Left = TxBufferPtr[i*N+j-1];
                DownLeft = 0;
				Right = 0;
            	DownRight = 0;
            	DownMiddle = 0;
			}
			else if (i==N-1) {
				UpRight = TxBufferPtr[(i-1)*N+j+1];
                UpMiddle = TxBufferPtr[(i-1)*N+j];
                UpLeft = TxBufferPtr[(i-1)*N+j-1];
                Left = TxBufferPtr[i*N+j-1];
                DownLeft = 0;
				Right = TxBufferPtr[i*N+j+1];
            	DownRight = 0;
            	DownMiddle = 0;
			}
			else if (j==N-1) {
				UpRight = 0;
                UpMiddle = TxBufferPtr[(i-1)*N+j];
                UpLeft = TxBufferPtr[(i-1)*N+j-1];
                Left = TxBufferPtr[i*N+j-1];
                DownLeft = TxBufferPtr[(i+1)*N+j-1];
				Right = 0;
            	DownRight = 0;
            	DownMiddle = TxBufferPtr[(i+1)*N+j];
			}
            else{
                UpRight = TxBufferPtr[(i-1)*N+j+1];
                UpMiddle = TxBufferPtr[(i-1)*N+j];
                UpLeft = TxBufferPtr[(i-1)*N+j-1];
                Left = TxBufferPtr[i*N+j-1];
                DownLeft = TxBufferPtr[(i+1)*N+j-1];
				Right = TxBufferPtr[i*N+j+1];
            	DownRight = TxBufferPtr[(i+1)*N+j+1];
            	DownMiddle = TxBufferPtr[(i+1)*N+j];
            }
            //Right = TxBufferPtr[i*N+j+1];
            //DownRight = TxBufferPtr[(i+1)*N+j+1];
            //DownMiddle = TxBufferPtr[(i+1)*N+j];

            if(line_odd_even == 0){ //grammes 0,2,...(green and blue)
                if(pixel_odd_even == 0){ //green
                    GREEN_BUFFERPtr[i*N+j] = TxBufferPtr[i*N+j];
                    BLUE_BUFFERPtr[i*N+j] = (Left + Right)/2;
                    //printf("%x\n", (Left + Right)/2);
                    RED_BUFFERPtr[i*N+j] = (UpMiddle + DownMiddle)/2;
                }
                else{   //blue
                    GREEN_BUFFERPtr[i*N+j] = (Left + Right + UpMiddle + DownMiddle)/4;
                    BLUE_BUFFERPtr[i*N+j] = TxBufferPtr[i*N+j];
                    RED_BUFFERPtr[i*N+j] = (UpLeft + UpRight + DownLeft + DownRight)/4;
                }
            }
            else{   //grammes 1,3,...(green and red)
                if(pixel_odd_even == 0){ //red
                    GREEN_BUFFERPtr[i*N+j] = (Left + Right + UpMiddle + DownMiddle)/4;
                    BLUE_BUFFERPtr[i*N+j] = (UpLeft + UpRight + DownLeft + DownRight)/4;
                    RED_BUFFERPtr[i*N+j] = TxBufferPtr[i*N+j];
                }
                else{ //green
                    GREEN_BUFFERPtr[i*N+j] = TxBufferPtr[i*N+j];
                    BLUE_BUFFERPtr[i*N+j] = (UpMiddle + DownMiddle)/2;
                    RED_BUFFERPtr[i*N+j] = (Left + Right)/2;
                }
            }
            pixel_odd_even = (pixel_odd_even + 1)%2;
        }
        line_odd_even = (line_odd_even + 1)%2;
    }


	/*for (i=0; i<N; ++i) {
		for (j=0; j<N; j++) {
			printf("%d ", RED_BUFFERPtr[i*N+j]);
			printf("%d ", GREEN_BUFFERPtr[i*N+j]);
			printf("%d\n", BLUE_BUFFERPtr[i*N+j]);
		}
	}*/

    /*
    for (i=0; i<N; ++i) {
    	for (j=0; j<N; j++) {
    		xil_printf("%d", i*N+j);
    	}
    } */
    XTime_GetTime(&postExecCyclesSW);

    // Step 6: Compare FPGA and SW results
    //     6a: Report total percentage error

    Index=0;
    Xil_DCacheInvalidateRange((UINTPTR)RxBufferPtr, MAX_PKT_LEN);
	for(Index = 0; Index < 4*MAX_PKT_LEN; Index++) {
		xil_printf("%d: %x\r\n",
					Index, (unsigned int)RxBufferPtr[Index]);
	}


    for(Index = 0; Index < MAX_PKT_LEN; Index++) {
	 	 xil_printf("%d: %x %x %x\r\n",
	 				Index, RED_BUFFERPtr[Index], GREEN_BUFFERPtr[Index], BLUE_BUFFERPtr[Index]);
	 }
//    Status = CheckData();
//    if (Status != XST_SUCCESS) {
//        return XST_FAILURE;
//    }

    //     6b: Report FPGA execution time in cycles (use preExecCyclesFPGA and postExecCyclesFPGA)
    //     6c: Report SW execution time in cycles (use preExecCyclesSW and postExecCyclesSW)
    //     6d: Report speedup (SW_execution_time / FPGA_exection_time)

    cleanup_platform();
    return 0;
}

static int CheckData(void)
{
    u8 *RxPacket;
    int Index = 0;
    u8 Value_Red, Value_Green, Value_Blue;

    RxPacket = (u8 *) RX_BUFFER;
    //Value = TEST_START_VALUE;

    /* Invalidate the DestBuffer before receiving the data, in case the
     * Data Cache is enabled
     */
    Xil_DCacheInvalidateRange((UINTPTR)RxPacket, MAX_PKT_LEN);
    for(Index = 0; Index < MAX_PKT_LEN; Index++) {
    	xil_printf("Data error %d: %x\r\n",
    	            Index, (unsigned int)RxPacket[Index]);
    }
//    for(Index = 0; Index < MAX_PKT_LEN; Index++) {
//        Value_Blue = RxPacket[Index] & 0xFF;
//        Value_Green = (RxPacket[Index] >> 8) & 0xFF;
//        Value_Red = (RxPacket[Index] >> 16) & 0xFF;
//        if ((BLUE_BUFFERPtr[Index] != Value_Blue) || (GREEN_BUFFERPtr[Index] != Value_Green) || (RED_BUFFERPtr[Index] != Value_Red)) {
//            xil_printf("Data error %d: %u/%u\r\n",
//            Index, (unsigned int)RxPacket[Index],
//                (unsigned int)Value_Blue);
//
//            //return XST_FAILURE;
//        }
//    }

    return XST_SUCCESS;
}
