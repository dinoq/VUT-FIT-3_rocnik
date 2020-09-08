/*
 * Autor: Petr Marek(xmarek69)
 *
 * Zmìny v souboru:
 * Pridany promenne
 * Upravene funkce:
 * 		move_mouse()
 * 		TestApp_Init()
 * Pridane funkce:
 * 		checkButtons() - kontrola ktere tlacitka byla stisknuta a provedeni prislusnych akci (u "pohybovych" tlacitek nastaveni dx a dy pro pohyb mysi, u tlacitka zmeny rychlosti blikani rychlost)
 * 		initButtons() - Inicializace tlacitek
 * 		initLeds() - Inicializace LED diod
 *
 * cca 70% souboru tvori moje vlastni kody, zbyvajicich 30% je prevzato (a z casti upraveno) z prikladu USB DEVICE z MQX RTOS
 *
 * Datum posledni zmeny:
 * 22.12. 2019
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "mouse.h"
#include <usb_hid.h>

#include "mqx.h"
#include "bsp.h"

extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L , 0, 0}
};

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for mouse.c to this structure */
extern USB_ENDPOINTS usb_desc_ep;
extern DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback;
MOUSE_GLOBAL_VARIABLE_STRUCT g_mouse;

//dx a dy slouzi pro nastaveni o kolik pixelu se ma pohybovat ukazatel mysi
int dx;
int dy;
int blink_speed;
int speed_index;

long released_buttons[4];
long pressed_buttons[4];
int speeds[3];

int previous_button_state;

//Deklarace tlacitek
#if defined BUTTON_LEFT
	LWGPIO_STRUCT btn_left;
#endif
#if defined BUTTON_DOWN
	LWGPIO_STRUCT btn_down;
#endif
#if defined BUTTON_RIGHT
	LWGPIO_STRUCT btn_right;
#endif
#if defined BUTTON_UP
	LWGPIO_STRUCT btn_up;
#endif
#if defined BUTTON_CHANGE
	LWGPIO_STRUCT btn_change;
#endif

//Deklarace LED diod
#ifdef BSP_LED1
	LWGPIO_STRUCT led_left;
#endif
#ifdef BSP_LED2
	LWGPIO_STRUCT led_down;
#endif
#ifdef BSP_LED3
	LWGPIO_STRUCT led_right;
#endif
#ifdef BSP_LED4
	LWGPIO_STRUCT led_up;
#endif

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg); 
uint8_t USB_App_Param_Callback(uint8_t request, uint16_t value, uint8_t **data, 
    uint32_t* size,void   *arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*****************************************************************************
* 
*      @name     move_mouse
*
*      @brief    This function gets makes the cursor on screen move left,right
*                up and down
*
*      @param    None      
*
*      @return   None
*     
* 
******************************************************************************/
void move_mouse(void) 
{
    g_mouse.rpt_buf[1] = (uint8_t)dx;
    g_mouse.rpt_buf[2] = (uint8_t)dy;

    (void)USB_Class_HID_Send_Data(g_mouse.app_handle,HID_ENDPOINT,
        g_mouse.rpt_buf,MOUSE_BUFF_SIZE);
}
/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg) 
{    
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)

    switch(event_type)
    {
        case USB_APP_BUS_RESET:     
            g_mouse.mouse_init=FALSE;
            break;
        case USB_APP_ENUM_COMPLETE:    
            g_mouse.mouse_init = TRUE;
            move_mouse();/* run the coursor movement code */
            break;
        case USB_APP_SEND_COMPLETE:    
             /*check whether enumeration is complete or not */
            if(g_mouse.mouse_init)
            {
                #if COMPLIANCE_TESTING             
                    uint32_t g_compliance_delay = 0x009FFFFF;
                    while(g_compliance_delay--);
                #endif
                move_mouse();/* run the coursor movement code */                  
            }
            break;
        case USB_APP_ERROR: 
            /* user may add code here for error handling 
               NOTE : val has the value of error from h/w*/
            break;
        default: 
            break;          
    }   
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Param_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
 uint8_t USB_App_Param_Callback
 (
    uint8_t request, 
    uint16_t value, 
    uint8_t **data, 
    uint32_t* size,
    void   *arg
) 
{
    uint8_t error = USB_OK;
    //uint8_t direction =  (uint8_t)((request & USB_HID_REQUEST_DIR_MASK) >>3);
    uint8_t index = (uint8_t)((request - 2) & USB_HID_REQUEST_TYPE_MASK); 
    
    UNUSED_ARGUMENT(arg)
    /* index == 0 for get/set idle, index == 1 for get/set protocol */        
    *size = 0;
    /* handle the class request */
    switch(request) 
    {
        case USB_HID_GET_REPORT_REQUEST :       
            *data = &g_mouse.rpt_buf[0]; /* point to the report to send */                    
            *size = MOUSE_BUFF_SIZE; /* report size */          
            break;                                        
              
        case USB_HID_SET_REPORT_REQUEST :
            for(index = 0; index < MOUSE_BUFF_SIZE ; index++) 
            {   /* copy the report sent by the host */          
                g_mouse.rpt_buf[index] = *(*data + index);
            }        
            break;
                
        case USB_HID_GET_IDLE_REQUEST :
            /* point to the current idle rate */
            *data = &g_mouse.app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;
                
        case USB_HID_SET_IDLE_REQUEST :
            /* set the idle rate sent by the host */
            g_mouse.app_request_params[index] =(uint8_t)((value & MSB_MASK) >> 
                HIGH_BYTE_SHIFT);
            break;
           
        case USB_HID_GET_PROTOCOL_REQUEST :
            /* point to the current protocol code 
               0 = Boot Protocol
               1 = Report Protocol*/
            *data = &g_mouse.app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;
                
        case USB_HID_SET_PROTOCOL_REQUEST :
            /* set the protocol sent by the host 
               0 = Boot Protocol
               1 = Report Protocol*/
               g_mouse.app_request_params[index] = (uint8_t)(value);  
               break;
    }           
    return error; 
}  
 
void checkButtons(){

	int btn_state = BUTTON_RELEASED;
	int value_pressed;

	#if defined(BSP_BUTTONS_ACTIVE_HIGH)
			value_pressed = LWGPIO_VALUE_HIGH;
	#else
			value_pressed = LWGPIO_VALUE_LOW;
	#endif

	#if defined BUTTON_LEFT
				if (lwgpio_get_value(&btn_left) == value_pressed){
					btn_state = BUTTON_PRESSED;
				}
				else{
					btn_state = BUTTON_RELEASED;
				}

				if(btn_state == BUTTON_PRESSED){
					if(released_buttons[BTN_LEFT]<DOUBLE_CLICK_TIME && released_buttons[BTN_LEFT]>0){
						dx = 5;
						pressed_buttons[BTN_LEFT]++;
						if((pressed_buttons[BTN_LEFT] / blink_speed) % 2 == 0){
							lwgpio_set_value(&led_left, LWGPIO_VALUE_LOW);
						}else{
							lwgpio_set_value(&led_left, LWGPIO_VALUE_HIGH);
						}
					}else{
						lwgpio_set_value(&led_left, LWGPIO_VALUE_LOW);
						if(dx == 0){
							dx = 2;
						}else{
							dx = 0;
						}
						released_buttons[BTN_LEFT] = 0;
					}
				}else{
					lwgpio_set_value(&led_left, LWGPIO_VALUE_HIGH);
					if(dx > 0){
						dx = 0;
					}
					released_buttons[BTN_LEFT]++;
					pressed_buttons[BTN_LEFT] = 0;
				}
	#endif
	#if defined BUTTON_DOWN
				if (lwgpio_get_value(&btn_down) == value_pressed){
					btn_state = BUTTON_PRESSED;
				}
				else{
					btn_state = BUTTON_RELEASED;
				}

				if(btn_state == BUTTON_PRESSED){
					if(released_buttons[BTN_DOWN]<DOUBLE_CLICK_TIME && released_buttons[BTN_DOWN]>0){
						dy = 5;
						pressed_buttons[BTN_DOWN]++;
						if((pressed_buttons[BTN_DOWN] / blink_speed) % 2 == 0){
							lwgpio_set_value(&led_down, LWGPIO_VALUE_LOW);
						}else{
							lwgpio_set_value(&led_down, LWGPIO_VALUE_HIGH);
						}
					}else{
						lwgpio_set_value(&led_down, LWGPIO_VALUE_LOW);
						if(dy == 0){
							dy = 2;
						}else{
							dy = 0;
						}
						released_buttons[BTN_DOWN] = 0;
					}
				}else{
					lwgpio_set_value(&led_down, LWGPIO_VALUE_HIGH);
					if(dy > 0){
						dy = 0;
					}
					released_buttons[BTN_DOWN]++;
				}
	#endif
	#if defined BUTTON_RIGHT
				if (lwgpio_get_value(&btn_right) == value_pressed){
					btn_state = BUTTON_PRESSED;
				}
				else{
					btn_state = BUTTON_RELEASED;
				}

				if(btn_state == BUTTON_PRESSED){
					if(released_buttons[BTN_RIGHT]<DOUBLE_CLICK_TIME && released_buttons[BTN_RIGHT]>0){
						dx = -5;
						pressed_buttons[BTN_RIGHT]++;
						if((pressed_buttons[BTN_RIGHT] / blink_speed) % 2 == 0){
							lwgpio_set_value(&led_right, LWGPIO_VALUE_LOW);
						}else{
							lwgpio_set_value(&led_right, LWGPIO_VALUE_HIGH);
						}
					}else{
						lwgpio_set_value(&led_right, LWGPIO_VALUE_LOW);
						if(dx == 0){
							dx = -2;
						}else{
							dx = 0;
						}
						released_buttons[BTN_RIGHT] = 0;
					}
				}else{
					lwgpio_set_value(&led_right, LWGPIO_VALUE_HIGH);
					if(dx < 0){
						dx = 0;
					}
					released_buttons[BTN_RIGHT]++;
				}
	#endif
	#if defined BUTTON_UP
				if (lwgpio_get_value(&btn_up) == value_pressed){
					btn_state = BUTTON_PRESSED;
				}
				else{
					btn_state = BUTTON_RELEASED;
				}

				if(btn_state == BUTTON_PRESSED){
					if(released_buttons[BTN_UP]<DOUBLE_CLICK_TIME && released_buttons[BTN_UP]>0){
						dy = -5;
						pressed_buttons[BTN_UP]++;
						if((pressed_buttons[BTN_UP] / blink_speed) % 2 == 0){
							lwgpio_set_value(&led_up, LWGPIO_VALUE_LOW);
						}else{
							lwgpio_set_value(&led_up, LWGPIO_VALUE_HIGH);
						}
					}else{
						lwgpio_set_value(&led_up, LWGPIO_VALUE_LOW);
						if(dy == 0){
							dy = -2;
						}else{
							dy = 0;
						}
						released_buttons[BTN_UP] = 0;
					}
				}else{
					lwgpio_set_value(&led_up, LWGPIO_VALUE_HIGH);
					if(dy < 0){
						dy = 0;
					}
					released_buttons[BTN_UP]++;
				}
	#endif
	#if defined BUTTON_CHANGE
				if (lwgpio_get_value(&btn_change) == value_pressed){
					btn_state = BUTTON_PRESSED;
				}
				else{
					btn_state = BUTTON_RELEASED;
				}

				if(btn_state == BUTTON_PRESSED && previous_button_state == BUTTON_RELEASED){
					blink_speed = speeds[speed_index++];
					if(speed_index > 2){
						speed_index = 0;
					}
				}
				previous_button_state = btn_state;

	#endif
}

void initButtons(){

	released_buttons[BTN_LEFT] = 0;
	released_buttons[BTN_DOWN] = 0;
	released_buttons[BTN_RIGHT] = 0;
	released_buttons[BTN_UP] = 0;

	pressed_buttons[BTN_LEFT] = 0;
	pressed_buttons[BTN_DOWN] = 0;
	pressed_buttons[BTN_RIGHT] = 0;
	pressed_buttons[BTN_UP] = 0;

#if defined BUTTON_LEFT
	if (!lwgpio_init(&btn_left, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
		{
			printf("Initializing button GPIO as input failed.\n");
			_task_block();
		}
	lwgpio_set_functionality(&btn_left, BSP_BUTTON1_MUX_GPIO);
	#if defined(BSP_BUTTONS_ACTIVE_HIGH)
		lwgpio_set_attribute(&btn_left, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
	#else
		lwgpio_set_attribute(&btn_left, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
	#endif
#endif

#if defined BUTTON_DOWN
	if (!lwgpio_init(&btn_down, BSP_BUTTON2, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
		{
			printf("Initializing button GPIO as input failed.\n");
			_task_block();
		}
	lwgpio_set_functionality(&btn_down, BSP_BUTTON2_MUX_GPIO);
	#if defined(BSP_BUTTONS_ACTIVE_HIGH)
		lwgpio_set_attribute(&btn_down, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
	#else
		lwgpio_set_attribute(&btn_down, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
	#endif
#endif
#if defined BUTTON_RIGHT
	if (!lwgpio_init(&btn_right, BSP_BUTTON3, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
		{
			printf("Initializing button GPIO as input failed.\n");
			_task_block();
		}
	lwgpio_set_functionality(&btn_right, BSP_BUTTON3_MUX_GPIO);
	#if defined(BSP_BUTTONS_ACTIVE_HIGH)
		lwgpio_set_attribute(&btn_right, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
	#else
		lwgpio_set_attribute(&btn_right, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
	#endif
#endif
#if defined BUTTON_UP
	if (!lwgpio_init(&btn_up, BSP_BUTTON4, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
		{
			printf("Initializing button GPIO as input failed.\n");
			_task_block();
		}
	lwgpio_set_functionality(&btn_up, BSP_BUTTON4_MUX_GPIO);
	#if defined(BSP_BUTTONS_ACTIVE_HIGH)
		lwgpio_set_attribute(&btn_up, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
	#else
		lwgpio_set_attribute(&btn_up, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
	#endif
#endif
#if defined BUTTON_CHANGE
	if (!lwgpio_init(&btn_change, BSP_BUTTON5, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
		{
			printf("Initializing button GPIO as input failed.\n");
			_task_block();
		}
	lwgpio_set_functionality(&btn_change, BSP_BUTTON5_MUX_GPIO);
	#if defined(BSP_BUTTONS_ACTIVE_HIGH)
		lwgpio_set_attribute(&btn_change, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
	#else
		lwgpio_set_attribute(&btn_change, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
	#endif
#endif

}
void initLeds(){
#ifdef BSP_LED1
	if (!lwgpio_init(&led_left, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing LED1 GPIO as output failed.\n");
		_task_block();
	}
	/* swich pin functionality (MUX) to GPIO mode */
	lwgpio_set_functionality(&led_left, BSP_LED1_MUX_GPIO);

	/* write logical 1 to the pin */
	lwgpio_set_value(&led_left, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
#ifdef BSP_LED2
	if (!lwgpio_init(&led_down, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing LED2 GPIO as output failed.\n");
		_task_block();
	}
	/* swich pin functionality (MUX) to GPIO mode */
	lwgpio_set_functionality(&led_down, BSP_LED2_MUX_GPIO);

	/* write logical 1 to the pin */
	lwgpio_set_value(&led_down, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
#ifdef BSP_LED3
	if (!lwgpio_init(&led_right, BSP_LED3, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing LED3 GPIO as output failed.\n");
		_task_block();
	}
	/* swich pin functionality (MUX) to GPIO mode */
	lwgpio_set_functionality(&led_right, BSP_LED3_MUX_GPIO);

	/* write logical 1 to the pin */
	lwgpio_set_value(&led_right, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
#ifdef BSP_LED4
	if (!lwgpio_init(&led_up, BSP_LED4, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing LED4 GPIO as output failed.\n");
		_task_block();
	}
	/* swich pin functionality (MUX) to GPIO mode */
	lwgpio_set_functionality(&led_up, BSP_LED4_MUX_GPIO);

	/* write logical 1 to the pin */
	lwgpio_set_value(&led_up, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
}
/******************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for mouse (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void TestApp_Init(void)
{       
    HID_CONFIG_STRUCT   config_struct;
    
    /* initialize the Global Variable Structure */
    USB_mem_zero(&g_mouse, sizeof(MOUSE_GLOBAL_VARIABLE_STRUCT));
    USB_mem_zero(&config_struct, sizeof(HID_CONFIG_STRUCT));
    
    /* Initialize the USB interface */
     
    config_struct.ep_desc_data = &usb_desc_ep;
    config_struct.hid_class_callback.callback = USB_App_Callback;
    config_struct.hid_class_callback.arg = &g_mouse.app_handle;
    config_struct.param_callback.callback = USB_App_Param_Callback;
    config_struct.param_callback.arg = &g_mouse.app_handle;
    config_struct.desc_callback_ptr = &desc_callback;
    config_struct.desc_endpoint_cnt = HID_DESC_ENDPOINT_COUNT;
    config_struct.ep = g_mouse.ep;

    if (MQX_OK != _usb_device_driver_install(USBCFG_DEFAULT_DEVICE_CONTROLLER)) {
        printf("Driver could not be installed\n");
        return;
    }
  
    g_mouse.app_handle = USB_Class_HID_Init(&config_struct);

	printf("\r\n");

    dx = 0;
    dy = 0;

    previous_button_state = BUTTON_RELEASED;

    speed_index = 0;

    speeds[SUPER_FAST] = 10000;
    speeds[FAST] = 30000;
    speeds[SLOW] = 120000;

    blink_speed = speeds[SUPER_FAST];

	initButtons();
	initLeds();

    while (TRUE) 
    {        
    	checkButtons();
        /* call the periodic task function */      
        USB_HID_Periodic_Task();
    } 
  
} 

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task( uint32_t param )
{   
    UNUSED_ARGUMENT (param)
    TestApp_Init();   
}

/* EOF */
