/*
 * movement.c
 *
 *  Created on: Feb 5, 2019
 *      Author: qsorice
 */
#include "movement.h"

void move_forward(oi_t *sensor, int centimeters) {

    sensor = oi_alloc();
    int sum = 0;
    oi_setWheels(500,500); //move forward full speed
    while(sum < centimeters) {
        oi_update(sensor);
        sum += sensor -> distance;  // increment distance
        lcd_printf("%d mm", sum);
    }
    oi_setWheels(0,0); //stop
}

void turn_right(oi_t *sensor) {

    int sum = 0;
    oi_setWheels(-500,500); //turn right full speed
    while(sum > -90) {
        oi_update(sensor);
        sum += sensor -> angle;  // increment angle left
    }
    oi_setWheels(0,0); //stop
}

void turn_left(oi_t *sensor) {

    int sum = 0;
    oi_setWheels(500,-500); //turn left full speed
    while(sum > -90) {
        oi_update(sensor);
        sum -= sensor -> angle;  // increment angle left
    }
    oi_setWheels(0,0); //stop
}

void move_backward(oi_t *sensor, int centimeters) {

    int sum = 0;
    oi_setWheels(-500,-500); //move back full speed
    while(sum < centimeters) {
        oi_update(sensor);
        sum -= sensor -> distance; //decrement distance
        lcd_printf("%d mm", sum);
    }
    oi_setWheels(0,0); //stop
}

void move_with_collision(oi_t *sensor, int millimeters) {

    int sum = 0;
    oi_setWheels(500,500); // full speed
    while(sum < millimeters) {
        oi_setWheels(500,500);
        oi_update(sensor);
        sum += sensor -> distance;
        lcd_printf("%d mm", sum);
        if(sensor -> bumpLeft) { // if bump on the left
            move_backward(sensor, 150);
            sum-=150;                   // go back 15 cm
            turn_right(sensor);         //turn right
            move_forward(sensor, 250); // go forward 25 cm
            turn_left(sensor); // turn left
        }
        else if(sensor -> bumpRight) {
            move_backward(sensor, 150); // go back 15 cm
            sum-=150;
            turn_left(sensor);           // turn left
            move_forward(sensor, 250);  // go forward 25 cm
            turn_right(sensor);          //turn right
        }
    }
    oi_setWheels(0,0); //stop
}

