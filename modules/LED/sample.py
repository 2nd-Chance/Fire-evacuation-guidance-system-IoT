import os
import time
 
# ds18b20 온도데이터값을 아래 temp_sensor 경로에 저장하기 위한 명령어
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')
 
# 라즈베리파이가 센서데이터를 받는 경로를 설정합니다. (각자 비슷한 경로를 찾아보셔야 합니다)
temp_sensor='/sys/bus/w1/devices/28-0416935fd1ff/w1_slave'
 
# 파일의 내용을 읽어오는 함수
def temp_raw():
    f = open(temp_sensor,'r')
    lines = f.readlines()
    f.close()
    return lines
 
# 읽어온 파일의 구문을 분석해 온도부분만 반환하는 함수
def read_temp():
    lines = temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = temp_raw()
 
 
    temp_output = lines[1].find('t=')
 
    if temp_output != -1:
        temp_string = lines[1].strip()[temp_output+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0/5.0 + 32.0
        return temp_c, temp_f
 
 
while True:
    print(read_temp())
    time.sleep(1)

[출처] [RaspPI] 라즈베리파이3 활용 예제코드입니다(1) - ds18b20 온도센서 + ThinkSpeak|작성자 tigerk0430
