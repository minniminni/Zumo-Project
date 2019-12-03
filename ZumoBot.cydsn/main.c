/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include <stdlib.h>
#define FORWARD 100*0.5
#define RIGHT 200*0.5
#define LEFT 200*0.5

#define G5 255
#define G6 255 / 2
#define JOTAIN 225
/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 0
//assignment 1

void dot(void);
void dash(void);

void zmain(void)
    {
        while(1){
            
            dot();
            
            vTaskDelay(500);

            dash();

            dot(); 
            
            vTaskDelay(2000);
            
        }

    }

    void dot(void) {
        for (int i = 1; i <=3; i++) {
            BatteryLed_Write(1);
            Beep(50, 100);
            vTaskDelay(50);
            BatteryLed_Write(0);
            vTaskDelay(50);
        }
    }

    void dash(void) {
        for (int i = 1; i <=3; i++) {
             BatteryLed_Write(1);
            Beep(50, 100);
            vTaskDelay(250);
            BatteryLed_Write(0);
            vTaskDelay(250);

        }
    }

#endif

#if 0
//assignment 2
    
void zmain(void) {

    TickType_t start;
    TickType_t end;
    int time, age;
    printf("Enter your age: \n");
    start = xTaskGetTickCount();
    scanf("%d", &age);
    end = xTaskGetTickCount();
    time = (end - start)/1000;
    printf("Execution time: %d \n", time);

    if (age < 22) {
        
        if (time < 3) {
            printf("Super fast dude! \n");
        }
        
        else if (time < 5 && time >= 3) {
            printf("So medicore. \n");
        }
        
        else {
            printf("My granny is faster than you! \n");
        }
    }
    
    else if (age >= 22 && age <= 50) {
        
        if (time < 3) {
            printf("Be quick or be dead \n");
        }
        else if (time < 5 && time >= 3) {
            printf("You are so average. \n");
        }
        else {
            printf("Have you been smoking something illegal? \n");
        }
    }

    else {
        
        if (time < 3) {
            printf("Still going strong \n");
        }
        else if (time < 5 && time >= 3) {
            printf("You are doing ok for your age. \n");
        }
        else {
            printf("Do they still allow you to drive? \n");
        }
    }
    
    while(1){
        vTaskDelay(100);
    }
}

#endif

#if 0
//assignment 3
//int i;    
void flash(void);

void zmain(void){
    
    ADC_Battery_Start();

    int16 adcresult =0;
    float volts = 0.0;
    float percentage;
 
    printf("\nStart\n");

    BatteryLed_Write(0); 


    while(true)
    {

        ADC_Battery_StartConvert(); 
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {
            
            adcresult = ADC_Battery_GetResult16();
            percentage = (adcresult / 4095.0) * 5;
            volts = percentage * 1.5;

            printf("ADC value: %d Volts: %.2f\r\n", adcresult, volts);
            vTaskDelay(500);
        }

    while (volts < 4) {
            
        flash();

        if (SW1_Read() == 0) {
            
            BatteryLed_Write(0);
            break;

            }
        }
    }
}

void flash(void) {
for (int i = 1; i <=3; i++) {
BatteryLed_Write(1);
vTaskDelay(500);
BatteryLed_Write(0);
vTaskDelay(500);
}
}

#endif

#if 0
// Hello World!
void zmain(void)
{
    printf("\nHello, World!\n");

    while(true)
    {
        vTaskDelay(100); // sleep (in an infinite loop)
    }
 }   
#endif

#if 0
// Name and age
void zmain(void)
{
    char name[32];
    int age;
    
    
    printf("\n\n");
    
    printf("Enter your name: ");
    //fflush(stdout);
    scanf("%s", name);
    printf("Enter your age: ");
    //fflush(stdout);
    scanf("%d", &age);
    
    printf("You are [%s], age = %d\n", name, age);

    while(true)
    {
        BatteryLed_Write(!SW1_Read());
        vTaskDelay(100);
    }
 }   
#endif


#if 0
//battery level//
void zmain(void)
{
    ADC_Battery_Start();        

    int16 adcresult =0;
    float volts = 0.0;

    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed

    while(true)
    {
        char msg[80];
        ADC_Battery_StartConvert(); // start sampling
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            // you need to implement the conversion
            
            // Print both ADC results and converted value
            printf("%d %f\r\n",adcresult, volts);
        }
        vTaskDelay(500);
    }
 }   
#endif

#if 0
// button
void zmain(void)
{
    while(true) {
        printf("Press button within 5 seconds!\n");
        int i = 50;
        while(i > 0) {
            if(SW1_Read() == 0) {
                break;
            }
            vTaskDelay(100);
            --i;
        }
        if(i > 0) {
            printf("Good work\n");
            while(SW1_Read() == 0) vTaskDelay(10); // wait until button is released
        }
        else {
            printf("You didn't press the button\n");
        }
    }
}
#endif

#if 0
// button
void zmain(void)
{
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    while(true)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) vTaskDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif


#if 0
//ultrasonic sensor// week 3 //assignment 2
void zmain(void)
{
    
    vTaskDelay(1000);
    
    Ultra_Start();              // Ultra Sonic Start function
    motor_start();              // enable motor controller
       
    while (true){
        
        motor_forward(100, 10);    // moving forward
        
        int d = Ultra_GetDistance();

                printf("distance = %d\r\n", d);           // Print the detected distance (centimeters)

                    if(d < 10)
                    
                        {
                            Beep(100, 150);
                            
                            motor_backward(100, 500);     // moving backward
                            
                            motor_turn(200, 0, 500);      // turn
                            
                            //motor_forward(0,0);         // stop motors
                            
                            //motor_stop();               // disable motor controller
                            
                        }
                }        
}   
#endif

#if 0
//accelerometer// week 3 //assignment 3
void zmain(void)
    {
        
    vTaskDelay(1000);
    
    
    void randomrun();
    void escape();
    void attack();
    
    while(SW1_Read() == 1)             // Wait for user button
        {
            vTaskDelay(1000);
        }
    
    motor_start();              // enable motor controller
        
    struct accData_ data;

    printf("Accelerometer test...\n");
        if(!LSM303D_Start()){

            printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
            while(1) vTaskDelay(10);

        }
        
    else
    printf("Device Ok...\n");


    while(true)
        {     
            
            randomrun();
                          
            LSM303D_Read_Acc(&data);
            
            int x = data.accX * (-1);
            int y = data.accY * (-1);
            
            printf("x=%d \t y=%d\n", x, y);
            
            if (x > 2500)
            {
                //escape();
                attack();
            }
        }
    }
    
void randomrun(void)
    {
        int r = rand() % 1000;
     
        if (r < 990)
        {
            motor_forward(232, 10);
        }
            
        else if (r > 990 && r < 995)
        {
            motor_turn(150, 50, 500);
        }
            
        else if (r > 995)
        {
            motor_turn(50, 150, 500);
        }
    }
    
void escape(void)
    {
        int r = rand() % 10;
        
        motor_backward(150, 1000);
        
        if (r < 5)
        {
            motor_turn(200, 0, 500);
        }
        
        else if (r > 5)
        {
            motor_turn(0, 200, 500);
        }
    }
    
void attack(void)
    {
        int r = rand() % 10;
        
        motor_backward(150, 1000);
        
        if (r < 5)
        {
            motor_turn(200, 0, 500);
        }
        
        else if (r > 5)
        {
            motor_turn(0, 200, 500);
        }
    }    
        
#endif

#if 0
//IR receiverm - how to wait for IR remote commands
void zmain(void)
{
    IR_Start();
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    bool led = false;
    // Toggle led when IR signal is received
    while(true)
    {
        IR_wait();  // wait for IR command
        led = !led;
        BatteryLed_Write(led);
        if(led) printf("Led is ON\n");
        else printf("Led is OFF\n");
    }    
 }   
#endif



#if 0
//IR receiver - read raw data //week 4 //assigment 1//
void zmain(void)
{
    while(true){
        
        if (SW1_Read() == 0) 
            {
            
            motor_start();              // enable motor controller

            vTaskDelay(1000);
            
            motor_forward(100,2000);
             
            motor_forward(0,0);         // stop motors

            motor_stop();               // disable motor controller
           
            break;
            
            }
               
        }
    
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\nHello humans!!\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
        
    
    // print received IR pulses and their lengths
    while(true)
    {
        if(IR_get(&IR_val, portMAX_DELAY)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            
            Beep(150,150);
            //vTaskDelay(1000);
            IR_flush();
            
            break;
        }
    }
    
    while(true)
    {
        motor_start();
        motor_forward(100,2000);
        
        motor_forward(0,0);         // stop motors

        motor_stop();               // disable motor controller
        
        break;
    }
 }   
#endif


#if 0
//reflectance
    
int onblack, onwhite, passed;    
    
void zmain(void)
{   
    void linecounter();
    void reflectancedrive();
        
    IR_Start();    
    uint32_t IR_val;     
    IR_flush();
   
    motor_start();
    
    while(SW1_Read() == 1)             // Wait for user button
        {
            vTaskDelay(1000);
        }
    while(true)             // Drive to first line
    {
        
        linecounter();
        
        reflectancedrive();
        
        if(onblack==1)
            {
                Beep(100, 100);
                motor_forward(0, 0);
                break;
            }
               
    }
    
    while(true)             // Wait for IR signal
    {
        if(IR_get(&IR_val, portMAX_DELAY)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            
            Beep(150,150);
            IR_flush();
                      
            break;
        }
    }

    while(true)             // Drive
    {               
        linecounter();
        
        reflectancedrive();
    }
}   

void linecounter(void)
    {
        struct sensors_ ref;
        struct sensors_ dig;

        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        printf("Sensor value: %5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        printf("a=%d b=%d c=%d\n", onblack, onwhite, passed);
        
        if (dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1)
            {
                onblack = 1;               
            }
        
        if (dig.l3 != 1 && dig.r3 != 1)
            {
                onwhite = 1;    
            }      
            
        if(onblack == 1 && onwhite == 1)
            {
                passed++;
                onblack = 0;
            }
            
        else{ onwhite = 0; }    
    }
    
void reflectancedrive(void)
    {
    
        struct sensors_ ref;
        struct sensors_ dig;

        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
       
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        void tank_turn_left(uint8, uint32);
        void tank_turn_right(uint8, uint32);
    
        if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0) //forward
            {
                motor_forward(250, 10);
            }
            
            else if ((dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0)||(dig.l3 == 0 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0)) // 90 degree turn right
                    {
                        vTaskDelay(50);
                        
                        if ((dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0)||(dig.l3 == 0 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0)) // recheck
                        {
                            //motor_turn(0, 200, 400);
                            tank_turn_left(200, 200);
                        }          
                    }
        
                else if ((dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1)||(dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 0)) //90 degree turn left
                    {
                        vTaskDelay(50);
                        
                        if ((dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1)||(dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 0)) // reheck
                        {
                            //motor_turn(200, 0, 400);
                            tank_turn_right(200, 200);
                        }          
                    }
                      
                    else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 0) // turn right
                        {
                            motor_turn(200, 50, 50);             
                        }
                        
                             else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 1 && dig.r3 == 1) // hard turn right
                                {
                                    motor_turn(250, 0, 100);             
                                }
                                
                                else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 1) //extra hard turn right
                                {
                                    motor_turn(255, 0, 200);             
                                }
                            
                                else if (dig.l3 == 0 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // turn left
                                    {
                                        motor_turn(50, 200, 50);             
                                    }
                                
                                    else if (dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // hard turn left
                                        {
                                            motor_turn(0, 250, 100);             
                                        }
                                        
                                        else if (dig.l3 == 1 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // extra hard turn left
                                        {
                                            motor_turn(0, 255, 200);             
                                        }
                                    
                                            else if (passed == 2) // stop //"passed == 2" : stop while 2 lines passed
                                                {
                                                    motor_forward(0, 0);
                                                }
                    
                    else
                    {
                        motor_forward(100, 10);
                    }
    }
    
    void tank_turn_left(uint8 speed, uint32 delay)
    {
        MotorDirLeft_Write(1);      // set LeftMotor forward mode
        MotorDirRight_Write(0);     // set RightMotor forward mode
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }

    void tank_turn_right(uint8 speed, uint32 delay)
    {
        MotorDirLeft_Write(0);      // set LeftMotor forward mode
        MotorDirRight_Write(1);     // set RightMotor forward mode
        PWM_WriteCompare1(speed); 
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }
    
#endif



#if 0
//reflectance //tank turn
    
int onblack, onwhite, passed;   
void tank_turn_left(uint8, uint32);
void tank_turn_right(uint8, uint32);
void linecounter();
void reflectancedrive();
    
void zmain(void)
{   
    IR_Start();    
    uint32_t IR_val;     
    IR_flush();
   
    motor_start();
    
    while(SW1_Read() == 1);             // Wait for user button
    
    while(true)             // Drive to first line
    {
        
        linecounter();
        
        reflectancedrive();
        
        if(onblack==1)
            {
                Beep(100, 100);
                motor_forward(0, 0);
                break;
            }
               
    }
    
    while(true)             // Wait for IR signal
    {
        if(IR_get(&IR_val, portMAX_DELAY)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            
            Beep(150,150);
            IR_flush();
                      
            break;
        }
    }

    while(true)             // Drive
    {               
        linecounter();
        
        reflectancedrive();
    }
}   

void linecounter(void)
    {
        struct sensors_ ref;
        struct sensors_ dig;

        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        printf("Sensor value: %5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
        printf("a=%d b=%d c=%d\n", onblack, onwhite, passed);
        
        if (dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1)
            {
                onblack = 1;               
            }
        
        if (dig.l3 != 1 || dig.r3 != 1)
            {
                onwhite = 1;    
            }      
            
        if(onblack == 1 && onwhite == 1)
            {
                passed++;
                onblack = 0;
            }
            
        else{ onwhite = 0; }    
    }
    
void reflectancedrive(void)
    {
    
        struct sensors_ ref;
        struct sensors_ dig;

        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
       
        reflectance_read(&ref);
        reflectance_digital(&dig);
    
        if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0) //forward
            {
                motor_forward(FORWARD, 10);
            }
            
            else if (onblack == 1 && passed == 1)//(dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0) // 90 degree turn right
                    {
                        vTaskDelay(500);
                        tank_turn_left(100, 800);
                    }
        
                else if ((onblack == 1 && passed == 2) || (onblack == 1 && passed == 3))//(dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1) //90 degree turn left
                    {
                        vTaskDelay(500);
                        tank_turn_right(100, 800);      
                    }
                      
                    else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 0) // turn right
                        {
                            motor_turn(100, 50, 10);             
                        }
                        
                             else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 1 && dig.r3 == 1) // hard turn right
                                {
                                    motor_turn(150, 0, 10);             
                                }
                                
                                else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 1) //extra hard turn right
                                {
                                    motor_turn(150, 0, 10);             
                                }
                            
                                else if (dig.l3 == 0 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // turn left
                                    {
                                        motor_turn(50, 100, 10);             
                                    }
                                
                                    else if (dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // hard turn left
                                        {
                                            motor_turn(0, 150, 10);             
                                        }
                                        
                                        else if (dig.l3 == 1 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // extra hard turn left
                                        {
                                            motor_turn(0, 150, 10);             
                                        }
                                    
                                            else if (passed == 5) // stop
                                                {
                                                    motor_forward(0, 0);
                                                }
                    
                    else
                    {
                        motor_forward(FORWARD, 10);
                    }
    }
   
void tank_turn_left(uint8 speed, uint32 delay)
{
    MotorDirLeft_Write(1);      // set LeftMotor forward mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(speed);
    PWM_WriteCompare2(speed);
    vTaskDelay(delay);
}

void tank_turn_right(uint8 speed, uint32 delay)
{
    MotorDirLeft_Write(0);      // set LeftMotor forward mode
    MotorDirRight_Write(1);     // set RightMotor forward mode
    PWM_WriteCompare1(speed); 
    PWM_WriteCompare2(speed);
    vTaskDelay(delay);
}
    
#endif

#if 0
//motor //week 3 //assignment 1
void zmain(void)
{
    while (1){
        
        if (SW1_Read() == 0) {
        
        motor_start();              // enable motor controller
        
        motor_forward(0,0);         // set speed to zero to stop motors

        vTaskDelay(1000);
        
        motor_turn(105, 100, 3500);    // moving forward

        motor_turn(200,50,645);     // turn

        motor_turn(105, 100, 2600);    // moving forward

        motor_turn(200,50,645);     // turn
        
        motor_turn(105, 100, 2650);    // moving forward
        
        motor_turn(200,50,645);     // turn
        
        motor_turn(145,75,1800);    // turn inside
        
        motor_turn(105, 100, 1200);    // moving forward
        
        vTaskDelay(500);
        
        //motor_backward(100,2000);    // moving backward
         
        motor_forward(0,0);         // stop motors

        motor_stop();               // disable motor controller
        
        //while(true)
        //{
        //    vTaskDelay(100);
        //}
    }
    }
    
}
#endif

#if 0
/* Example of how to use te Accelerometer!!!*/
void zmain(void)
{
    struct accData_ data;
    
    printf("Accelerometer test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Device Ok...\n");
    }
    
    while(true)
    {
        LSM303D_Read_Acc(&data);
        printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        vTaskDelay(50);
    }
 }   
#endif    

#if 0
// MQTT test
void zmain(void)
{
    int ctr = 0;

    printf("\nBoot\n");
    send_mqtt("Zumo01/debug", "Boot");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 

    while(true)
    {
        printf("Ctr: %d, Button: %d\n", ctr, SW1_Read());
        print_mqtt("Zumo01/debug", "Ctr: %d, Button: %d", ctr, SW1_Read());

        vTaskDelay(1000);
        ctr++;
    }
 }   
#endif


#if 0
void zmain(void)
{    
    struct accData_ data;
    struct sensors_ ref;
    struct sensors_ dig;
    
    printf("MQTT and sensor test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Accelerometer Ok...\n");
    }
    
    int ctr = 0;
    reflectance_start();
    while(true)
    {
        LSM303D_Read_Acc(&data);
        // send data when we detect a hit and at 10 second intervals
        if(data.accX > 1500 || ++ctr > 1000) {
            printf("Acc: %8d %8d %8d\n",data.accX, data.accY, data.accZ);
            print_mqtt("Zumo01/acc", "%d,%d,%d", data.accX, data.accY, data.accZ);
            reflectance_read(&ref);
            printf("Ref: %8d %8d %8d %8d %8d %8d\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       
            print_mqtt("Zumo01/ref", "%d,%d,%d,%d,%d,%d", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);
            reflectance_digital(&dig);
            printf("Dig: %8d %8d %8d %8d %8d %8d\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
            print_mqtt("Zumo01/dig", "%d,%d,%d,%d,%d,%d", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
            ctr = 0;
        }
        vTaskDelay(10);
    }
 }   

#endif

#if 0
void zmain(void)
{    
    RTC_Start(); // start real time clock
    
    RTC_TIME_DATE now;

    // set current time
    now.Hour = 12;
    now.Min = 34;
    now.Sec = 56;
    now.DayOfMonth = 25;
    now.Month = 9;
    now.Year = 2018;
    RTC_WriteTime(&now); // write the time to real time clock

    while(true)
    {
        if(SW1_Read() == 0) {
            // read the current time
            RTC_DisableInt(); /* Disable Interrupt of RTC Component */
            now = *RTC_ReadTime(); /* copy the current time to a local variable */
            RTC_EnableInt(); /* Enable Interrupt of RTC Component */

            // print the current time
            printf("%2d:%02d.%02d\n", now.Hour, now.Min, now.Sec);
            
            // wait until button is released
            while(SW1_Read() == 0) vTaskDelay(50);
        }
        vTaskDelay(50);
    }
 }   
#endif

#if 0

    // PROJECT // 1:RING //

int ooxxoo, oxxooo, oooxxo, xooooo, ooooox, xoooox, x_or_x;    
int x_axle, y_axle, distance;
void sensors();    
void reflectance();
void linefollow();
void ring();
void escape();
void tank_turn_left(uint8, uint32);
void tank_turn_right(uint8, uint32);
    
void zmain(void)
{   
    IR_Start();    
    IR_flush();
    Ultra_Start();
    motor_start();
    
    while(SW1_Read() == 1);             // Wait for user button
    
    while(true)             // Drive to first line
    {
        sensors();
        
        linefollow();
        
        if(x_or_x == 1)
            {
                Beep(100,100);
                motor_forward(0, 0);
                break;
            }         
    }
    
    while(true)             // Wait for IR signal
    {
        IR_wait();
        Beep(100,50);
        Beep(100,100);
        Beep(100,150);
        Beep(100,200);
        motor_forward(255, 100);
        break;
    }

    while(true)             // Drive
    {               
        sensors();
        
        ring();
        
        escape();
    }
}   

// Functions:

    void sensors(void)
    {
        struct accData_ data;
        struct sensors_ ref;
        struct sensors_ dig;

        LSM303D_Start();
        reflectance_start();
        reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        LSM303D_Read_Acc(&data);
            
        x_axle = data.accX * (-1);  //effective value for x_axle is 2500!
        y_axle = data.accY * (-1);
            
        /*printf("x-axle=%d \t y-axle=%d\n", x_axle, y_axle);
        
        if (x_axle  > 3000)
            {
                printf("X\n");
                Beep(100,100);
            }*/
        
        distance = Ultra_GetDistance(); //set distance value
        
        if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 0 && dig.r3 == 0) //set ooxxoo value
            {
                ooxxoo = 1;
            }
            
        else if (dig.l3 == 0 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) //set oxxooo value
            {
                oxxooo = 1;
            }
                
        else if (dig.l3 == 0 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 0) // set oooxxo value
            {
                oooxxo = 1;
            }
            
        else if (dig.l3 == 1 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // set xooooo value
            {
                xooooo = 1;
            }
            
        else if (dig.l3 == 1 && dig.l2 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) // set ooooox value
            {
                ooooox = 1;
            }      
         
        else if (dig.l3 == 1 && dig.r3 == 1) //set xoooox value
            {
                xoooox = 1;
            }
            
        else if (dig.l3 == 1 || dig.r3 == 1) //set x_or_x value
            {
                x_or_x = 1;
            }    
                
        else ooxxoo = 0, oxxooo = 0, oooxxo = 0, xooooo = 0, ooooox = 0, xoooox = 0, x_or_x = 0;
    }
    
    void linefollow(void)
    {   
        if (ooxxoo == 1)
            {
                motor_forward(100, 1);
            }
            
        else if (oxxooo == 1)
            {
                motor_turn(100, 200, 1);
            }
            
        else if (oooxxo == 1)
            {
                motor_turn(200, 100, 1);
            }
                
        else motor_forward(100, 1);
    }
    
    void ring(void)
    {
        int r = rand() % 10;
        
        if (r < 8)                                      // drive forward
        {
            for (int i = 0; i <100; i++)
            {
                if (x_or_x != 1 && distance > 30)
                    {
                        motor_forward (100, 1);
                    }
                    
                else break;
            }
        }
            
        if (r == 8)                                      // turn left
                {
                    for (int i = 0; i <200; i++)
                    {
                        if (x_or_x != 1  && distance > 30)
                            {
                                motor_turn(0, 255, 1);
                            }
                            
                        else break;  
                    }
                }
            
        if (r == 9)                                      // turn right
            {
                for (int i = 0; i <200; i++)
                {
                    if (x_or_x != 1  && distance > 30)
                        {
                            motor_turn(255, 0, 1);
                        }
                        
                    else break;     
                }
            }
        
        if (x_or_x != 1  && distance < 30) //attack
            {
                motor_forward(255, 1);
            }
    }  
    
    void escape(void)
    {
        int r = rand() % 10;
        
        if (xoooox == 1)
            {
                motor_backward(255, 200);
                
                if (r < 5)
                {
                    tank_turn_left(255, 300);
                }
            
                    if (r > 5)
                    {
                        tank_turn_right(255, 300);
                    }
            }
        
        if (x_or_x == 1)
            {
                if (r < 5)
                {
                    motor_forward(0, 0);
                    tank_turn_left(255, 100);
                    motor_backward(255, 100);
                }
            
                    if (r > 5)
                    {
                        motor_forward(0, 0);
                        tank_turn_right(255, 100);
                        motor_backward(255, 100);
                    }
            }
            
        if (x_axle > 3000 && distance > 5)
            {
                Beep(100, 100);
                tank_turn_left(255, 200);
            }
            
    }

    void tank_turn_left(uint8 speed, uint32 delay)
    {
        MotorDirLeft_Write(1);      // set LeftMotor backward mode
        MotorDirRight_Write(0);     // set RightMotor forward mode
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }

    void tank_turn_right(uint8 speed, uint32 delay)
    {
        MotorDirLeft_Write(0);      // set LeftMotor forward mode
        MotorDirRight_Write(1);     // set RightMotor backward mode
        PWM_WriteCompare1(speed); 
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }
    
#endif



#if 1

    // PROJECT // 3:MAZE //


    
void zmain(void)
{
    while(SW1_Read() == 1)
    {
        //Beep(1000, G5);    //G-5
        //Beep(1000, G6);  //G-6
        Beep(1000, JOTAIN);  //G-6
    }
}
#endif


/* [] END OF FILE */
