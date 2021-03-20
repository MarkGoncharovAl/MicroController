# Distance measurer on Atmega 8535

### Mark Goncharov, Ruslan Zaripov, Batarin Egor

### 20 march 2021 г.

```
Annotation
In this work we present measuring devices, based on AVR Atmega8535 microcontroller. It uses 2 HC­SR
ultrasonic modules, which allow to calculate and compare distance.
```
## 1 Introduction

```
It’s an educational project referred to Microcontoller courses in MIPT at 2nd grade.
Research supervisor: candidate of technical sciences Donov Gennady Innokentievich.
```
## 2 Project scheme

```
The scheme of the Measuring device (MD) is a follows:
```
```
![MD Scheme](https://github.com/MarkGoncharovAl/MicroController/tree/master/images/1.png)
```
```
It consist of 3 main parts:
```
1. Atmega 8535 ­ Microcontroller unit ­ will be called MC


2. Two HC­SR04 modules ­ ultrasonic waves detector ­ UWD
3. USB­ASP ­ flash driver ­ allows to program MU ­ ASP
4. USB TO TTL ­ flash driver ­ was used to transmit information from MC to PC ­ TTL

## 3 HC­SR04 description

The transmitter emits a 8 bursts of an directional 40 KHzultrasonic wave when triggered and starts a timer.
Ultrasonicpulsestraveloutwarduntiltheyencounteranobject,Theobjectcausesthethewavetobereflectedback
towards the unit. The ultrasonic receiver would detect the reflected wave and stop the stop timer. The velocity of
the ultrasonic burst is 340m/sec. in air. Based on the number of counts by the timer, the distance can be calculated
between the object and transmitter.
The formula is expressed as:D=C·Twhich is know as the time/rate/distance measurement formula where
Disthemeasureddistance,andRisthepropagationvelocity(Rate)inair(speedofsound)andTrepresentstime.
In this applicationTis devided by 2 asTis double the time value from transmitter to object back to receiver.

```
![HC­SR04 views](https://github.com/MarkGoncharovAl/MicroController/tree/master/images/2.png)
```
```
Pin Symbol Pin Function Description
1 VCC 5V power supply
2 Trig Trigger Input pin
3 Echo Receiver Output pin
4 GND Power ground
```
## 4 Using of HC­SR

Our aim was to create enough pulse in the (UWD).
Then we had to measure time using inner TIMER 1 (PD6).
In our experiment we connected 2 devices in order to compare distances. Conflict of Echo was solved using
diodes (or resistances). Therefore we used 2 devices at the same time.
We translate information from MC to PC using TTL. Our Information transfer protocol was UART.

## 5 How to use?

1. Get main parts, such as AtMega 8535, USBASP, USB TO TTL, indicators HC­SR04.
2. **Assemble the installation** according to scheme above.


```
![](https://github.com/MarkGoncharovAl/MicroController/tree/master/images/3.png)
![Our layout board](https://github.com/MarkGoncharovAl/MicroController/tree/master/images/4.png)
```
3. **Clone** this repository: https://github.com/MarkGoncharovAl/MicroController.com
4. Open the terminal inside downloaded folder
5. Install **AVR­instruments**.
6. **Compile** file: ”make”
7. **Program your MC** : ”make flash”
8. Check results in mm using graphical serial terminal. For example, we used cutecom: ”sudo apt install
    cutecom”.



