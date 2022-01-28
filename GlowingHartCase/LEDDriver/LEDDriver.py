#!/usr/bin/python3
from gpiozero import LED, PWMLED
from time import sleep
import random

LED_W = PWMLED(13, initial_value=0.1)
LED_W.off()

STAR_NUMBERS = [14, 15, 23, 24, 25, 8, 7, 12, 16, 4, 17, 27, 22, 10, 9, 11, 5, 6, 26]
STARS = []
MAX_ID = len(STAR_NUMBERS) - 1


for led_num in STAR_NUMBERS:
    STAR = LED(led_num)
    STARS.append(STAR)

while True:
    for i in range(0, MAX_ID * 2):
        star_id = random.randint(0, MAX_ID)
        STARS[star_id].on()
        sleep(0.2)

    for i in range(0, MAX_ID * 2):
        star_id = random.randint(0, MAX_ID)
        STARS[star_id].off()
        sleep(0.2)

