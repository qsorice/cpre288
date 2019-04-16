/**
 * main.c for final project
 * @author: thine mother
 * @date: today        maybe
 */

#include "lcd.h"
#include "button.h"
#include "UART.h"
#include "ADC_stuff.h"
#include "ping.h"
#include "timer.h"
#include "servo.h"
#include <math.h>

double PINGunder50[20];//collection of objects that fall within 50 cm from the ping sensor
double PINGtemp[100];   //temp array for averaging the values collected by the ping sensor

/**
 * main.c
 * Prints the 20 characters specified by the user
 */
int main(void) {

    uart_init();            //init uart ya silly doofus
    lcd_init();             //initialize LCD
    ping_init();            //init the pingerino
    servo_init();           //init servoo
    init_ADC();             //init abcdefg

    servo_move(0);          //move the servo to 0 degrees
    current_degree = 0;
    char sendstr[81];

    uart_sendStr("Degrees          IR Distance (cm)    Sonar Distance (cm)\r\n");  //print the intro line
    timer_waitMillis(2000); //wait a little bit so the robot has time to move the servo back to 0 degrees

    int start_degrees[10]; //start degree of objects as they are found by the sensors
    int sdi = 0;            //start dergee index
    int end_degrees[10];   //end degrees of objects as they are seen by the sensors
    int edi= 0;             //end degree index

    int pui = 0, pti = 0;   //PING under 50 index, PING temp index
    int start = 1;          //whether the degree is the starting or ending value
    int num_objects = 0;    //actual number of objects seen in 180 degree sweep

    while(current_degree <= 180) { //scan once

            servo_move(current_degree);

            //get an IR sensor reading
            double sensor = 0.0;
            double toAverage[16];   //store 16 samples of data to average later
            int i= 0;
            for(i = 0; i < 16; i++) {
                get_sensor_reading(&sensor);    //grab the data from the sensor, store it in the double
                toAverage[i] = sensor;          //store that double value in the array
            }
            int ADC_result = adc_read(toAverage); //average the IR sensor readings for a more accurate result

            //get a P I N G )  ) )))))   )     ))))))  ))) ))))) sensor reading
            double PING_result = ping_read();

            timer_waitMillis(200); //_____________________________________________________________________________

            if(ADC_result < 50 && PING_result < 50.0) {//if something close enough is seen, take note
                PINGtemp[pti] = PING_result;        //store the PING sensor readings
                pti++;
                if(start) {
                    start_degrees[sdi] = current_degree; //save the start degree value
                    sdi++;
                }
                start = 0; //make sure not to get multiple start degree values
            }
            if(((PING_result > 50 || ADC_result > 50) || (PING_result < 50 && ADC_result < 50 &&current_degree == 180)) && start == 0 ) { //if the sensors have stopped seeing an object, take note
                //IR was too unreliable to take distance readings, is now only used for detecting an object
                int i = 0;
                //average the PING values to get a distance of an object from the PING sensor
                i = 0;
                double totalP = 0;
                while(PINGtemp[i] > 0 && PINGtemp[i] < 50) {
                    totalP += PINGtemp[i];
                    i++;
                }
                totalP = totalP / (double)i;//find the average ping value
                PINGunder50[pui] = totalP;  //store the ping value for distance
                pui++;              //increment ping value index

                end_degrees[edi] = current_degree-5; //end degrees are consistently off by a few degrees :(
                edi++;              //increment end degree index to store the next value
                num_objects++;      //increment number of objects
                pti = 0;            //reset temp array index
                start =1;           //make sure the next object can be detected
            }

            //_________________________________________________________________________________________

            sprintf(sendstr, "%.0lf %20d %20.2lf\r\n", current_degree, ADC_result, PING_result); //prepares the data for printing
            uart_sendStr(sendstr);              //prints one line of data

            current_degree += 2;  //increment degree
     }

    if(num_objects == 0) {
        sprintf(sendstr, "No objects detected within 50cm of bot.\r\n"); //prepares the data for printing
        uart_sendStr(sendstr);              //prints one line of data
    }

    else {
        int k= 0, smallest = 0; //smallest keeps track of the smallest object width
        int angular_width = 0;  //temp for angular width
        double object_sizes[10];
        double smallObj = 100;  //value for an object to be smaller than
        for(k = 0; k < num_objects; k++) {
            angular_width = (end_degrees[k] - start_degrees[k]); //angular width
            double trueDistance = (PINGunder50[k]);              //use ping sensor for distance calculation
            double linear_width = fabs((2.0 * trueDistance) * tanf( ((M_PI/180.0) * ( ((double)angular_width) / 2.0 )))); //linear width eqn
            object_sizes[k] = linear_width; //store linear width of each object detected
            sprintf(sendstr, "object %d at %.0lf degrees is %d degrees wide, is %.0lfcm away, and is %.1lf cm wide\r\n", (k+1), ((start_degrees[k] + end_degrees[k])/2.0), angular_width, trueDistance, linear_width); //prepares the data for printing
            uart_sendStr(sendstr);              //prints one line of data
            sprintf(sendstr, "object %d, starts at %d degrees ends at %d degrees\r\n", (k+1), start_degrees[k], end_degrees[k]); //prepares the data for printing
            uart_sendStr(sendstr);              //prints one line of data
            if(object_sizes[k] < smallObj) {    //if the newest object seen is the smallest recorded,
                 smallest = k;                  //store it
                 smallObj = linear_width;       //and save it as the smallest seen
            }

        }
        k = smallest; //for ezpz copy and paste
        angular_width = end_degrees[k] - start_degrees[k]; //get angular width again for the smallest object
        sprintf(sendstr, "Smallest: #%d\nseen at %.0lf\n%d wide %.0lfcm away\n%.1lf cm wide", (k+1), ((start_degrees[smallest] + end_degrees[smallest])/2.0), angular_width, PINGunder50[k], object_sizes[k]); //prepares the data for printing
        lcd_printf(sendstr);                               //prints one line of data
        servo_move(((start_degrees[k] + end_degrees[k])/2.0)-5); //point at the smallest object
    }

    return 0;
}
