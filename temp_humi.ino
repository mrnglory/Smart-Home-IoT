#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>
#include<Servo.h>

LiquidCrystal_I2C lcd(0x3F,16,2);      //LCD 설정
Servo servo;        // servo객체 생성
int angle = 0;      // 초기 각도 = 0

int Tmotor = 3;        //Temp모터는 3번핀
int Hmotor = 5;       // Humi모터는 5번핀

int led_air = 8;      // 공기청정기 led는 8번
int led_Tmotor = 9;   // Tmotor(에어컨)led는 9번
int led_heater = 12;  // heater led는 12번

int sw_servo = A1;     // servo switch는 7번핀

int pin=2; 
DHT11 dht11(pin);   // dht는 2번핀 

int err;
float Temp, Humi;   //온,습도
float index;        //불쾌지수
 
void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.display();         // lcd 리셋 및 불을 킴
  servo.attach(4);       // servo는 4번
  servo.write(0);        // servo 초기값 0
  Serial.begin(9600);   
  
  pinMode(Tmotor,OUTPUT);     // Tmotor 3번핀 output
  pinMode(Hmotor,OUTPUT);     // Hmotor 5번핀 output

  
  pinMode(led_air,OUTPUT);   // led_air 8번핀 output
  pinMode(led_Tmotor,OUTPUT); // led_Tmotor 9번핀 output
  pinMode(led_heater,OUTPUT); // led_heater 12번핀 output    
  
}

void servofunc() {                            // 기본적인 자동 제어
    
//servo motor 조작 
    char in_data;             
    
    int swVal = analogRead(sw_servo);          // sw_servo를 analog input으로 swVal에 저장
    
    if(swVal > 950)                           // switch HIGH 이면
    {
    if(Serial.available())                     //시리얼 모니터에 데이터가 입력되면
    {
    in_data = Serial.read();                   //시리얼모니터로 입력된 데이터 in_data로 저장
    if(in_data == '1' || in_data == '2')        //입력된 데이터가 1,2이라면
    {
      angle = 90;                                   //각도를 90도 증가시킨다.
      digitalWrite(led_air,LOW);                    //led_air LOW -> 미세먼지 농도가 좋으면 창문을 열고 공기청정기를 끈다.
   
    }
    else if(in_data!='1' && in_data!='2' && in_data!='\n')           //그외의 데이터가 입력되면
    {
      angle = 0;                                          //각도를 0으로 초기화
      digitalWrite(led_air,HIGH);                        // led_air high -> 미세먼지 농도가 안좋으면 창문을 닫고 공기청정기를 킨다.
    }  
    servo.write(angle);                                  // angle 값의 각도로 회전
    }
    }
    
    else if(swVal <= 950)                              // switch LOW 이면
    {
    
      angle = 0;                                      // 각도를 0으로 초기화
      digitalWrite(led_air,LOW);                      // led_air low -> 스위치를 끄면 공기청정기를 끄고 창문을 닫는다.
     
      servo.write(angle);                            // angle 값의 각도로 회전.
    }  
 

// 온도에 따른 motor, led 조작
   if (Temp > 30 & Temp <= 32)                
      {
      analogWrite(Tmotor,200);
      digitalWrite(led_Tmotor,HIGH);
      digitalWrite(led_heater,LOW);               
      }
    else if (Temp > 32)
      { 
      analogWrite(Tmotor,255);
      digitalWrite(led_Tmotor,HIGH);
      digitalWrite(led_heater,LOW);
      }
    else if (Temp < 10)
    {
      digitalWrite(led_Tmotor,LOW);
      digitalWrite(led_heater,HIGH);
    }
    
    else
      {
      analogWrite(Tmotor,0);
      digitalWrite(led_Tmotor,LOW);
      digitalWrite(led_heater,LOW);
      }


// 습도에 따른 motor, led 조작      
   if (Humi >60 & Humi <= 80)              
   {
    analogWrite(Hmotor,200);
   }
   else if (Humi > 80)
   {
    analogWrite(Hmotor,255);
   }
   else
   {
    analogWrite(Hmotor,0);
   }
  
 }


void loop(){

// lcd에 온도, 습도 표시 
  if((err=dht11.read(Humi, Temp))==0)   
  { 

    lcd.setCursor(0,0);
    lcd.print("Temp : ");
    lcd.print(Temp);
    lcd.print(" C  ");

    lcd.setCursor(0,1);
    lcd.print("Humi : ");
    lcd.print(Humi);
    lcd.print(" %  ");
   
  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print("ERROR NO.: dht11");
    lcd.setCursor(0,1);
    lcd.print("                ");
  }


   int i;
   for(i=0; i < 30; i++)           // delay 3초 
   {
    servofunc();
    delay(100);
   }

// lcd에 불쾌지수 표시
    index = 1.8*Temp-0.55*(1-Humi/100)*(1.8*Temp-26)+32;       // 불쾌지수 공식
    
    lcd.setCursor(0,0);
    lcd.print("Discomfort index");
    
    lcd.setCursor(0,1);
    lcd.print(": ");
    lcd.print(index);
    if(index < 68)
    {
      lcd.setCursor(7,1);
      lcd.print("   Good  ");
    }
    else if(index > 68 && index < 75)
    {
      lcd.setCursor(7,1);
      lcd.print("  Normal ");
    }
    else if(index >= 75 && index <80)
    {
      lcd.setCursor(7,1);
      lcd.print("   Bad   ");
    }
    else
    {
      lcd.setCursor(7,1);
      lcd.print(" Too Bad ");
    }

    
   for(i=0; i < 30; i++)            // delay 3초
   {
    servofunc();
    delay(100);
   }

 lcd.clear();
}
