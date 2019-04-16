/*
 * movement.h
 *
 *  Created on: Feb 5, 2019
 *      Author: qsorice
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"

void move_forward(oi_t *sensor, int centimeters);
void turn_right(oi_t *sensor);
void turn_left(oi_t *sensor);
void move_backward(oi_t *sensor, int centimeters);
void move_with_collision(oi_t *sensor, int millimeters);


#endif /* MOVEMENT_H_ */
