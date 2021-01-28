import requests
from bs4 import BeautifulSoup
import pandas
import serial
import I2C_LCD_driver
import time

ser = serial.Serial("/dev/ttyACM0" ,9600)

M = '&numOfRows=1&pageNo=1&stationName=마포구&dataTerm=DAILY&ver=1.3'
key = 'baHQwL7PU9aOAl5VEMy8uEvanZj%2F9NkUstZkCki%2FJDskKLQymF%2FRLMEBg2KXwCAKxQ6BkMVE8nFBpMHeilnNMQ%3D%3D'
url = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/getMsrstnAcctoRltmMesureDnsty?serviceKey=' + key + M

response = requests.get(url)
soup = BeautifulSoup(response.text, "html.parser")
ItemList = soup.findAll('item')
for item in ItemList:
    a = item.find('datatime').text
    g = item.find('pm10value').text
    i = item.find('pm25value').text
    s = item.find('pm10grade1h').text
    t = item.find('pm25grade1h').text
    print('측정소: 마포구')
    print('측정시간:' + a)
    print('미세먼지 농도:' + g + '㎍/㎥ ( ' + s + ' )')
    print('초미세먼지 농도:' + i + '㎍/㎥ ( ' + t + ' )')
    print('( 좋음: 1 ),( 보통: 2 ),( 나쁨: 3 ),( 매우나쁨: 4)' )

open_or_close = '0'
if (s <= '2' and t <= '2'):
    open_or_close = '1'
else:
    open_or_close = '3'

ser.write(open_or_close.encode())

if (s == '1'):
    f = "(^_^)"
elif (s == '2'):
    f = "(-_-)"
elif (s == '3'):
    f = "(T_T)"
elif (s == '4'):
    f = "(X_X)"

if (t == '1'):
    u = "(^_^)" 
elif (t == '2'):
    u = "(-_-)"
elif (t == '3'):
    u = "(T_T)"
elif (t == '4'):
    u = "(X_X)"

###############################I2C

mylcd = I2C_LCD_driver.lcd()
mylcd.lcd_display_string("fine dust: {0}".format(f), 1)
mylcd.lcd_display_string("ultrafine: {0}".format(u), 2)