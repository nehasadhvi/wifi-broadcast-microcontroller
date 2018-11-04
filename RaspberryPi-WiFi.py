import RPi.GPIO as GPIO
import board
import time
import busio
import adafruit_tcs34725
from netifaces import interfaces, ifaddresses, AF_INET
from socket import *
from threading import Timer

i2c = busio.I2C(board.SCL, board.SDA);
sensor = adafruit_tcs34725.TCS34725(i2c);
values = [0]*5;
indexVal = 0;
average = 0;
total = 0;
d = 0;
a = 0;

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(17,GPIO.OUT)
GPIO.setup(27,GPIO.OUT)
GPIO.output(17,False)
GPIO.output(27,False)

arduinoIP = "192.168.43.54"
arduinoPort = 4210
MYPORT = 3000

s=socket(AF_INET, SOCK_DGRAM)
s.settimeout(0.5)

host = 'localhost';
s.bind(('',MYPORT))

while(True):
            r, g, b = sensor.color_rgb_bytes
            total = total - values[indexVal];
            values[indexVal] = (r+g+b)/3;
            total = total + values[indexVal];
            average = total/5*40;
            
            indexVal = indexVal + 1;
            if indexVal >= 5:
                indexVal = 0;
                
            piSend = bytearray(str(average), 'utf-8');
            s.sendto(piSend, (arduinoIP, arduinoPort))
            
            time.sleep(0.50);
            
            try:
                d = s.recvfrom(1024);
                a = 1;
                print('ESP Reading: {0}, RaspberryPI Reading: {1}'.format(int(d[0]), average))
                if int(d[0]) < average:
                    GPIO.output(17,GPIO.HIGH)
                    GPIO.output(27,GPIO.LOW)
                if int(d[0]) > average:
                    GPIO.output(17,GPIO.LOW)
                    GPIO.output(27,GPIO.HIGH)
            except timeout as err:
                a = 0;
            if(a == 0):
                GPIO.output(17,GPIO.HIGH)
                GPIO.output(27,GPIO.LOW)
                time.sleep(0.25)
                GPIO.output(17,GPIO.LOW)
                GPIO.output(27,GPIO.LOW)
            
                
                
            
            
        
        