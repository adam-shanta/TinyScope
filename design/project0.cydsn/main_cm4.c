/******************************************************************************
* File Name: main_cm4.c
*
* Version: 1.10
*
* Description: TinyScope oscilloscope for CSE 121/L
               using the EmWin Graphics library snd the Newhaven Display 240x320 TFT Display.
*
*
*******************************************************************************
* Copyright (2020), Adam Shanta & Anujan Varma. All rights reserved.
 *******************************************************************************/
#include "TinyScope.h"
#include "project.h"

// ------ CONTROLS -------
int SX  = 5000;
int SY = 1000;
int trigger = 1000;
// One-Hot Only, if all values 0 default is free running mode
uint16_t pos_trig[2] = {0,0};
uint16_t neg_trig[2] = {1,0};
// --------------------

/* Arrays to hold channel waveform data for drawing */
GUI_HMEM ch1_spline;
GUI_HMEM ch2_spline;

cy_en_scb_uart_status_t uart_init_status;
uint16_t src0[BUFFER_SIZE];
uint16_t src1[BUFFER_SIZE];
uint32_t activeDescr1; /* flag to control which descriptor to use */
uint32_t dma_1_error = 0;

uint16_t src2[BUFFER_SIZE];
uint16_t src3[BUFFER_SIZE];
uint32_t activeDescr2; /* flag to control which descriptor to use */
uint32_t dma_2_error = 0;

int plot_x_0[MAXPOINTS];
int plot_y_0_src0[MAXPOINTS];
int plot_y_0_src1[MAXPOINTS];
int lpf_temp0;
int lpf_temp1;

int plot_x_1[MAXPOINTS];
int plot_y_1_src2[MAXPOINTS];
int plot_y_1_src3[MAXPOINTS];

uint32_t scroll[2] = {50,120};

uint32_t scroll_temp[2] = {0,0};
uint32_t scroll_hold[2] = {0,0};

int frequency_temp[2] = {0,0};
int frequency[2] = {0,0};
int frequency_counter[2] = {0,0};

uint8_t pos_edge[2] = {0,0};
uint8_t neg_edge[2] = {0,0};

int trig_val[2] = {0,0};
uint8_t trig_temp[2] = {1,1};

/*******************************************************************************
* Function Name: int main(void)
*******************************************************************************/
int main(void)
{
    // Initialize source arrays
    for(uint16_t i=0; i<BUFFER_SIZE; i++){
        src0[i] = 0;
        src1[i] = 0;
        src2[i] = 0;
        src3[i] = 0;
    }
    
    for (int i=0; i<MAXPOINTS; i++){
        plot_y_0_src0[i] = 0;
        plot_y_0_src1[i] = 0;
        plot_y_1_src2[i] = 0;
        plot_y_1_src3[i] = 0;
    }
    
    for (int i=0; i < MAXPOINTS; i++){
        plot_x_0[i] = XMARGIN+i;
        plot_x_1[i] = XMARGIN+i;
    }
    
    lpf_temp1 = lpf_temp0;
    
    InitAll();
   
    /* Display the startup screen for 5 seconds */
    ShowStartupScreen();   
    Cy_SysLib_Delay(1000);

   /* Display background */
    drawBackground(XSIZE, YSIZE, XMARGIN-1, YMARGIN);
    /* Display grid*/
    drawGrid(XSIZE, YSIZE, XDIVISIONS, YDIVISIONS, XMARGIN-1, YMARGIN);
   /* Display scale settings */
    printScaleSettings(SX, SY, 200, 15);
   /* Display frequency */
    printFrequency(frequency[0], frequency[1], 10, 15);
    
    ch1_spline = GUI_SPLINE_Create(plot_x_0, plot_y_0_src0, XSIZE-2*XMARGIN);
    ch2_spline = GUI_SPLINE_Create(plot_x_1, plot_y_1_src2, XSIZE-2*XMARGIN);
    
    for(;;){
        
        printWaveforms(scroll, frequency);
        
        // *********** PROCESS DMA_1 *******************
        if(activeDescr1){                                    // Case 0: DMA_1 Descr_2 Active
            processDesr(src0, &ch1_spline, 0, plot_x_0, plot_y_0_src0);
        }else {                                              // Case 1: DMA_1 Descr_1 Active
            processDesr(src1, &ch1_spline, 0, plot_x_0, plot_y_0_src1);
        }
        // *********************************************
        
        printWaveforms(scroll, frequency);
        
        // *********** PROCESS DMA_2 *******************
        if(activeDescr2){                                   // Case 0: DMA_2 Descr_2 Active
            processDesr(src2, &ch2_spline, 1, plot_x_1, plot_y_1_src2);
        }else{                                              // Case 1: DMA_2 Descr_1 Active
            processDesr(src3, &ch2_spline, 1, plot_x_1, plot_y_1_src3);
        }
        // *********************************************
    }
}

/* [] END OF FILE */