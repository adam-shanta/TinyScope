/******************************************************************************
* File Name: TinyScope.c
*
* Description: TinyScope oscilloscope C function file
*
*
*******************************************************************************
* Copyright (2020), Adam Shanta & Anujan Varma. All rights reserved.
 *******************************************************************************/
#include "TinyScope.h"

/*******************************************************************************
Dislay startup screen
*******************************************************************************/
void ShowStartupScreen(void)
{
    /* Set font size, foreground and background colors */
    GUI_SetFont(GUI_FONT_32B_1);
    GUI_SetBkColor(GUI_BLUE);
    GUI_SetColor(GUI_LIGHTMAGENTA);
    GUI_Clear();
  
    GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_DispStringAt("TinyScope", 160, 120);
    
    GUI_SetFont(GUI_FONT_24B_1);
    GUI_SetColor(GUI_LIGHTGREEN);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispStringAt("CSE 121/L Final Project", 160, 200);
}
                
/*******************************************************************************
// Draw the background for the oscilloscope screen
*******************************************************************************/

void drawBackground(int w, int h, // width and height of screen
		    int xmargin, int ymargin){ // Margin around the image

   GUI_SetBkColor(GUI_BLACK);
   GUI_SetColor(GUI_DARKCYAN);
   GUI_FillRect(0, 0, w, h);
   GUI_SetPenSize(1);
   GUI_SetColor(GUI_LIGHTGRAY);
   GUI_DrawRect(xmargin, ymargin, w-xmargin, h-ymargin);
   GUI_SetLineStyle(GUI_LS_DOT);
}

/*******************************************************************************
// Draw the X/Y grid
*******************************************************************************/
void drawGrid(int w, int h, // width and height of screen
		    int xdiv, int ydiv,// Number of x and y divisions
		    int xmargin, int ymargin){ // Margin around the image

 int xstep = (w-xmargin*2)/xdiv;
 int ystep = (h-ymargin*2)/ydiv;
 
  GUI_SetPenSize(1);
   GUI_SetColor(GUI_LIGHTGRAY);
   GUI_SetLineStyle(GUI_LS_DOT);
   for (int i=1; i<ydiv; i++){
      GUI_DrawLine(xmargin, ymargin+i*ystep, w-xmargin, ymargin+i*ystep);
    }
    for (int i=1; i<xdiv; i++){
      GUI_DrawLine(xmargin+i*xstep, ymargin, xmargin+i*xstep, h-ymargin);
    }
}

/*******************************************************************************
// Display x and y scale settings
*******************************************************************************/

void printScaleSettings(int xscale, int yscale, int xposition, int yposition) { 
 char str[20];
 
 GUI_SetBkColor(GUI_DARKCYAN); // Set background color
 GUI_SetFont(GUI_FONT_16B_1); // Set font
 GUI_SetColor(GUI_LIGHTGRAY); // Set foreground color
 if (xscale >= 1000)
    sprintf(str, "Xscale: %0d ms/div", xscale/1000);
 else
    sprintf(str, "Xscale: %0d us/div", xscale);
 GUI_DispStringAt(str, xposition, yposition);
 int yscaleVolts = yscale/1000;
 int yscalemVolts = yscale % 1000;
 if (yscale >= 1000)
   sprintf(str, "Yscale: %0d V/div", yscaleVolts);
 else
   sprintf(str, "Yscale: %0d.%0d V/div", yscaleVolts, yscalemVolts/100);
 GUI_DispStringAt(str, xposition, yposition + 20);
  
}

/*******************************************************************************
// Display frequency on screen
*******************************************************************************/

void printFrequency(int xfreq, int yfreq, int xposition, int yposition) { 
 char str[20];
 
 GUI_SetBkColor(GUI_DARKCYAN); // Set background color
 GUI_SetFont(GUI_FONT_16B_1); // Set font
 GUI_SetColor(GUI_LIGHTGRAY); // Set foreground color
 
 sprintf(str, "Ch 1 Freq: %0d Hz", xfreq);
 GUI_DispStringAt(str, xposition, yposition);
 sprintf(str, "Ch 2 Freq: %0d Hz", yfreq);
 GUI_DispStringAt(str, xposition, yposition + 20);
}

/*******************************************************************************
// Plot a single channel waveform
*******************************************************************************/

void plotWave(int channel, // Channel to be plotted: 0 or 1
              int xpos, int ypos, // X and Y coordinates
              uint32_t color){ // Color of wave
                
    GUI_SetPenSize(4);
    GUI_SetColor(color);
    if (channel == 0)
     GUI_SPLINE_Draw(ch1_spline, xpos, ypos);
    else
     GUI_SPLINE_Draw(ch2_spline, xpos, ypos);
}

/*******************************************************************************
// Print waveforms onto screen
*******************************************************************************/
void printWaveforms(uint32_t* scroll,
                    int*      frequency){
    drawGrid(XSIZE, YSIZE, XDIVISIONS, YDIVISIONS, XMARGIN-1, YMARGIN); // Redraw grid
    plotWave(0, XMARGIN, scroll[0], GUI_GREEN);
    plotWave(1, XMARGIN, scroll[1], GUI_YELLOW); // Re-draw Channel-1 waveform
    printScaleSettings(SX, SY, 200, 15); // Refresh scale settings text
    printFrequency(frequency[0], frequency[1], 10, 15); // Refresh frequency display text
}
 
/*******************************************************************************
// Low Pass Filter
*******************************************************************************/
int lpf(int beta, int in){
    
    // Run through fp low pass filter with beta of 0.5
    int lpf_temp = 1;
    in <<= beta;
    lpf_temp = (lpf_temp << beta) - lpf_temp;
    lpf_temp += in;
    lpf_temp >>= beta;
    return(lpf_temp >> beta);
}

/*******************************************************************************
// Wave Scrolling
*******************************************************************************/
int scrollWave(int scroll_in){
    return scroll_in;
    /*
    scroll[0] = 0;
    for (uint16_t j = 0; j < 16; j++){ 
        scroll_temp[0] = 0;
        for(uint16_t i = 0; i < 1024; i++){
            scroll_temp[0] += ADC_1_GetResult16(1);   
        }
        scroll_temp[0] >>= 10;
        scroll[0] += scroll_temp[0];
    }
    scroll[0] >>= 5;
    scroll[0] &= 0xFF;
    scroll[0] -= 241;
    scroll[0] *= 20;
    if(scroll[0] < 21){
        scroll[0] = 5;
    }else if(scroll[0] > 240){
        scroll[0] = 240;
    }
    */
}

/*******************************************************************************
// Erase Waveform
*******************************************************************************/
long eraseWave(uint8_t index, int scroll_hold, int* plot_x, int* plot_y, long channel){
    plotWave(index, XMARGIN, scroll_hold, GUI_DARKCYAN); // Erase current Channel-0 waveform
    GUI_SPLINE_Delete(channel);
    return(GUI_SPLINE_Create(plot_x, plot_y, XSIZE-2*XMARGIN));
}

/*******************************************************************************
// Implements X and Y scaling, a leaky integrator low pass filter, scrolling, and trigger detection
*******************************************************************************/
void processDesr(uint16_t* src, long* channel, uint8_t index, int* plot_x, int* plot_y){
    int lpf_temp = 0;
    for(uint16_t i = 0; i < MAXPOINTS; i++){
        // ----------- Y SCALING ----------------
        lpf_temp = ( ((src[(i*255)/309])*(YSIZE-2*YMARGIN)*3300)/(SY*YDIVISIONS) ) >> 11;
        // --------------------------------------
        
        //---------- LOW PASS FILTER ---------------
        plot_y[i] = lpf(INV_BETA, lpf_temp);
        // -----------------------------------------           
        
        //---------- EDGE DETECTION -----------------
        if(plot_y[i] > frequency_temp[index]){
            // negative edge
            neg_edge[index] = 1;
            if(pos_edge[index]){
                // proccess
                frequency[index] = (PX*1000000)/(SX*frequency_counter[index]);
                frequency_counter[index] = 0;
            }
            pos_edge[index] = 0;
            
            // wait for negative trigger values
            if(neg_trig[0] && trig_temp[index]){
                if(!(plot_y_0_src0[i] < trig_val[0])){
                    i = 0;
                    trig_temp[index] = 0;
                }
            }
            if(neg_trig[1] && trig_temp[index]){
                if(!(plot_y[i] < trig_val[1])){
                    i = 0;
                    trig_temp[index] = 0;
                }
            }
        }else if(plot_y[i] < frequency_temp[index]){
            // positive edge
            pos_edge[index] = 1;
            
            // fist opposite signed value
            if(neg_edge[index]){
                // capture max value and scale for both negative and positive triggers
                trig_val[index] = (plot_y[i-1]*(3300-trigger))/3300;
            }
            neg_edge[index] = 0;
            
            // wait for positive trigger values
            if(pos_trig[0] && trig_temp[index]){
                if(!(plot_y[i] > trig_val[0])){
                    i = 0;
                    trig_temp[index] = 0;
                }
            }
            if(pos_trig[1] && trig_temp[index]){
                if(!(plot_y[i] > trig_val[1])){
                    i = 0;
                    trig_temp[index] = 0;
                }
            }
        }else{
            // peak points, do nothing
        }
        // --------------------------------------
        
        frequency_temp[index] = plot_y[i];
        frequency_counter[index]++;
        
        // ----------- X SCALING ----------------
        Cy_SysLib_DelayUs((SX*XDIVISIONS)/MAXPOINTS);
        // --------------------------------------
    }
    trig_temp[index] = 1;
    
    // ----------- SCROLLING --------------
    scroll_hold[index] = scrollWave(scroll[index]);
    // --------------------------------------
    
    // ---------- ERASE CH0 WAVEFORM -------------------
    *channel = eraseWave(index, scroll_hold[index], plot_x, plot_y, *channel);
    // -------------------------------------------------
}
            
//----------------------------------------------------------------------------------------------------------------
            
/*******************************************************************************
// DMA_1 Interrupt
*******************************************************************************/
void isrDMA_1(void){
    /* Check interrupt cause to capture errors.
    *  Note that next descriptor is NULL to stop descriptor execution */
    if (!(CY_DMA_INTR_CAUSE_COMPLETION & Cy_DMA_Channel_GetStatus(DMA_1_HW, DMA_1_DW_CHANNEL)))
    {
        /* DMA error occurred while TX operations */
        dma_1_error++;
        printf("dma_1_error: %d\r\ncause: %d\r\n", dma_1_error, Cy_DMA_Channel_GetStatus(DMA_1_HW, DMA_1_DW_CHANNEL));
        printf("src0: %d\r\nSAR: %d\r\n", src0[0], (uint32_t) SAR->CHAN_RESULT[0]);
        printf("\r\n");
    }
	else{
        activeDescr1 ^= DMA_DESCR1;
		Cy_DMA_Channel_ClearInterrupt(DMA_1_HW, DMA_1_DW_CHANNEL);
	}
}

/*******************************************************************************
// DMA_2 Interrupt
*******************************************************************************/
void isrDMA_2(void){
    /* Check interrupt cause to capture errors.
    *  Note that next descriptor is NULL to stop descriptor execution */
    if (!(CY_DMA_INTR_CAUSE_COMPLETION & Cy_DMA_Channel_GetStatus(DMA_2_HW, DMA_2_DW_CHANNEL)))
    {
        /* DMA error occurred while TX operations */
        dma_2_error++;
        printf("dma_2_error: %d\r\ncause: %d\r\n", dma_2_error, Cy_DMA_Channel_GetStatus(DMA_2_HW, DMA_2_DW_CHANNEL));
        //printf("src0: %d\r\nSAR: %d\r\n", src0[0], (uint32_t) SAR->CHAN_RESULT[0]);
        printf("\r\n");
    }
	else{
        activeDescr2 ^= DMA_DESCR1;
		Cy_DMA_Channel_ClearInterrupt(DMA_2_HW, DMA_2_DW_CHANNEL);
	}
}

void InitAll(void){
    DMAInit_1();
    DMAInit_2();
    
    // Initialize UART
    uart_init_status = Cy_SCB_UART_Init(UART_HW, &UART_config, &UART_context);
    
    Cy_SysInt_Init(&UART_SCB_IRQ_cfg, &UART_Interrupt);
    Cy_SysInt_Init(&DMA_1_INT_cfg, &isrDMA_1);
    Cy_SysInt_Init(&DMA_2_INT_cfg, &isrDMA_2);
    
    __enable_irq(); /* Enable global interrupts. */
    NVIC_EnableIRQ(UART_SCB_IRQ_cfg.intrSrc);
    NVIC_EnableIRQ(DMA_1_INT_cfg.intrSrc);
    NVIC_EnableIRQ(DMA_2_INT_cfg.intrSrc);
    
    Cy_DMA_Channel_SetInterruptMask(DMA_1_HW, DMA_1_DW_CHANNEL, CY_DMA_INTR_MASK);
    Cy_DMA_Channel_SetInterruptMask(DMA_2_HW, DMA_2_DW_CHANNEL, CY_DMA_INTR_MASK);
    
    // Enable UART
    Cy_SCB_UART_Enable(UART_HW);
    
    // Print message to stdout
    Cy_SCB_UART_PutString(UART_HW, "\r\n     Testing for lab_project \r\n");
    Cy_SCB_UART_PutString(UART_HW, "--------------------------------------\r\n");
    
    // Start ADC Conversion
    ADC_1_Start();
    ADC_1_StartConvert();
    
    Cy_DMA_Channel_Enable(DMA_1_HW, DMA_1_DW_CHANNEL);
    Cy_DMA_Enable(DMA_1_HW);
    
    Cy_DMA_Channel_Enable(DMA_2_HW, DMA_2_DW_CHANNEL);
    Cy_DMA_Enable(DMA_2_HW);
    
    /* Initialize EmWin Graphics */
    GUI_Init();
}

void DMAInit_1(void){
    cy_en_dma_status_t dma_init_status;
	cy_stc_dma_channel_config_t channelConfig;	
    
    /* Initialize the DMA channel */
    channelConfig.descriptor  = &DMA_1_Descriptor_1;
    channelConfig.preemptable = DMA_1_PREEMPTABLE;
    channelConfig.priority    = DMA_1_PRIORITY;
    channelConfig.enable      = 0u;
    channelConfig.bufferable  = DMA_1_BUFFERABLE;
    
    /* Initialize descriptor 1 */
    dma_init_status = Cy_DMA_Descriptor_Init(&DMA_1_Descriptor_1, &DMA_1_Descriptor_1_config);
	if(dma_init_status!=CY_DMA_SUCCESS)
    {
        handle_error();
    }
	
    /* Initialize descriptor 2 */
    dma_init_status = Cy_DMA_Descriptor_Init(&DMA_1_Descriptor_2, &DMA_1_Descriptor_2_config);
	if(dma_init_status!=CY_DMA_SUCCESS)
    {
        handle_error();
    }

    /* Set source and destination address for descriptor 1 */
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_1_Descriptor_1, (uint32_t *) &(SAR->CHAN_RESULT[0]));
    Cy_DMA_Descriptor_SetDstAddress(&DMA_1_Descriptor_1, &src0);
    activeDescr1 = DMA_DESCR0;

    /* Set source and destination address for descriptor 2 */
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_1_Descriptor_2, (uint32_t *) &(SAR->CHAN_RESULT[0]));
    Cy_DMA_Descriptor_SetDstAddress(&DMA_1_Descriptor_2, &src1);
    
    dma_init_status = Cy_DMA_Channel_Init(DMA_1_HW, DMA_1_DW_CHANNEL, &channelConfig);
	if(dma_init_status!=CY_DMA_SUCCESS)
    {
       handle_error();
    }
}

void DMAInit_2(void){
    cy_en_dma_status_t dma_init_status;
	cy_stc_dma_channel_config_t channelConfig;	
    
    /* Initialize the DMA channel */
    channelConfig.descriptor  = &DMA_2_Descriptor_1;
    channelConfig.preemptable = DMA_2_PREEMPTABLE;
    channelConfig.priority    = DMA_2_PRIORITY;
    channelConfig.enable      = 0u;
    channelConfig.bufferable  = DMA_2_BUFFERABLE;
    
    /* Initialize descriptor 1 */
    dma_init_status = Cy_DMA_Descriptor_Init(&DMA_2_Descriptor_1, &DMA_2_Descriptor_1_config);
	if(dma_init_status!=CY_DMA_SUCCESS)
    {
        handle_error();
    }
	
    /* Initialize descriptor 2 */
    dma_init_status = Cy_DMA_Descriptor_Init(&DMA_2_Descriptor_2, &DMA_2_Descriptor_2_config);
	if(dma_init_status!=CY_DMA_SUCCESS)
    {
        handle_error();
    }

    /* Set source and destination address for descriptor 1 */
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_2_Descriptor_1, (uint32_t *) &(SAR->CHAN_RESULT[3]));
    Cy_DMA_Descriptor_SetDstAddress(&DMA_2_Descriptor_1, &src2);
    activeDescr2 = DMA_DESCR0;

    /* Set source and destination address for descriptor 2 */
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_2_Descriptor_2, (uint32_t *) &(SAR->CHAN_RESULT[3]));
    Cy_DMA_Descriptor_SetDstAddress(&DMA_2_Descriptor_2, &src3);
    
    dma_init_status = Cy_DMA_Channel_Init(DMA_2_HW, DMA_2_DW_CHANNEL, &channelConfig);
	if(dma_init_status!=CY_DMA_SUCCESS)
    {
       handle_error();
    }
}

void handle_error(void){
    printf("ERROR\r\n");
}

/* [] END OF FILE */
