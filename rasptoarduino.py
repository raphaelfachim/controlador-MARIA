import pygame
import serial
import time
import sys

port0 = 0
con0 = False

port1 = 0
con1 = False

ser0_w = ''
ser1_w = ''

final_input = []
keyb_input = []
str1 = ''

while not con0:
    try:
        ser0 = serial.Serial("/dev/ttyUSB{}".format(port0),9600)
    except:
        print('Não foi possível conectar em /dev/ttyUSB{}'.format(port0))
        port0 += 1
    else:
        print('Porta 0 conectada em /dev/ttyUSB{}'.format(port0))
        con0 = True

while (not con1) and (port1 < 5):
    if port1 != port0:
        try:
            ser1 = serial.Serial("/dev/ttyUSB{}".format(port1), 9600)
        except:
            print('Não foi possível conectar em /dev/ttyUSB{}'.format(port1))
            port1 += 1
        else:
            print('Porta 1 conectada em /dev/ttyUSB{}'.format(port1))
            con1 = True
    else:
        port1 += 1
            
        
ser0.flush()
ser1.flush()

def setup():
    w0_id = False
    w1_id = False
    while not w0_id:
        ser0.write(str(9).encode('utf-8'))
        time.sleep(0.2)
        if ser0.in_waiting > 0:
            arduino_response = ser0.read().decode('utf-8')
            if arduino_response == '1':
                w0_id = True
                ser0_w = 'left'
                print('ser0 na roda esquerda')
            elif arduino_response == '2':
                w0_id = True
                ser0_w = 'right'
                print('ser0 na roda direita')
                
    while not w1_id:
        ser1.write(str(9).encode('utf-8'))
        time.sleep(0.2)
        if ser1.in_waiting > 0:
            arduino_response = ser1.read().decode('utf-8')
            if arduino_response == '1':
                w1_id = True
                ser1_w = 'left'
                print('ser1 na roda esquerda')
            elif arduino_response == '2':
                w1_id = True
                ser1_w = 'right'
                print('ser1 na roda direita')
                
    return ser0_w, ser1_w

def init():
    pygame.init()
    win = pygame.display.set_mode((100,100))

def getKey(ser0_w, ser1_w):
    
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN:
           
            key_p = str(pygame.key.name(event.key))
            if key_p == 'w':
                ser0.write(str(1).encode('utf-8'))
                ser1.write(str(1).encode('utf-8'))
          
            elif key_p == 's':
                ser0.write(str(2).encode('utf-8'))
                ser1.write(str(2).encode('utf-8'))
                
            elif key_p == 'a':
                if ser0_w == 'left':
 
                    #ser0.write(str(2).encode('utf-8'))
                    ser1.write(str(1).encode('utf-8'))
                elif ser0_w == 'right':
  
                    ser0.write(str(1).encode('utf-8'))
                    #ser1.write(str(2).encode('utf-8'))
            elif key_p == 'd':
               
                if ser0_w == 'left':
  
                    ser0.write(str(1).encode('utf-8'))
                    #ser1.write(str(2).encode('utf-8'))
                elif ser0_w == 'right':
  
                    #ser0.write(str(2).encode('utf-8'))
                    ser1.write(str(1).encode('utf-8'))
            elif key_p == 'delete':
                sys.exit()
                    
        if event.type == pygame.KEYUP:
            ser0.write(str(3).encode('utf-8'))
            ser1.write(str(3).encode('utf-8'))
            
       
ser0_w, ser1_w = setup()
print('ser0_w = {}'.format(ser0_w))
print('ser1_w = {}'.format(ser1_w))
init()
while True:
    getKey(ser0_w, ser1_w)
    