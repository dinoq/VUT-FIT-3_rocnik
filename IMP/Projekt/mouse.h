/*
 * Autor: Petr Marek(xmarek69)
 *
 * Zmìny v souboru:
 * Pridany konstanty
 *
 * cca 60% souboru tvori moje vlastni kody, zbyvajicich 40% je prevzato (a z casti upraveno) z prikladu USB DEVICE z MQX RTOS
 *
 * Datum posledni zmeny:
 * 22.12. 2019
 */

#ifndef _MOUSE_H
#define _MOUSE_H

#include "usb_descriptor.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

//Definice tlacitek
#if defined BSP_BUTTON1
	#define BUTTON_LEFT BSP_BUTTON1
#endif
#if defined BSP_BUTTON2
	#define BUTTON_DOWN BSP_BUTTON2
#endif
#if defined BSP_BUTTON3
	#define BUTTON_RIGHT BSP_BUTTON3
#endif
#if defined BSP_BUTTON4
	#define BUTTON_UP BSP_BUTTON4
#endif
#if defined BSP_BUTTON5
	#define BUTTON_CHANGE BSP_BUTTON1
#endif

//konstanty pro rozliseni zmacknuti/pusteni tlacitka
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

//Indexy do pole tlacitek
#define BTN_LEFT 0
#define BTN_DOWN 1
#define BTN_RIGHT 2
#define BTN_UP 3

#define DOUBLE_CLICK_TIME 100000

//indexy do pole rychlosti blikani tlacitka
#define SUPER_FAST 0
#define FAST 1
#define SLOW 2


/******************************************************************************
 * Macro's
 *****************************************************************************/
#define  MOUSE_BUFF_SIZE   (4)   /* report buffer size */
#define  REQ_DATA_SIZE     (1)

#define COMPLIANCE_TESTING    (0)/*1:TRUE, 0:FALSE*/
/******************************************************************************
 * Types
 *****************************************************************************/
typedef struct _mouse_variable_struct
{
    HID_HANDLE app_handle;
    bool mouse_init;/* flag to check lower layer status*/
    uint8_t rpt_buf[MOUSE_BUFF_SIZE];/*report/data buff for mouse application*/
    USB_CLASS_HID_ENDPOINT ep[HID_DESC_ENDPOINT_COUNT];
    uint8_t app_request_params[2]; /* for get/set idle and protocol requests*/
}MOUSE_GLOBAL_VARIABLE_STRUCT, * PTR_MOUSE_GLOBAL_VARIABLE_STRUCT; 

/*****************************************************************************
 * Global variables
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);

extern void move_mouse(void);

#endif 

/* EOF */
