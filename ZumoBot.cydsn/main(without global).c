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
    
//=================Zumo robot project. Metropolia University of Applied Sciences. Helsinki 13.12.2019=================//
    
            //===========Electric lady group: Alexander Seleznev, Minni Ojala, Jan Ranta===========//

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

/* ========================================
 *
 * Convert music notes to 8 bit value for Zumo PWM Buzzer
 *
 * max_value / ( (2^(1/12)) ^ semitone)
 *
 * ========================================
*/

//natural notes:

            #define G1 255
            #define A1 227
            #define B1 202
            #define C1 191
            #define D1 170
            #define E1 152
            #define F1 143
            #define G2 128
            #define A2 114
            #define B2 101
            #define C2 96
            #define D2 85
            #define E2 76
            #define F2 72
            #define G3 64
            #define A3 57
            #define B3 51
            #define C3 48
            #define D3 43
            #define E3 38
            #define F3 36
            #define G4 32
            
                            //sharp notes:
            
                            #define Gs1 241
                            #define As1 214
                            #define Cs1 180
                            #define Ds1 161
                            #define Fs1 135
                            #define Gs2 120
                            #define As2 107
                            #define Cs2 90
                            #define Ds2 80
                            #define Fs2 68
                            #define Gs3 60
                            #define As3 54
                            #define Cs3 45
                            #define Ds3 40
                            #define Fs3 34

/**
 * @file    main.c
 * @brief
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 1   // 1 = program enabled; 0 = program disbled;

    // PROJECT // 1:RING //

TickType_t starttime, finishtime, timepass;
int online();
int x_or_x();
void linefollow();
void ring();
void escape();
void tank_turn_left(uint8, uint32);
void tank_turn_right(uint8, uint32);
    
void zmain(void)    // main program
{
    IR_Start();
    reflectance_start();
    IR_flush();
    Ultra_Start();
    motor_start();
    
    struct sensors_ dig;
    reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance treshold
    reflectance_digital(&dig);
    
    while(SW1_Read() == 1); // wait for user button
    
    while(x_or_x() != 1)    // drive to first line
        {
            online();
            linefollow();
        }
        
    motor_forward(0, 0);
    print_mqtt("Zumo021/ready ", "zumo");   // send mqtt message ready
    
    while(true) // wait for IR signal
        {
            IR_wait();
            
            starttime = xTaskGetTickCount();    // set starttime
            print_mqtt("Zumo021/start ", "%d", starttime);  //print mqtt message starttime
            
            motor_forward(255, 100);
            
            break;
        }

    while(SW1_Read() == 1)             // drive in ring; exit while user button is pressed
        {
            online();
            
            ring();
            
            escape();
        }
    
    motor_stop();
    
    finishtime = xTaskGetTickCount();   // set finishtime
    print_mqtt("Zumo021/stop ", "%d", finishtime);  // print mqtt message finishtime
    
    timepass = finishtime - starttime;  // count timepass
    print_mqtt("Zumo021/time ", "%d", timepass);    // pint mqtt message timepass
    
}

int online()    // get values from reflectance sensors
    {
        struct sensors_ dig;
        
        int line = 32*dig.l3 + 16*dig.l2 + 8*dig.l1 + 4*dig.r1 + 2*dig.r2 + 1*dig.r3;   // set all sensors values in one variable using binary numbers
        
        return line;
    }

int x_or_x()    // set exception value if only l3 or r3 is active
    {
        int x_or_x;
        
        struct sensors_ dig;
        
        if (dig.l3 == 1 || dig.r3==1)
            {
                x_or_x = 1;
            }
            
        else x_or_x = 0;
            
        return x_or_x;
    }
  
void linefollow(void)   // linefollow function; using binary numbers in if statements for better representation
    {
        if (online() == 0b001100)
            {
                motor_forward(100, 0);  // go forward
            }
            
        else if (online() == 0b011000)
            {
                motor_turn(50, 200, 0); // slight left
            }
            
        else if (online() == 0b000110)
            {
                motor_turn(200, 50, 0); // slight right
            }
                
        else motor_forward(100, 0);
    }
    
void ring(void) // ring function for competition; drive forward and take random turns
    {
        int r = rand() % 2; // get random number for turns
        
        for (int i = 0; i <500; i++)    // drive forward
            {
                x_or_x();
                
                if (x_or_x() != 1 && Ultra_GetDistance() > 30)
                    {
                        motor_forward (200, 1);
                    }
                    
                else break;
            }
            
        if (r == 0) // turn left
                {
                    for (int i = 0; i <200; i++)
                        {
                            x_or_x();
                            
                            if (x_or_x() != 1 && Ultra_GetDistance() > 30)
                                {
                                    motor_turn(0, 255, 1);
                                }
                                
                            else break;
                        }
                }
            
        if (r == 1) // turn right
            {
                for (int i = 0; i <200; i++)
                    {
                        x_or_x();
                        
                        if (x_or_x() != 1 && Ultra_GetDistance() > 30)
                            {
                                motor_turn(255, 0, 1);
                            }
                            
                        else break;
                    }
            }
        
        if (x_or_x() != 1 && Ultra_GetDistance() < 30) //attack if distance to enemy is < 30 cm
            {
                motor_forward(255, 0);
            }
    }
    
void escape(void)   // escape function if robot is on edge
    {
        int r = rand() % 2; // get random number for turns
        
        if (x_or_x() == 1)  // if edge detected -> drive backward
            {
                motor_backward(255, 200);
            }
            
        if (x_or_x() == 1 && r == 0)    // take random turns
                {
                    tank_turn_left(255, 200);
                    motor_forward(0, 0);
                }
                
                else if (x_or_x() == 1 && r == 1)
                {
                    tank_turn_right(255, 200);
                    motor_forward(0, 0);
                }
    }
    
void tank_turn_left(uint8 speed, uint32 delay)  // custom tank turn function
    {
        MotorDirLeft_Write(1);      // set LeftMotor backward mode
        MotorDirRight_Write(0);     // set RightMotor forward mode
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }

void tank_turn_right(uint8 speed, uint32 delay)  // custom tank turn function
    {
        MotorDirLeft_Write(0);      // set LeftMotor forward mode
        MotorDirRight_Write(1);     // set RightMotor backward mode
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }
    
#endif

#if 1   // 1 = program enabled; 0 = program disbled;

    // PROJECT // 2:LINE FOLLOWER //

int online();
int linecounter();
void drive();
void reveille();
void darude();
    
void zmain(void)    // main program
{
    IR_Start();
    IR_flush();
    reflectance_start();
    motor_start();
    
    TickType_t starttime, finishtime, timepass;
    struct sensors_ dig;
    reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance treshold
    reflectance_digital(&dig);

    while (SW1_Read() == 1);    // wait for user button
    
    while (true)                // drive to fisrt line
        {
            online();
            drive();
            
            if (online() == 0b111111)   // stop when on line
                {
                    motor_forward(0, 0);
                    Beep(100, 100);
                    print_mqtt("Zumo021/ready ", "line");   // print mqtt message
                    break;
                }
                
            else motor_forward(100, 0);
        }
    
    while(true)                 // wait for IR signal
    {
        IR_wait();
        
        reveille(); // play music
        
        starttime = xTaskGetTickCount();    // get starttime
        print_mqtt("Zumo021/start ", "%d", starttime);  // print mqtt message starttime
        
        motor_forward(100, 100);
        
        break;
    }
    
    while (true)    // drive from start to finish
        {
            online();
            linecounter();
            drive();
            
            if (linecounter() == 1 && online() == 0b111111) // stop on second line
                {
                    motor_forward(0, 0);
                    
                    finishtime = xTaskGetTickCount();   // get finishtime
                    
                    print_mqtt("Zumo021/stop ", "%d", finishtime);  // print mqtt message finishtime
                    
                    timepass = finishtime - starttime;  // count passed time
                    
                    print_mqtt("Zumo021/time ", "%d", timepass);    // print mqtt message timepass
                    
                    darude();   // play music
                    
                    break;
                }
        }
        
}
    
int online()    // get values from reflectance sensors
    {
        struct sensors_ dig;
        
        int line = 32*dig.l3 + 16*dig.l2 + 8*dig.l1 + 4*dig.r1 + 2*dig.r2 + 1*dig.r3;   // set all sensors values in one variable using binary numbers
        
        return line;
        
    }

int linecounter()   // count passed lines
    {
        struct sensors_ dig;
        
        int onblack, onwhite, passed;
        
        if (dig.l3 == 1 && dig.r3 == 1)
            {
                onblack = 1;
            }
        
        if (dig.l3 == 0 && dig.r3 == 0)
            {
                onwhite = 1;
            }
            
        if (onblack == 1 && onwhite == 1)
            {
                passed++;
                onblack = 0;
                onwhite = 0;
            }
            
        return passed;
    }
    
void drive(void)    // main drive function; using binary numbers in if statements for better representation
    {
        if (online() == 0b001100)
            {
                motor_forward(255, 0);    // go forward
                return;
            }
            
        if (online() == 0b011000)
            {
                motor_turn(0, 150, 0);  // slight turn left
                return;
            }
            
        if (online() == 0b000110)
            {
                motor_turn(150, 0, 0);  // slight turn right
                return;
            }
                    
        if (online() == 0b110000)
            {
                motor_turn(0, 255, 0);  // hard turn left
                return;
            }
                        
        if (online() == 0b000011)
            {
                motor_turn(255, 0, 0);  // hard turn right
                return;
            }
    }
    
void reveille(void) // music function
    {
        Beep(200, G1);
        Beep(200, C1);
        Beep(100, E1);
        Beep(100, C1);
        Beep(200, G1);
        
        Beep(200, E1);
        Beep(200, C1);
        Beep(100, E1);
        Beep(100, C1);
        Beep(200, G1);
        
        Beep(200, E1);
        Beep(200, C1);
        Beep(100, E1);
        Beep(100, C1);
        Beep(200, G1);
        
        Beep(200, C1);
        Beep(400, E1);
        Beep(200, C1);
    }
    
void darude(void)   // music function
    {
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(150);//
        
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(150);//
        
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(150);//
        
        Beep(50, B2);
        vTaskDelay(50);
        Beep(50, B2);
        vTaskDelay(50);
        Beep(50, B2);
        vTaskDelay(50);
        Beep(50, B2);
        vTaskDelay(50);
        Beep(50, B2);
        vTaskDelay(50);
        Beep(50, B2);
        vTaskDelay(50);
        Beep(50, B2);
        vTaskDelay(150);//
        
        Beep(50, Fs1);
        vTaskDelay(50);
        Beep(50, Fs1);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(150);//
        
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(150);//
        
        Beep(50, Cs2);
        vTaskDelay(50);
        Beep(50, Cs2);
        vTaskDelay(50); //
        
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(150);//
        
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(50);
        Beep(50, Gs2);
        vTaskDelay(150);//
    }
    
#endif

#if 1      // 1 = program enabled; 0 = program disbled;

    // PROJECT // 3:MAZE //
    
int line();
int obstacle();
int linedetector(int onblack, int online);
void coordinate(int *px, int *py);
char drive();
void turn_left();
void turn_right();
void tank_turn_left(uint8, uint32);
void tank_turn_right(uint8, uint32);
void reveille();
void champions();
    
void zmain(void)    // main program
{
    
    IR_Start();
    IR_flush();
    Ultra_Start();
    reflectance_start();
    motor_start();
    
    char compass = drive();
    
    int x, y;                   // get x, y from memory using memory pointer
    coordinate(&x, &y);
    
    TickType_t starttime, finishtime, timepass;
    
    struct sensors_ dig;
    reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance sensor treshold
    reflectance_digital(&dig);
    
    
    while (SW1_Read() == 1);    // wait for user button
    
    while (true)                // drive to fisrt line
        {
            line();
            drive();
            
            if(line() == 0b111111)  // stop when on line
                {
                    motor_forward(0, 0);
                    Beep(100, 100);
                    print_mqtt("Zumo021/ready ", "maze");   // print mqtt message ready
                    break;
                }
        }
        
    while(true) // wait for IR signal
        {
            IR_wait();
            reveille(); //  play music
            
            starttime = xTaskGetTickCount();    // get starttime
            print_mqtt("Zumo021/start ", "%d", starttime);  // print mqtt message starttime
            
            break;
        }
            
    while (true)    // main maze drive
    {
        drive();
        
        if (line() == 0b000000 && y == 13 && x == 0)    // break if maze passed
            {
                motor_stop();
                
                finishtime = xTaskGetTickCount();   // get finishtime
                print_mqtt("Zumo021/stop ", "%d", finishtime);  // print mqtt message finishtime
                
                timepass = finishtime - starttime;  // count passed time
                print_mqtt("Zumo021/time ", "%d", timepass);    // print mqtt message passed time
                
                champions();    // play music
                break;
            }
    }
}

int line()  // get reflectance sensors values
    {
        struct sensors_ dig;

        int line = 32*dig.l3 + 16*dig.l2 + 8*dig.l1 + 4*dig.r1 + 2*dig.r2 + 1*dig.r3;   // set sensor values in one variable using binary number
        
        return line;
    }

int obstacle()  // detect obstacle
    {
        int obstacle;
            
        if (Ultra_GetDistance() < 15)   // set obstacle value if obstacle closer than 15 cm
            {
                obstacle = 1;
            }
            
        else obstacle = 0;
            
        return obstacle;
    }
    
int linedetector(int onblack, int online)   // line detector function
    {
        int onwhite, x, y;  // get x, y from memory using memory pointer
        coordinate(&x, &y);
        
        if (line() == 0b001100 || line() == 0b011000 || line() == 0b000110)
            {
                onwhite = 1;
                onblack = 0;
            }

        if (line() == 0b111111 || line() == 0b011111 || line() == 0b111110 || line() == 0b001111 || line() == 0b111100 || line() == 0b000111 || line() == 0b111000)
            {
                onblack = 1;
            }
    
        if (onwhite == 1 && onblack == 1)
            {
                online = 1;
                onwhite = 0;
                onblack = 0;
                print_mqtt("Zumo021/position ", "%d %d", x, y); // print robot coordinates via mqtt
            }
            
        return online;  // return online value for coordinate counter
    }
    
void coordinate(int *px, int *py)   // coordinate counter; using memory pointer to return multiple values from function
    {
        char compass;     // get compass character from memory using memory pointer
        drive(&compass);
        
        int x, y = -1;
        
        
        
        if (linedetector(1, 1) == 1 && compass == 'N')
            {
                y++;
                linedetector(0, 0);
            }
            
        if (linedetector(1, 1) == 1 && compass == 'W')
            {
                x--;
                linedetector(0, 0);
            }
            
        if (linedetector(1, 1) == 1 && compass == 'E')
            {
                x++;
                linedetector(0, 0);
            }
            
        (*px) = x;  // put x, y to memory
        (*py) = y;
    }
    
char drive()    // main drive function
    {
        char compass = 'N';
        
        int x, y;   // get x, y from memory using memory pointer
        coordinate(&x, &y);
        
        if (line() == 0b001100)
            {
                motor_forward(75, 0);   // drive forward
            }
            
        if (line() == 0b011000)
            {
                motor_turn(0, 100, 0);  // slight left
            }
            
        if (line() == 0b000110)
            {
                motor_turn(100, 0, 0);  // slight right
            }
                    
        if (line() == 0b110000 || line() == 0b010000)
            {
                motor_turn(0, 150, 0);  // hard left
            }
                        
        if (line() == 0b000011 || line() == 0b000010)
            {
                motor_turn(150, 0, 0);  // hard right
            }
                            
        if (line() == 0b111111 && obstacle() == 1 && compass == 'N')    // turn if north and obstackle!
            {
                if (x >= 0) // if robot on right side, take first left
                    {
                        vTaskDelay(250);
                    
                        turn_left();
                        
                        compass = 'W';  // set compass
                                            
                        if (obstacle() == 1)  // check corner; if obstacle > turn away
                            {
                                turn_right();
                                turn_right();
                                compass = 'E';  // set compass
                            }
                    }
                    
                    else if (x < 0) // if robot on left side, take first right
                        {
                            vTaskDelay(250);
                            
                            turn_right();
                                
                            compass = 'E';  // set compass
                                                    
                                if (obstacle() == 1)  // check corner; if obstacle > turn away
                                    {
                                        turn_left();
                                        turn_left();
                                        compass = 'W';  // set compass
                                    }
                        }
            }
                                    
        if (line() == 0b111111 && obstacle() == 0 && compass == 'W' && x != -3)   // turn right if west & no obstacle
            {
                vTaskDelay(250);
                                                        
                turn_right();
                    
                compass = 'N';  // set compass
                                        
                if (obstacle() == 1)  // check corner; if obstacle > turn away
                    {
                        turn_left();
                        compass = 'W';  // set compass
                    }
            }
            
        if ((line() == 0b111111 && obstacle() == 1 && compass == 'W') || (line() == 0b111111 && x == -3 && compass == 'W'))    // turn right if west & obstacle!   ||  return from x -3
            {
                vTaskDelay(250);
                                                        
                turn_right();
                                           
                compass = 'N';  // set compass
                    
                if (obstacle() == 1)  // check corner; if obstacle > turn away
                    {
                        turn_right();
                        compass = 'E';  // set compass
                    }
            }
            
        if (line() == 0b111111 && obstacle() == 0 && compass == 'E' && x != 3)    // turn left if east & no obstacle
            {
                vTaskDelay(250);
                                                        
                turn_left();
                                           
                compass = 'N';  // set compass
                    
                if (obstacle() == 1)  // check corner; if obstacle > turn away
                    {
                        turn_right();
                        compass = 'E';  // set compass
                    }
            }
            
        if ((line() == 0b111111 && obstacle() == 1 && compass == 'E') || (line() == 0b111111 && x == 3 && compass == 'E'))    // turn left if east & obstacle!   ||  return from x +3
            {
                vTaskDelay(250);
                                                        
                turn_left();
                                           
                compass = 'N';  // set compass
                    
                if (obstacle() == 1)  // check corner; if obstacle > turn away
                    {
                        turn_left();
                        compass = 'W';  // set compass
                    }
            }
           
        if ((line() == 0b011111 || line() == 0b111110 || line() == 0b001111 || line() == 0b111100 || line() == 0b000111 || line() == 0b111000) && obstacle() == 1)   // sides with obstacle
            {
                vTaskDelay(50);
                        
                    if (line() == 0b011111 || line() == 0b111110 || line() == 0b001111 || line() == 0b111100 || line() == 0b000111 || line() == 0b111000)   // recheck if robot drive at an angle
                        {
                            if (x < 0)  // turn right if x < 0
                                {
                                    vTaskDelay(200);
                                    turn_right();
                                
                                    compass = 'E';  // set compass
                                }
                                
                                else if (x > 0)  // turn left if x > 0
                                    {
                                        vTaskDelay(200);
                                        turn_left();
                                    
                                        compass = 'W';  // set compass
                                    }
                        }
            }
            
        if ((line() == 0b011111 || line() == 0b111110 || line() == 0b001111 || line() == 0b111100 || line() == 0b000111 || line() == 0b111000) && obstacle() == 0)    // sides and corners without obstacles
            {
                vTaskDelay(50);
                        
                    if (line() == 0b011111 || line() == 0b111110 || line() == 0b001111 || line() == 0b111100 || line() == 0b000111 || line() == 0b111000)   // recheck if robot drive at an angle
                        {
                            
                            if (x < 0)  // turn right if x < 0
                                {
                                    vTaskDelay(200);
                                    turn_right();
                                
                                    compass = 'E';  // set compass
                                }
                                
                                else if (x > 0)  // turn left if x > 0
                                    {
                                        vTaskDelay(200);
                                        turn_left();
                                    
                                        compass = 'W';  // set compass
                                    }
                        }
            }
             
        else motor_forward(75, 0);

        return compass; // return compass value
    }

void turn_left(void)    // turn left function with auto tune -> no dependence on battery level
    {
        tank_turn_left(100, 200);
        motor_forward(0, 0);
        
        while (line() != 0b001100)  // auto tune; stop turning when robot is on line
            {
                line();
                tank_turn_left(100, 0);
            }
    }
    
void turn_right(void)    // turn right function with auto tune -> no dependence on battery level
    {
        tank_turn_right(100, 200);
        motor_forward(0, 0);
        
        while (line() != 0b001100)  // auto tune; stop turning when robot is on line
            {
                line();
                tank_turn_right(100, 0);
            }
    }
 
void tank_turn_left(uint8 speed, uint32 delay)  // custom tank turn function
    {
        MotorDirLeft_Write(1);      // set LeftMotor backward mode
        MotorDirRight_Write(0);     // set RightMotor forward mode
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }

void tank_turn_right(uint8 speed, uint32 delay)  // custom tank turn function
    {
        MotorDirLeft_Write(0);      // set LeftMotor forward mode
        MotorDirRight_Write(1);     // set RightMotor backward mode
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(delay);
    }
    
void reveille(void) // music
    {
        Beep(200, G1);
        Beep(200, C1);
        Beep(100, E1);
        Beep(100, C1);
        Beep(200, G1);
        
        Beep(200, E1);
        Beep(200, C1);
        Beep(100, E1);
        Beep(100, C1);
        Beep(200, G1);
        
        Beep(200, E1);
        Beep(200, C1);
        Beep(100, E1);
        Beep(100, C1);
        Beep(200, G1);
        
        Beep(200, C1);
        Beep(400, E1);
        Beep(200, C1);
    }
    
void champions(void)    // music
    {
        Beep(250, F2);
        Beep(250, F1);
        Beep(250, A1);
        Beep(250, C1);
                        Beep(250, E2);
                        Beep(250, F2);
                        Beep(250, E2);
        Beep(250, A1);
                        Beep(250, C2);
        Beep(250, A1);
        Beep(250, E1);
                        Beep(250, A2);
                        Beep(250, D2);
        Beep(150, D1);
                        Beep(100, C2);
                        Beep(250, A2);
        Beep(250, D1);
        Beep(250, A1);
        Beep(250, F1);
        
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        vTaskDelay(100);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        vTaskDelay(100);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        Beep(10, F2);
        Beep(10, F1);
        Beep(10, As2);
        Beep(10, D2);
        vTaskDelay(100);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        vTaskDelay(100);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        Beep(10, G1);
        Beep(10, C2);
        Beep(10, E2);
        Beep(10, G3);
        vTaskDelay(50);

                        Beep(250, C2);
                        
                        Beep(100, E2);
                        Beep(150, F2);
        Beep(250, F1);
        Beep(250, A1);
        Beep(250, C1);
                        Beep(250, G3);
                        Beep(250, A3);
                        Beep(250, C3);
        Beep(250, A1);
                        Beep(250, A3);
        Beep(250, A1);
        Beep(250, E1);
        Beep(250, G2);
                        Beep(400, D2);
                        Beep(100, E2);
                        Beep(250, D2);
        Beep(250, As1);
        
        Beep(10, A1);
        Beep(10, E1);
        Beep(10, A2);
        Beep(10, C2);
        Beep(10, A1);
        Beep(10, E1);
        Beep(10, A2);
        Beep(10, C2);
        Beep(10, A1);
        Beep(10, E1);
        Beep(10, A2);
        Beep(10, C2);
        Beep(10, A1);
        Beep(10, E1);
        Beep(10, A2);
        vTaskDelay(100);
        Beep(10, G1);
        Beep(10, D1);
        Beep(10, G2);
        Beep(10, As2);
        Beep(10, G1);
        Beep(10, D1);
        Beep(10, G2);
        Beep(10, As2);
        Beep(10, G1);
        Beep(10, D1);
        Beep(10, G2);
        Beep(10, As2);
        Beep(10, G1);
        Beep(10, D1);
        Beep(10, G2);
        vTaskDelay(100);
        Beep(10, Cs1);
        Beep(10, F1);
        Beep(10, A2);
        Beep(10, Cs2);
        Beep(10, Cs1);
        Beep(10, F1);
        Beep(10, A2);
        Beep(10, Cs2);
        Beep(10, Cs1);
        Beep(10, F1);
        Beep(10, A2);
        Beep(10, Cs2);
        Beep(10, Cs1);
        Beep(10, F1);
        Beep(10, A2);
        Beep(10, Cs2);
        
    }
    
#endif

/* [] END OF FILE */
