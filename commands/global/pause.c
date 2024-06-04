#include <stdio.h>
#include "pico/stdlib.h"
#include "pirate.h"
#include "system_config.h"
#include "opt_args.h"
#include "ui/ui_term.h"
#include "ui/ui_help.h"
#include "usb_rx.h"
#include "ui/ui_cmdln.h"
#include "pirate/button.h"

static const char * const usage[]={
    "pause and wait for any key: pause",  
    "pause and wait for button press: pause -b", 
    "pause and wait for button or any key: pause -b -k", 
    "'x' key to exit: pause -x", 
};

void pause_handler(struct command_result *res){
    if(ui_help_show(res->help_flag,usage,count_of(usage), 0x00,0x00)) return;

    //check for -b button flag
    bool pause_for_button=cmdln_args_find_flag('b'|0x20);
    bool pause_for_key=cmdln_args_find_flag('k'|0x20) || !pause_for_button;
    bool exit_on_x=cmdln_args_find_flag('x'|0x20);

    //print option messages
    if(pause_for_key) printf("%s\r\n", t[T_PRESS_ANY_KEY]);
    if(pause_for_button) printf("%s\r\n", t[T_PRESS_BUTTON]);
    if(exit_on_x) printf("%s\r\n", t[T_PRESS_X_TO_EXIT]);
    
    for(;;){

        //even if we are not pausing for key or button, 
        //it is cleaner if we consume any errant characters to 
        //avoid unexpected behavior after the pause
        char c;
        if(rx_fifo_try_get(&c)){
            if(exit_on_x && ((c|0x20)=='x')) {
                res->error=true;
                printf("%s\r\n", t[T_EXIT]);
                return;
            }
            if(pause_for_key) return;
        }   


        if(pause_for_button){
            if(button_get(0)) return;
        }

        if(system_config.error){
            res->error=true;
            return;
        }

    }
 
}
