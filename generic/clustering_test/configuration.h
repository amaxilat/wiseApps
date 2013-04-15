/* 
 * File:   config.h
 * Author: amaxilatis
 *
 * Created on January 8, 2013, 12:50 PM
 */

#define CHANNEL 14

#define ENABLE_CLUSTERING

// Replace the first Algorithm name with one from the list in comment
//#define ECHO//ECHO ADAPTIVE
#define ADAPTIVE //ECHO ADAPTIVE
// Replace the first Algorithm name with one from the list in comment
#define FRONTS //FRONTS MOCA LCA

//#define ENABLE_UART_CL
#define FAILURES_PERCENTAGE 0 //*10%

//#define VIRTUAL_RADIO
//#define REMOTE_DEBUG

#define CHANGE_POWER
#define DB -6

//Set parameters of DUTY_CYCLING
#define DUTY_PERIOD 1000
#define DUTY_RATE 100   //percentage of being wake

