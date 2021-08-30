import serial
import csv
import sys
import keyboard

ser = serial.Serial("COM3", 9600)
data = []
counter = 0

fieldnames = ["tempo", "velocidade"]

with open('data.csv', 'w') as csv_file:
    csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    csv_writer.writeheader()

while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode("utf-8").rstrip()
            print(data)
            counter += 0.5
            with open('data.csv', 'a') as csv_file:
                csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
                info = {
                    "tempo": counter,
                    "velocidade": data,
                }
                csv_writer.writerow(info)
        if keyboard.is_pressed('q'):
            sys.exit()

