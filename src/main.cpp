/*
 * Simple program to measure execution time of FRDM-K64F and the 
 * application board
 *
 * DK - 28-11-2018
 */


#include <stdbool.h>
#include <stdint.h>
#include <mbed.h>
#include "C12832.h"
#include "MMA7660.h"
#include "LM75B.h"
#include <stdio.h>
#include "counter.h"

enum controlData {N_ITERATIONS = 2500};

//static DigitalOut led_app_red(D5);
//AnalogIn pot1(A0); // Pot 1 - Left
//AnalogIn pot2(A1); // Pot 2 - Right
//float pot1Val;
//float pot2Val;
//DigitalIn joystick[5] = {D4, A2, A3, A4, A5};
//char joystickVal;
//MMA7660 accel(D14, D15);
//float accelVal[3];
//LM75B temp(D14, D15);
//float tempVal;
static DigitalOut red(LED_RED);
static DigitalIn sw3(PTA4);
C12832 lcd(D11, D13, D12, D7, D10);
Serial pc(USBTX, USBRX);
uint32_t timeElapsed;

void displayStats(uint32_t, uint32_t, uint32_t, uint64_t);

int main(void) {
  uint32_t timeElapsed = 0;
  uint32_t minTime = 0xFFFFFFFF;
  uint32_t meanTime = 0;
  uint32_t maxTime = 0;
  uint64_t totalTime = 0;

  counterInit();
  while (true) {
    /* Turn off the red LED */
    red = 1;

    /* Measure the time taken to start and stop the counter
     * when there is no other code to execute
     */ 
    counterStart();
    timeElapsed = counterStop();
    pc.printf("\nNOP  : %020lu\n", timeElapsed);

    /* Reset all statistics and display the values to
     * ensure that they have been reset
     */
    minTime = 0xFFFFFFFF;
    meanTime = 0;
    maxTime = 0;
    totalTime = 0;
    displayStats(minTime, meanTime, maxTime, totalTime);

    /* Repeatedly measure the execution time of the code in
     * which we are interested - this is the 'Software under test'
     */
    for (int i = N_ITERATIONS; i > 0; i-=1) {
      counterStart();
      /********* Software under test *********************/
      lcd.locate(2, 8);
      lcd.printf("Hello World!");
      /********* End Software under test ****************/
      timeElapsed = counterStop();
      if (timeElapsed < minTime) {
        minTime = timeElapsed;
      }
      if (timeElapsed > maxTime) {
        maxTime = timeElapsed;
      }
      totalTime += timeElapsed;
    }

    /* Display the statistics that have been captured */
    meanTime = totalTime / N_ITERATIONS;
    displayStats(minTime, meanTime, maxTime, totalTime);

    /* Toggle the red LED while waiting for user input */
    while (sw3 == 1) {
      red = 1 - red;
      wait(0.5);
    }
  }
}

void displayStats(uint32_t minTime,
                  uint32_t meanTime,
                  uint32_t maxTime,
                  uint64_t totalTime) {
  pc.printf("Min  : %020lu\n", minTime);
  pc.printf("Mean : %020lu\n", meanTime);
  pc.printf("Max  : %020lu\n", maxTime);
  pc.printf("Tot  : %020llu\n", totalTime);
}

