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

#if 0   // 1 = program enabled; 0 = program disbled;

    // PROJECT // 1:RING //

int line, x_or_x;
int x, y, angle, hittime, distance;
TickType_t starttime, finishtime, timepass;
void sensors();
void reflectance();
void linefollow();
void ring();
void escape();
void accangles();
void tank_turn_left(uint8, uint32);
void tank_turn_right(uint8, uint32);
    
void zmain(void)    // main program
{
    IR_Start();
    LSM303D_Start();
    reflectance_start();
    IR_flush();
    Ultra_Start();
    motor_start();
    
    while(SW1_Read() == 1);             // wait for user button
    
    while(x_or_x != 1)             // drive to first line
        {
            sensors();
            
            linefollow();
        }
        
    motor_forward(0, 0);
    print_mqtt("Zumo021/ready ", "zumo");   // send mqtt message ready
    
    while(true)             // wait for IR signal
        {
            IR_wait();
            
            starttime = xTaskGetTickCount();    // set starttime
            print_mqtt("Zumo021/start ", "%d", starttime);  //print mqtt message starttime
            
            motor_forward(255, 100);
            
            break;
        }

    while(SW1_Read() == 1)             // drive in ring; exit while user button is pressed
        {
            sensors();
            
            ring();
            
            escape();
        }
    
    motor_stop();
    
    finishtime = xTaskGetTickCount();   // set finishtime
    print_mqtt("Zumo021/stop ", "%d", finishtime);  // print mqtt message finishtime
    
    timepass = finishtime - starttime;  // count timepass
    print_mqtt("Zumo021/time ", "%d", timepass);    // pint mqtt message timepass
    
}

void sensors(void)    // get values from reflectance sensors
    {
        struct sensors_ dig;
        
        reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance treshold

        reflectance_digital(&dig);
        
        line = 32*dig.l3 + 16*dig.l2 + 8*dig.l1 + 4*dig.r1 + 2*dig.r2 + 1*dig.r3;   // set all sensor values in one variable using binary numbers
        
        if (dig.l3 == 1 || dig.r3==1)    // set exception value if only l3 or r3 is active
            {
                x_or_x = 1;
            }
            
        else x_or_x = 0;
            
        distance = Ultra_GetDistance(); // set distance value
    }
  
void linefollow(void)   // linefollow function; using binary numbers in if-statements for better representation
    {
        if (line == 0b001100)
            {
                motor_forward(100, 0);
            }
            
        else if (line == 0b011000)
            {
                motor_turn(50, 200, 0);
            }
            
        else if (line == 0b000110)
            {
                motor_turn(200, 50, 0);
            }
                
        else motor_forward(100, 0);
    }
    
void ring(void) // ring function for competition; drive forward and take random turns
    {
        int r = rand() % 2; // get random number for turns
        
        for (int i = 0; i <500; i++)    // drive forward
            {
                sensors();
                
                if (x_or_x != 1 && distance > 30)
                    {
                        motor_forward (200, 1);
                    }
                    
                else break;
            }
            
        if (r == 0)                                      // turn left
                {
                    for (int i = 0; i <200; i++)
                        {
                            sensors();
                            
                            if (x_or_x != 1 && distance > 30)
                                {
                                    motor_turn(0, 255, 1);
                                }
                                
                            else break;
                        }
                }
            
        if (r == 1)                                      // turn right
            {
                for (int i = 0; i <200; i++)
                    {
                        sensors();
                        
                        if (x_or_x != 1 && distance > 30)
                            {
                                motor_turn(255, 0, 1);
                            }
                            
                        else break;
                    }
            }
        
        if (x_or_x != 1 && distance < 30) //attack if distance to enemy is < 30 cm
            {
                motor_forward(255, 0);
            }
    }
    
void escape(void)   // escape function if robot is on edge
    {
        int r = rand() % 2; // get random number for turns
        
        if (x_or_x == 1)  // if edge detected -> drive backward
            {
                motor_backward(255, 200);
            }
            
        if (x_or_x == 1 && r == 0)    // take random turns
                {
                    tank_turn_left(255, 200);
                    motor_forward(0, 0);
                }
                
                else if (x_or_x == 1 && r == 1)
                {
                    tank_turn_right(255, 200);
                    motor_forward(0, 0);
                }
    }
    
void accangles(void)    // get accelerometer values // not used in competition due to incompleteness
    {
        struct accData_ data;
        
        LSM303D_Read_Acc(&data);
        hittime = xTaskGetTickCount();
            
        x = data.accX;
        y = data.accY;

        if (x < -20000 || y > 20000 )
            {
                if (x < -20000 && y > 20000 )
                    {
                        angle = 45;
                    }
                
                else if (x < -20000)
                    {
                        angle = 0;
                    }
                    
                else if (y > 20000)
                    {
                        angle = 90;
                    }
                    
                print_mqtt("Zumo021/hit ", "%d %d", hittime, angle);
            }
            
            else if (y < -20000 || x > 20000)
                {
                    if (y < -20000 && x > 20000)
                        {
                            angle = 225;
                        }
                        
                    else if (y < -20000)
                        {
                            angle = 270;
                        }
                        
                    else if (x > 20000)
                        {
                            angle = 180;
                        }
                        
                    print_mqtt("Zumo021/hit ", "%d %d", hittime, angle);
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


#if 0   // 1 = program enabled; 0 = program disbled;

    // PROJECT // 2:LINE FOLLOWER //

int line;
int onblack, onwhite, passed;
TickType_t starttime, finishtime, timepass;
void sensors();
void linecounter();
void drive();
void reveille();
void darude();
    
void zmain(void)    // main program
{
    IR_Start();
    IR_flush();
    reflectance_start();
    motor_start();

    while (SW1_Read() == 1);    // wait for user button
    
    while (true)                // drive to fisrt line
        {
            sensors();
            drive();
            
            if (line == 0b111111)   // stop when on line
                {
                    motor_forward(0, 0);
                    Beep(100, 100);
                    passed ++;
                    print_mqtt("Zumo021/ready ", "line");   // print mqtt message ready
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
    
    while (true)    // drive
        {
            sensors();
            linecounter();
            drive();
            
            if (passed == 2 && onblack == 1) // stop on second line
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
    
void sensors(void)    // get values from reflectance sensors
    {
        struct sensors_ dig;
        
        reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance treshold

        reflectance_digital(&dig);
        
        line = 32*dig.l3 + 16*dig.l2 + 8*dig.l1 + 4*dig.r1 + 2*dig.r2 + 1*dig.r3;   // set all sensors values in one variable using binary numbers
        
    }

void linecounter(void)  // linecounter function
    {
        struct sensors_ dig;
        
        reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance treshold
        
        reflectance_digital(&dig);
        
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
                passed++;   // count passed lines
                onblack = 0;
            }
            
        else onwhite = 0;
    }
    
void drive(void)    // drive from start to finish; using binary numbers in if-statements for better representation
    {
        if (line == 0b001100)
            {
                motor_turn(255, 0);    // go forward
                return;
            }
            
        if (line == 0b011000)
            {
                motor_turn(0, 150, 0);  // slight left
                return;
            }
            
        if (line == 0b000110)
            {
                motor_turn(150, 0, 0);  // slight right
                return;
            }
                    
        if (line == 0b110000)
            {
                motor_turn(0, 255, 0);  // hard left
                return;
            }
                        
        if (line == 0b000011)
            {
                motor_turn(255, 0, 0);  // hard right
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


#if 0      // 1 = program enabled; 0 = program disbled;

    // PROJECT // 3:MAZE //
   
int line;
int x, y = -1, distance, obstacle;
int onblack, onwhite, online;
TickType_t starttime, finishtime, timepass;
char compass;
void sensors();
void linedetector();
void coordinatecounter();
void drive();
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
    
    while (SW1_Read() == 1);    // wait for user button
    
    compass = 'N';
    
    while (true)                // drive to fisrt line
        {
            sensors();
            drive();
            
            if(line == 0b111111)    // stop when on line
                {
                    motor_forward(0, 0);
                    Beep(100, 100);
                    print_mqtt("Zumo021/ready ", "maze");   // send mqtt message ready
                    break;
                }
        }
        
    while(true)                 // wait for IR signal
        {
            IR_wait();
            reveille();
            
            starttime = xTaskGetTickCount();    // get starttime
            print_mqtt("Zumo021/start ", "%d", starttime);  // print mqtt message starttime
            
            break;
        }
            
    while (true)                // main maze drive
    {
        sensors();
        
        linedetector();
        
        coordinatecounter();
        
        drive();
        
        if (line == 0b000000 && y == 13 && x == 0) // stop when maze passed
            {
                motor_stop();
                
                finishtime = xTaskGetTickCount();   // get finish time
                print_mqtt("Zumo021/stop ", "%d", finishtime);  // print mqtt message finishtime
                
                timepass = finishtime - starttime;  // count passed time
                print_mqtt("Zumo021/time ", "%d", timepass);    // print mqtt message timepassed
                
                champions();    // play Queen :=)
                break;
            }
    }
}

void sensors(void)  // sensors function; get values from several sensors
    {
        struct sensors_ dig;
        
        reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);    // set reflectance treshold

        reflectance_digital(&dig);

        line = 32*dig.l3 + 16*dig.l2 + 8*dig.l1 + 4*dig.r1 + 2*dig.r2 + 1*dig.r3;   // set sensor values in one variable using binary numbers
      
        distance = Ultra_GetDistance(); // set distance value
                
        if (distance < 15)  // set obstacle on if distance to obstacle < 15 cm
            {
                obstacle = 1;
            }
            
        else obstacle = 0;
            
    }
    
void linedetector(void)   // line detector function; using binary numbers in if-statements for better representation
    {
        
        if (line == 0b001100 || line == 0b011000 || line == 0b000110)
            {
                onwhite = 1;
                onblack = 0;
            }

        if (line == 0b111111 || line == 0b011111 || line == 0b111110 || line == 0b001111 || line == 0b111100 || line == 0b000111 || line == 0b111000)
            {
                onblack = 1;
            }
    
        if (onwhite == 1 && onblack == 1)   // set online = 1 when robot staying on line
            {
                online = 1;
                onwhite = 0;
                onblack = 0;
                print_mqtt("Zumo021/position ", "%d %d", x, y); // print coordinates via mqtt
            }
    }
    
void coordinatecounter(void)    // count x & y coordinates depending on compass value
    {
        if (online == 1 && compass == 'N')
            {
                y++;
                online = 0;
                onblack = 0;
            }
            
        if (online == 1 && compass == 'W')
            {
                x--;
                online = 0;
            }
            
        if (online == 1 && compass == 'E')
            {
                x++;
                online = 0;
            }
    }
    
void drive(void)   // main drive function; using binary numbers in if-statements for better representation
    {
        
        if (line == 0b001100)
            {
                motor_forward(75, 0);   // go forward
                return;
            }
            
        if (line == 0b011000)
            {
                motor_turn(0, 100, 0);  // slight left
                return;
            }
            
        if (line == 0b000110)
            {
                motor_turn(100, 0, 0);  // slight right
                return;
            }
                    
        if (line == 0b110000 || line == 0b010000)
            {
                motor_turn(0, 150, 0);  // hard left
                return;
            }
                        
        if (line == 0b000011 || line == 0b000010)
            {
                motor_turn(150, 0, 0);  // hard rigth
                return;
            }
                            
        if (line == 0b111111 && obstacle == 1 && compass == 'N')    // turn if north and obstackle!
            {
                if (x >= 0) // if robot on right side, take first left
                    {
                        vTaskDelay(250);
                    
                        turn_left();
                        
                        compass = 'W';  // set compass
                                            
                        if (obstacle == 1)  // check corner; if obstacle > turn away
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
                                                    
                                if (obstacle == 1)  // check corner; if obstacle > turn away
                                    {
                                        turn_left();
                                        turn_left();
                                        compass = 'W';  // set compass
                                    }
                        }
                
                return;
            }
                                    
        if (line == 0b111111 && obstacle == 0 && compass == 'W' && x != -3)   // turn right if west & no obstacle
            {
                vTaskDelay(250);
                                                        
                turn_right();
                    
                compass = 'N';  // set compass
                                        
                if (obstacle == 1)  // check corner; if obstacle > turn away
                    {
                        turn_left();
                        compass = 'W';  // set compass
                    }
                    
                return;
            }
            
        if ((line == 0b111111 && obstacle == 1 && compass == 'W') || (line == 0b111111 && x == -3 && compass == 'W'))    // turn right if west & obstacle!   ||  return from x -3
            {
                vTaskDelay(250);
                                                        
                turn_right();
                                           
                compass = 'N';  // set compass
                    
                if (obstacle == 1)  // check corner; if obstacle > turn away
                    {
                        turn_right();
                        compass = 'E';  // set compass
                    }
                    
                    return;
            }
            
        if (line == 0b111111 && obstacle == 0 && compass == 'E' && x != 3)    // turn left if east & no obstacle
            {
                vTaskDelay(250);
                                                        
                turn_left();
                                           
                compass = 'N';  // set compass
                    
                if (obstacle == 1)  // check corner; if obstacle > turn away
                    {
                        turn_right();
                        compass = 'E';  // set compass
                    }
                    
                return;
            }
            
        if ((line == 0b111111 && obstacle == 1 && compass == 'E') || (line == 0b111111 && x == 3 && compass == 'E'))    // turn left if east & obstacle!   ||  return from x +3
            {
                vTaskDelay(250);
                                                        
                turn_left();
                                           
                compass = 'N';  // set compass
                    
                if (obstacle == 1)  // check corner; if obstacle > turn away
                    {
                        turn_left();
                        compass = 'W';  // set compass
                    }
                    
                return;
            }
           
        if ((line == 0b011111 || line == 0b111110 || line == 0b001111 || line == 0b111100 || line == 0b000111 || line == 0b111000) && obstacle == 1)   // sides with obstacle
            {
                vTaskDelay(50);
                        
                    sensors();
                    coordinatecounter();
                    linedetector();
                    
                    if (line == 0b011111 || line == 0b111110 || line == 0b001111 || line == 0b111100 || line == 0b000111 || line == 0b111000)   // recheck if robot drive at an angle
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
            return;
            
            }
            
        if ((line == 0b011111 || line == 0b111110 || line == 0b001111 || line == 0b111100 || line == 0b000111 || line == 0b111000) && obstacle == 0)    // sides and corners without obstacles
            {
                vTaskDelay(50);
                        
                    sensors();
                    coordinatecounter();
                    linedetector();
                    
                    if (line == 0b011111 || line == 0b111110 || line == 0b001111 || line == 0b111100 || line == 0b000111 || line == 0b111000)   // recheck if robot drive at an angle
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
            return;
            }
             
        else motor_forward(75, 0);
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
