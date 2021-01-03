/******************************************************************************
* File Name: TinyScope.h
*
* Description: TinyScope oscilloscope C function header
*
*
*******************************************************************************
* Copyright (2020), Adam Shanta & Anujan Varma. All rights reserved.
 *******************************************************************************/

//*************** INCLUDES *******************
#include "project.h"
#include "GUI.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "dma/cy_dma.h"
//********************************************

//*************** DEFINES ********************
#define XSIZE 320 // Width of LCD screen in pixels
#define YSIZE 240 // Height of LCF screen in pixels
#define XDIVISIONS 10 // Number of x-axis divisions for grid
#define YDIVISIONS 8 // Number of y-axis divisions for grid
#define XMARGIN 5 // Margin around screen on x-axis
#define YMARGIN 4 // Margin around screen on y-axis
#define MAXPOINTS (XSIZE-2*XMARGIN) // Maximum number of points in wave for plotting

#define PX (MAXPOINTS/XDIVISIONS)

#define INV_BETA 2
#define SCROLL_OFFSET 20
#define SCROLL_BUFF 5

#define DMA_DESCR0       (0u)
#define DMA_DESCR1       (1u)
#define BUFFER_SIZE      (256u)
//********************************************

//************** VARIABLES *******************
extern int SX;
extern int SY;
extern int trigger;
extern uint16_t pos_trig[2];
extern uint16_t neg_trig[2];

/* Arrays to hold channel waveform data for drawing */
extern GUI_HMEM ch1_spline;
extern GUI_HMEM ch2_spline;

extern cy_en_scb_uart_status_t uart_init_status;
extern uint16_t src0[BUFFER_SIZE];
extern uint16_t src1[BUFFER_SIZE];
extern uint32_t activeDescr1; /* flag to control which descriptor to use */
extern uint32_t dma_1_error;

extern uint16_t src2[BUFFER_SIZE];
extern uint16_t src3[BUFFER_SIZE];
extern uint32_t activeDescr2; /* flag to control which descriptor to use */
extern uint32_t dma_2_error;

extern int plot_x_0[MAXPOINTS];
extern int plot_y_0_src0[MAXPOINTS];
extern int plot_y_0_src1[MAXPOINTS];
extern int lpf_temp0;
extern int lpf_temp1;

extern int plot_x_1[MAXPOINTS];
extern int plot_y_1_src2[MAXPOINTS];
extern int plot_y_1_src3[MAXPOINTS];

extern uint32_t scroll[2];

extern uint32_t scroll_temp[2];
extern uint32_t scroll_hold[2];

extern int frequency_temp[2];
extern int frequency[2];
extern int frequency_counter[2];

extern uint8_t pos_edge[2];
extern uint8_t neg_edge[2];

extern int trig_val[2];
extern uint8_t trig_temp[2];
//********************************************

//************ PROTOTYPES ********************
void InitAll(void);
void DMAInit_1(void);
void DMAInit_2(void);
void isrDMA_1(void);
void isrDMA_2(void);
void handle_error(void);

void ShowStartupScreen(void);
void drawBackground(int w, int h, int xmargin, int ymargin);
void drawGrid(int w, int h, int xdiv, int ydiv, int xmargin, int ymargin);
void printScaleSettings(int xscale, int yscale, int xposition, int yposition);
void printFrequency(int xfreq, int yfreq, int xposition, int yposition);
void plotWave(int channel, int xpos, int ypos, uint32_t color);
void printWaveforms(uint32_t* scroll, int* frequency);
int lpf(int beta, int in);
int scrollWave(int scroll_in);
long eraseWave(uint8_t index, int scroll_hold, int* plot_x, int* plot_y, long channel);
void processDesr(uint16_t* src, long* channel, uint8_t index, int* plot_x, int* plot_y);
//********************************************

/* [] END OF FILE */
