
    #include <stdio.h>
    #include "system.h"
    #include "includes.h"
    #include "altera_avalon_pio_regs.h"
    #include "sys/alt_irq.h"
    #include "sys/alt_alarm.h"
    #include "altera_avalon_pio_regs.h"
    #include "alt_types.h"
    #include <time.h>
    #include <sys/alt_timestamp.h>
    #include <sys/alt_cache.h>
    
    
    
    #define N 8192
    
    #define M 32
    
    #define DEBUG 1
    
    #define HW_TIMER_PERIOD 100 /* 100ms */
    
    /* Button Patterns */
    
    #define GAS_PEDAL_FLAG      0x08
    #define BRAKE_PEDAL_FLAG    0x04
    #define CRUISE_CONTROL_FLAG 0x02
    /* Switch Patterns */
    
    #define TOP_GEAR_FLAG       0x00000002
    #define ENGINE_FLAG         0x00000001
    
    /* LED Patterns */
    
    #define LED_RED_0  0x00000001 // Engine
    #define LED_RED_1  0x00000002 // Top Gear
    #define LED_RED_12 0x00001000 //position = [0,400]
    #define LED_RED_13 0x00002000 //position = [400,800]
    #define LED_RED_14 0x00004000
    #define LED_RED_15 0x00008000
    #define LED_RED_16 0x00010000 //position = [1600,2000]
    #define LED_RED_17 0x00020000 //position = [2000m, 2400m]
    
    #define LED_GREEN_0 0x0001 // Cruise Control activated
    #define LED_GREEN_2 0x0002 // Cruise Control Button
    #define LED_GREEN_3 0x0004 // Cruise Control Button
    #define LED_GREEN_4 0x0010 // Brake Pedal
    #define LED_GREEN_6 0x0040 // Gas Pedal
    
    #define TASK_STACKSIZE 2048
    
    OS_STK StartTask_Stack[TASK_STACKSIZE];
    OS_STK ControlTask_Stack[TASK_STACKSIZE];
    OS_STK VehicleTask_Stack[TASK_STACKSIZE];
    OS_STK WatchdogTask_Stack[TASK_STACKSIZE];
    OS_STK DetectionTask_Stack[TASK_STACKSIZE];
    // Task Priorities
    
    #define STARTTASK_PRIO      5
    #define VEHICLETASK_PRIO    10
    #define CONTROLTASK_PRIO    12
    #define DETECTIONTASK_PRIO  13
    #define WATCHDOGTASK_PRIO   14
    
    
    // Task Periods
    
    #define CONTROL_PERIOD  300
    #define VEHICLE_PERIOD  300
    
    /*
     * Definition of Kernel Objects
     */
    
    // Mailboxes
    OS_EVENT *Mbox_Throttle;
    OS_EVENT *Mbox_Velocity;
    OS_EVENT *Mbox_Writeok;
    
    // Semaphores
    OS_EVENT *aSemaphore;
    OS_EVENT *bSemaphore;
    
    // SW-Timer
    OS_TMR *MyTmr1;
    alt_u32 ticks;
    alt_u32 time_1;
    alt_u32 time_2;
    alt_u32 timer_overhead;
    /*
     * Types
     */
    enum active {on, off};
    
    enum active gas_pedal = off;
    enum active brake_pedal = off;
    enum active top_gear = off;
    enum active engine = off;
    enum active cruise_control = off;
    
    /*
     * Global variables
     */
    int delay; // Delay of HW-timer
    INT16U led_green = 0; // Green LEDs
    INT32U led_red = 0;   // Red LEDs
    /*alt_u32 ticks;
    alt_u32 time_1;
    alt_u32 time_2;
    alt_u32 timer_overhead;*/
    void TmrCallback()
    {
        // Post to the semaphore to signal that it's time to run the task.
        OSSemPost(aSemaphore); // Releasing the key
        OSSemPost(aSemaphore);
    
    
    }
    
    int buttons_pressed(void)
    {
        return ~IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_KEYS4_BASE);
    }
    
    int switches_pressed(void)
    {
        return IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_TOGGLES18_BASE);
    }
    
    /*
     * ISR for HW Timer
     */
    alt_u32 alarm_handler(void* context)
    {
        OSTmrSignal(); /* Signals a 'tick' to the SW timers */
    
        return delay;
    }
   /* 

    /*void shownumberonleds(int x)
    {
    IOWR_ALTERA_AVALON_PIO_DATA( LED_PIO_BASE , x ) ;
    }*/
    
    static int b2sLUT[] = {0x40, //0
                           0x79, //1
                           0x24, //2
                           0x30, //3
                           0x19, //4
                           0x12, //5
                           0x02, //6
                           0x78, //7
                           0x00, //8
                           0x18, //9
                           0x3F, //-
                          };
    
    /*
     * convert int to seven segment display format
     */
    int int2seven(int inval) {
        return b2sLUT[inval];
    }
    
    /*
     * output current velocity on the seven segement display
     */
    void show_velocity_on_sevenseg(INT8S velocity) {
        int tmp = velocity;
        int out;
        INT8U out_high = 0;
        INT8U out_low = 0;
        INT8U out_sign = 0;
    
        if (velocity < 0) {
            out_sign = int2seven(10);
            tmp *= -1;
        } else {
            out_sign = int2seven(0);
        }
    
        out_high = int2seven(tmp / 10);
        out_low = int2seven(tmp - (tmp/10) * 10);
    
        out = int2seven(0) << 21 |
              out_sign << 14 |
              out_high << 7  |
              out_low;
        IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_HEX_LOW28_BASE,out);
    }
    
    
    /*
     * output current position on the seven segement display
     */
    void show_position_on_sevenseg(INT8S position) {
    
    
    }
    
    void show_target_velocity(INT8S velocity) {
        int tmp = velocity;
        int out;
        INT8U out_high = 0;
        INT8U out_low = 0;
        INT8U out_sign = 0;
    
        if (velocity < 0) {
            out_sign = int2seven(10);
            tmp *= -1;
        } else {
            out_sign = int2seven(0);
        }
    
        out_high = int2seven(tmp / 10);
        out_low = int2seven(tmp - (tmp/10) * 10);
    
        out = int2seven(0) << 21 |
              out_sign << 14 |
              out_high << 7  |
              out_low;
        if (cruise_control == off) {
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_HEX_HIGH28_BASE,out);
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_0);
        } else {
    
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_HEX_HIGH28_BASE,0x00);
        }
    }
    
    /*
     * indicates the position of the vehicle on the track with the four leftmost red LEDs
     * LEDR17: [0m, 400m)
     * LEDR16: [400m, 800m)
     * LEDR15: [800m, 1200m)
     * LEDR14: [1200m, 1600m)
     * LEDR13: [1600m, 2000m)
     * LEDR12: [2000m, 2400m]
     */
    void show_position(INT16U position)
    {
     if (position < 4000) 
        if (engine==on)
            if(top_gear==on)
            // Turn ON only those LED below remain unchanged the other LEDS
                IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_1|LED_RED_17);
            else
                IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_17);
        else
            if(top_gear==on)
                IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_1|LED_RED_17);
            else
                IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_17);
                
    else
      if (position < 8000)
          if (engine==on)
              if(top_gear==on)
                  IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_1|LED_RED_16);
              else
                  IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_16);
          else
              if(top_gear==on)
                  IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_1|LED_RED_16);
              else
                  IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_16);
                  
      else 
        if (position < 12000)
            if (engine==on)
                if(top_gear==on)
                    IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_1|LED_RED_15);
                else
                    IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_15);
            else
                if(top_gear==on)
                    IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_1|LED_RED_15);
                else
                    IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_15);
                    
        else
          if (position < 16000)
                if (engine==on)
                    if(top_gear==on)
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_1|LED_RED_14);
                    else
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_14);
                else
                    if(top_gear==on)
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_1|LED_RED_14);
                    else
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_14);
                        
          else
            if (position < 20000)
                if (engine==on)
                    if(top_gear==on)
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_1|LED_RED_13);
                    else
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_13);
                else
                    if(top_gear==on)
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_1|LED_RED_13);
                    else
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_13);
            else
                if (engine==on)
                    if(top_gear==on)
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_1|LED_RED_12);
                    else
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_0|LED_RED_12);
                else
                    if(top_gear==on)
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_1|LED_RED_12);
                    else
                        IOWR_ALTERA_AVALON_PIO_DATA (DE2_PIO_REDLED18_BASE,LED_RED_12);

        
    }
    
    /*
     * The function 'adjust_position()' adjusts the position depending on the
     * acceleration and velocity.
     */
    INT16U adjust_position(INT16U position, INT16S velocity,
                           INT8S acceleration, INT16U time_interval)
    {
        INT16S new_position = position + velocity * time_interval / 1000
                              + acceleration / 2  * (time_interval / 1000) * (time_interval / 1000);
    
        if (new_position > 24000) {
            new_position -= 24000;
        } else if (new_position < 0) {
            new_position += 24000;
        }
    
        //show_position(new_position);
        return new_position;
    }
    
    /*
     * The function 'adjust_velocity()' adjusts the velocity depending on the
     * acceleration.
     */
    INT16S adjust_velocity(INT16S velocity, INT8S acceleration,
                           enum active brake_pedal, INT16U time_interval)
    {
        INT16S new_velocity;
        INT8U brake_retardation = 200;
    
        if (brake_pedal == off)
            new_velocity = velocity  + (float) (acceleration * time_interval) / 1000.0;
        else {
            if (brake_retardation * time_interval / 1000 > velocity)
                new_velocity = 0;
            else
                new_velocity = velocity - brake_retardation * time_interval / 1000;
        }
    
        return new_velocity;
    }
    
    int cruise_velocity =0;
    int cruise_control_increase_velocity = 0;
    int cruise_control_decrease_velocity = 0;
    void Button1IO(INT16S* current_velocity)
    {
        printf("Button1IO %d\n", buttons_pressed() );
        if (buttons_pressed()==-14 && *current_velocity >= 20 && top_gear == on && gas_pedal == off && brake_pedal==off) {
            cruise_control = on;
            cruise_velocity = *current_velocity;
            show_target_velocity(cruise_velocity/10);
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_0);
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_2);
        }
        else if (buttons_pressed() == -16) {
            if (cruise_control == 0 && cruise_velocity > *current_velocity && cruise_velocity >0) {
                cruise_control_increase_velocity = 1;
                IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_0);
            } else if (cruise_control == 0 && cruise_velocity < *current_velocity && cruise_velocity >0) {
                cruise_control_decrease_velocity = 1;
                IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_0);
            } else {
    
                IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, 0x0000);
            }
        }
        printf("buttons_pressed: %d\n",buttons_pressed() );
    }
    
    void Button2IO(INT16S current_velocity)
    {
        if (buttons_pressed() == -12) {
            brake_pedal = on;
            cruise_control = off;
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE,0x00000);//clear
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE,LED_GREEN_4);
            cruise_velocity = 0;
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_HEX_HIGH28_BASE,0x00);//clear target velocity
        }
        else {
            brake_pedal = off;
            if (! cruise_velocity > 0  ) {
                IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE,0x00000);
            } else {
                if (cruise_control == 0) {
                    IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_0);
                }
            }
        }
    }
    void ButtonIO(INT16S* current_velocity, INT8U throttle)
    {
        int btn_reg = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_KEYS4_BASE);
        if (btn_reg == 7) { //buttons_pressed() == -8){
            gas_pedal = on;
            cruise_control = off;
            IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE,LED_GREEN_6);
        } else if (buttons_pressed() == -16) {
            gas_pedal = off;
            if (! cruise_velocity > 0  ) {
                IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE,0x00000);
            } else {
                IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_GREENLED9_BASE, LED_GREEN_0);
            }
        }
        printf("ButtonIO buttons_pressed: %d\n",  buttons_pressed());
    }
    float microseconds(int ticks)
    {
        return (float) 1000000 * (float) ticks / (float) alt_timestamp_freq();
    }
    void initArray(int x[], int n)
    {
        int i;
    
        for (i = 0; i < n; i++)
            x[i] = i;
    }
    void start_measurement()
    {
        /* Flush caches */
        alt_dcache_flush_all();
        alt_icache_flush_all();
        /* Measure */
        alt_timestamp_start();
        time_1 = alt_timestamp();
    }
    void stop_measurement()
    {
        time_2 = alt_timestamp();
        ticks = time_2 - time_1;
    }
    void SwitchIO(INT16S* current_velocity, int position)
    {
        int w[700];
        int tmp = 600;
        int x[13];
        int a, b;
        int size = M;
        int i, j;
        timer_overhead = 0;
    
        if (switches_pressed()==1) {
            engine = on;
            top_gear=off;
            show_position(position);
            printf("The engine is turned on\n");
        } else  if (switches_pressed()==3) {
            engine = on;
            top_gear=on;
            show_position(position);
        }
        else if (switches_pressed()==0) {
            printf("switches_pressed()==0\n");
            top_gear = off;
            engine = off;
            show_position(position);
        }
        else if (switches_pressed()==2) {
    
        }
        else if ((switches_pressed()-19)/16 >= 0) {
            int number = (switches_pressed()-19)/16+1;
            if (number > 50) {
                number = 50;
            }
            printf("extra load %d\n", number);
            for (i = 0; i < 10; i++) {
                start_measurement();
                stop_measurement();
                timer_overhead = timer_overhead + time_2 - time_1;
            }
            initArray(w, 600);
            initArray(x, 13);
            start_measurement();
    
            j=tmp+number*4;
            for (i = 0; i < j; i++)
                w[i]++;
            stop_measurement();
            printf("%5.2f us", (float) microseconds(ticks - timer_overhead));
            printf("(%d ticks)\n", (int) (ticks - timer_overhead));
    
        }
        printf("switches_pressed: %d\n", switches_pressed());
        printf("switches_pressed: %d\n", (switches_pressed()-19)/16);
    
    }
    
    void pollkey()
    {
        int btn_reg;
        static int last_value = -1;
        btn_reg = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_KEYS4_BASE);
        btn_reg = (~btn_reg) & 0xf;
        if (last_value != btn_reg)
        {
            last_value = btn_reg;
            switch (btn_reg)
            {
            case 1:
                //run = 1;
                break;
            case 2:
                //run = 0;
                break;
            case 4:
                //tick (&timeloc);
                break;
            case 8:
                //timeloc = 0;
                break;
            default:
                break;
            }
        }
    }
    
    
    int globalPosition = 0;
    
    void setGlobalPosition(int position) {
    
        globalPosition = position;
    }
    
    int getGlobalPosition() {
        return globalPosition;
    }
    /*
     * The task 'VehicleTask' updates the current velocity of the vehicle
     */
    void VehicleTask(void* pdata)
    {
        INT8U err;
        void* msg;
        INT8U* throttle;
        INT8S acceleration;  /* Value between 40 and -20 (4.0 m/s^2 and -2.0 m/s^2) */
        INT8S retardation;   /* Value between 20 and -10 (2.0 m/s^2 and -1.0 m/s^2) */
        INT16U position = 0; /* Value between 0 and 20000 (0.0 m and 2000.0 m)  */
        INT16S velocity = 0; /* Value between -200 and 700 (-20.0 m/s amd 70.0 m/s) */
        INT16S wind_factor;   /* Value between -10 and 20 (2.0 m/s^2 and -1.0 m/s^2) */
    
        printf("Vehicle task created!\n");
        while (1)
        {
            OSSemPend(aSemaphore, 0, &err); // Trying to access the key
            err = OSMboxPost(Mbox_Velocity, (void *) &velocity);
            /* Non-blocking read of mailbox:
             - message in mailbox: update throttle
             - no message:         use old throttle
            */
            msg = OSMboxPend(Mbox_Throttle, 1, &err);
            if (err == OS_NO_ERR)
                throttle = (INT8U*) msg;
    
            /* Retardation : Factor of Terrain and Wind Resistance */
            if (velocity > 0)
                wind_factor = velocity * velocity / 10000 + 1;
            else
                wind_factor = (-1) * velocity * velocity / 10000 + 1;
    
            if (position < 4000)
                retardation = wind_factor; // even ground
            else if (position < 8000)
                retardation = wind_factor + 15; // traveling uphill
            else if (position < 12000)
                retardation = wind_factor + 25; // traveling steep uphill
            else if (position < 16000)
                retardation = wind_factor; // even ground
            else if (position < 20000)
                retardation = wind_factor - 10; //traveling downhill
            else
                retardation = wind_factor - 5 ; // traveling steep downhill
    
            acceleration = *throttle / 2 - retardation;
            position = adjust_position(position, velocity, acceleration, 300);
            setGlobalPosition(position);
            velocity = adjust_velocity(velocity, acceleration, brake_pedal, 300);
            show_position(position);
            printf("Position: %dm\n", position / 10);
            printf("Velocity: %4.1fm/s\n", velocity /10.0);
            printf("Throttle: %dV\n", *throttle / 10);
            show_velocity_on_sevenseg((INT8S) (velocity / 10));
            // break inside vehicle task
            Button2IO( (velocity/10) );
        }
    }
    
    /*
     * The task 'ControlTask' is the main task of the application. It reacts
     * on sensors and generates responses.
     */
    
    void ControlTask(void* pdata)
    {
        INT8U err;
        INT8U throttle = 40; /* Value between 0 and 80, which is interpreted as between 0.0V and 8.0V */
        void* msg;
        INT16S* current_velocity;
        int btn_reg;
        INT16S* current_output;
        printf("Control Task created!\n");
    
        while (1)
        {
    
            OSSemPend(aSemaphore, 1, &err); // Trying to access the key
            msg = OSMboxPend(Mbox_Velocity, 0, &err);
            current_velocity = (INT16S*) msg;
            //printf("Control Task!\n");
            ButtonIO(current_velocity, throttle);
            btn_reg = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_KEYS4_BASE);
            //printf("btn_reg %d\n", btn_reg);
            int increment = 30;
            if (btn_reg == 7) {
                ++throttle;
            } else if (cruise_control_increase_velocity == 1) {
                //printf("increase velocity \n");
                if (*current_velocity <= cruise_velocity) {
                    throttle = throttle + increment - 16;
                }
    
    
                cruise_control_increase_velocity = 0;
            }
            else if (btn_reg == 11) {
                if (throttle > 0) {
                    --throttle;
                }
    
            } else if (cruise_control_decrease_velocity == 1 && throttle >= increment && * current_velocity >= cruise_velocity) {
                //printf("decrease_velocity \n");
            
                    throttle = throttle -increment;
    
                cruise_control_decrease_velocity = 0;
            }
            if (btn_reg== 13) {
                //printf("do cruise control\n" );
                cruise_velocity = *current_velocity;
            }
            Button1IO(current_velocity);
            SwitchIO(current_velocity, getGlobalPosition());
            err = OSMboxPost(Mbox_Throttle, (void *) &throttle);
        }
    }
    void WatchDogTask(void* pdata)
    {
        INT8U err;
        void* msg;
        INT8U* throttle;
        printf("Watchdog task created!\n");
        INT16S* current_output;
        OS_SEM_DATA sem_data;
        INT8U err2;
        int i;
        while (1)
        {
            for (i = 0; i < 10; i++) {
                start_measurement();
                stop_measurement();
                timer_overhead = timer_overhead + time_2 - time_1;
            }
    
            start_measurement();
            OSSemPend(bSemaphore, 1, &err); // Trying to access the key
            msg = OSMboxPend(Mbox_Writeok, 0, &err);
            current_output = (INT16S*) msg;
            printf("Watchdog:%d\n", * current_output);
            if ( * current_output != 17 ) {
                printf("System overload:%d\n", * current_output);
            }
    
            stop_measurement();
            float measure = (float) microseconds(ticks - timer_overhead);
            if (measure > 7600 && measure < 10000) {
                printf("Overload warning");
            }
            printf("Watchdog %5.2f us", (float) microseconds(ticks - timer_overhead));
            printf("(%d ticks)\n", (int) (ticks - timer_overhead));
    
    
            current_output = 0;
        }
    }
    void DetectionTask(void* pdata)
    {
        INT8U err;
        printf("DetectionTask created!\n");
        int next = 17;
        while (1)
        {
            OSSemPend(bSemaphore, 0, &err); // Trying to access the key
            err = OSMboxPost(Mbox_Writeok, (void *) &next);
            /* Check “err” */
    
        }
    }
    /*
     * The task 'StartTask' creates all other tasks kernel objects and
     * deletes itself afterwards.
     */
    
    void StartTask(void* pdata)
    {
        INT8U err;
        void* context;
        IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_REDLED18_BASE,0X00000);
        IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_HEX_HIGH28_BASE,0X00000);
        static alt_alarm alarm;     /* Is needed for timer ISR function */
    
        /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
        delay = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000;
        printf("delay in ticks %d\n", delay);
        //printf("Starting time measurement\n");
        //startTimeMeasurement();
        /*
         * Create Hardware Timer with a period of 'delay'
         */
        if (alt_alarm_start (&alarm,
                             delay,
                             alarm_handler,
                             context) < 0)
        {
            printf("No system clock available!n");
        }
    
        /*
         * Create and start Software Timer
         */
    
        MyTmr1 = OSTmrCreate(0,
                              (CONTROL_PERIOD/100),
                              OS_TMR_OPT_PERIODIC,
                              TmrCallback,
                              NULL,
                              NULL,
                              &err);
        if (err == OS_ERR_NONE) {
            /* Timer was created */
            printf("Soft timer was created \n");
        }
    
        BOOLEAN status = OSTmrStart(MyTmr1,
                                    &err);
        if (status > 0 && err == OS_ERR_NONE) {
            /* Timer was started */
            printf("Soft timer was started!\n");
        }
        /*
         * Creation of Kernel Objects
         */
    
        // Mailboxes
        Mbox_Throttle = OSMboxCreate((void*) 0); /* Empty Mailbox - Throttle */
        Mbox_Velocity = OSMboxCreate((void*) 0); /* Empty Mailbox - Velocity */
        Mbox_Writeok = OSMboxCreate((void*) 0); /* Empty Mailbox - Write ok */
    
        /*
         * Create statistics task
         */
    
        OSStatInit();
    
        /*
         * Creating Tasks in the system
         */
    
        err = OSTaskCreateExt(
                  ControlTask, // Pointer to task code
                  NULL,        // Pointer to argument that is
                  // passed to task
                  &ControlTask_Stack[TASK_STACKSIZE-1], // Pointer to top
                  // of task stack
                  CONTROLTASK_PRIO,
                  CONTROLTASK_PRIO,
                  (void *)&ControlTask_Stack[0],
                  TASK_STACKSIZE,
                  (void *) 0,
                  OS_TASK_OPT_STK_CHK);
    
        err = OSTaskCreateExt(
                  VehicleTask, // Pointer to task code
                  NULL,        // Pointer to argument that is
                  // passed to task
                  &VehicleTask_Stack[TASK_STACKSIZE-1], // Pointer to top
                  // of task stack
                  VEHICLETASK_PRIO,
                  VEHICLETASK_PRIO,
                  (void *)&VehicleTask_Stack[0],
                  TASK_STACKSIZE,
                  (void *) 0,
                  OS_TASK_OPT_STK_CHK);
    
        err = OSTaskCreateExt(
                  WatchDogTask, // Pointer to task code
                  NULL,        // Pointer to argument that is
                  // passed to task
                  &WatchdogTask_Stack[TASK_STACKSIZE-1], // Pointer to top
                  // of task stack
                  WATCHDOGTASK_PRIO,
                  WATCHDOGTASK_PRIO,
                  (void *)&WatchdogTask_Stack[0],
                  TASK_STACKSIZE,
                  (void *) 0,
                  OS_TASK_OPT_STK_CHK);
    
        err = OSTaskCreateExt(
                  DetectionTask, // Pointer to task code
                  NULL,        // Pointer to argument that is
                  // passed to task
                  &DetectionTask_Stack[TASK_STACKSIZE-1], // Pointer to top
                  // of task stack
                  DETECTIONTASK_PRIO,
                  DETECTIONTASK_PRIO,
                  (void *)&DetectionTask_Stack[0],
                  TASK_STACKSIZE,
                  (void *) 0,
                  OS_TASK_OPT_STK_CHK);
    
    
    
        printf("All Tasks and Kernel Objects generated!\n");
        /* Task deletes itself */
    
        OSTaskDel(OS_PRIO_SELF);
    }
    
    int main(void) {
    
        printf("Cruise Control 20141010\n");
        aSemaphore = OSSemCreate(1); // binary semaphore (1 key)
        bSemaphore = OSSemCreate(1); // binary semaphore (1 key)
    
        OSTaskCreateExt(
            StartTask, // Pointer to task code
            NULL,      // Pointer to argument that is
            // passed to task
            (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
            // of task stack
            STARTTASK_PRIO,
            STARTTASK_PRIO,
            (void *)&StartTask_Stack[0],
            TASK_STACKSIZE,
            (void *) 0,
            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    
        OSStart();
    
        return 0;
    }



