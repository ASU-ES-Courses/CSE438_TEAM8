#!/bin/bash

declare -a GPIO_PIN=(11 12 13 14 6 0 1 38 40 4 10 5 15 7);
declare -a LS_PIN=(32 28 34 16 36 18 20 -1 -1 22 26 24 42 30);
declare -a MUX_PIN=(-1 45 77 76 -1 66 68 -1 -1 70 74 44 -1 46);


echo -e "Enter Integer input values in order [ percentage of PWM, R_IO, G_IO, B_IO ]"
read pwm
read R_IO
read G_IO
read B_IO

cycle_duration=20   #Value in milliseconds
step_duration=5/10   #Value in seconds
#k=$step_duration*/$cycle_duration/1000
#pwm_on=$cycle_duration*1000*$pwm/100 
#pwm_off=$cycle_duration*1000-$pwm_on
pwm_on=200*$pwm
pwm_off=20000-$pwm_on

ledR=${GPIO_PIN[$R_IO]}
l_shifR=${LS_PIN[$R_IO]}
muxR=${MUX_PIN[$R_IO]}

ledG=${GPIO_PIN[$G_IO]}
l_shifG=${LS_PIN[$G_IO]}
muxG=${MUX_PIN[$G_IO]}

ledB=${GPIO_PIN[$B_IO]}
l_shifB=${LS_PIN[$B_IO]}
muxB=${MUX_PIN[$B_IO]}

#------------------------LED-------------------------
echo -n $ledR >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$ledR/direction

echo -n $ledG >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$ledG/direction

echo -n $ledB >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$ledB/direction

#------------------LEVEL SHIFTER----------------------
if [ $l_shifR -gt 0 ]
then
echo -n $l_shifR >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$l_shifR/direction
echo -n "0" > /sys/class/gpio/gpio$l_shifR/value
fi

if [ $l_shifG -gt 0 ]
then
echo -n $l_shifG >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$l_shifG/direction
echo -n "0" > /sys/class/gpio/gpio$l_shifG/value
fi

if [ $l_shifB -gt 0 ]
then
echo -n $l_shifB >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$l_shifB/direction
echo -n "0" > /sys/class/gpio/gpio$l_shifB/value
fi

#--------------------MUX------------------------
if [ $muxR -gt 0 ]
then
echo -n $muxR >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$muxR/direction
echo -n "0" > /sys/class/gpio/gpio$muxR/value
fi

if [ $muxG -gt 0 ]
then
echo -n $muxG >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$muxG/direction
echo -n "0" > /sys/class/gpio/gpio$muxG/value
fi

if [ $muxB -gt 0 ]
then
echo -n $muxB >> /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$muxB/direction
echo -n "0" > /sys/class/gpio/gpio$muxB/value
fi

#-------------------BLINKING------------------------
for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledR/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledR/value
	usleep $pwm_off
done

for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledG/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledG/value
	usleep $pwm_off
done

for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_off
done

for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledR/value
	echo -n "1" > /sys/class/gpio/gpio$ledG/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledR/value
	echo -n "0" > /sys/class/gpio/gpio$ledG/value
	usleep $pwm_off
done

for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledR/value
	echo -n "1" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledR/value
	echo -n "0" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_off
done

for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledG/value
	echo -n "1" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledG/value
	echo -n "0" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_off
done

for i in {1..25}
do
	echo -n "1" > /sys/class/gpio/gpio$ledR/value
	echo -n "1" > /sys/class/gpio/gpio$ledG/value
	echo -n "1" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_on
	echo -n "0" > /sys/class/gpio/gpio$ledR/value
	echo -n "0" > /sys/class/gpio/gpio$ledG/value
	echo -n "0" > /sys/class/gpio/gpio$ledB/value
	usleep $pwm_off
done


#----------------UNEXPORT---------------------
echo -n $ledR >> /sys/class/gpio/unexport
if [ $l_shifR -gt 0 ]
then
echo -n $l_shifR >> /sys/class/gpio/unexport
fi
if [ $muxR -gt 0 ]
then
echo -n $muxR >> /sys/class/gpio/unexport
fi

echo -n $ledG >> /sys/class/gpio/unexport
if [ $l_shifG -gt 0 ]
then
echo -n $l_shifG >> /sys/class/gpio/unexport
fi
if [ $muxG -gt 0 ]
then
echo -n $muxG >> /sys/class/gpio/unexport
fi

echo -n $ledB >> /sys/class/gpio/unexport
if [ $l_shifB -gt 0 ]
then
echo -n $l_shifB >> /sys/class/gpio/unexport
fi
if [ $muxB -gt 0 ]
then
echo -n $muxB >> /sys/class/gpio/unexport
fi


exit

