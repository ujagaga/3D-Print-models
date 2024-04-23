// #include "lcdgfx.h"

#define SERVO1PIN   1
#define SERVO2PIN   2
#define PWR_ON_PIN  0

/* Servo pulse duration 1000us to 2000us */
#define MIN_ANGLE   1200
#define MAX_ANGLE   1800
#define INCREMENT   10

#define STATE_OFF           0
#define STATE_UP_CONTRACT   1
#define STATE_UP_EXPAND     2
#define STATE_DOWN_CONTRACT 3
#define STATE_DOWN_EXPAND   4

uint32_t angle1 = MIN_ANGLE;
uint32_t angle2 = MIN_ANGLE;
bool pwr_flag = false;
uint8_t h_state = STATE_OFF;


void servo_delay_short(uint32_t duration_us){ 
  while(duration_us > 500){
    duration_us -= 500;
    delayMicroseconds(500);
  }
  delayMicroseconds(duration_us); 
  
}

void servo_write(int pin, uint32_t duration_us){
  uint32_t pause = 20000-duration_us;
  uint32_t short_pause = pause - 17600;

  digitalWrite(pin, HIGH);
  servo_delay_short(duration_us);
  digitalWrite(pin, LOW);

  delay(18);
  delayMicroseconds(short_pause);

}
   
void setup() {
  // Set up the interrupt that will refresh the servo for us automagically
  OCR0A = 0xAF;            // any number is OK
  pinMode(PWR_ON_PIN, INPUT); 
  pinMode(SERVO1PIN, OUTPUT); 
  pinMode(SERVO2PIN, OUTPUT); 
  digitalWrite(SERVO1PIN, LOW);
  digitalWrite(SERVO2PIN, LOW); 
}

void loop()  {
  pwr_flag = (digitalRead(PWR_ON_PIN) == HIGH);

  if(h_state == STATE_OFF){
    if(pwr_flag){      
      h_state = STATE_UP_CONTRACT;
    }
  }else{                 

    if(h_state == STATE_UP_CONTRACT){
      if(angle1 < MAX_ANGLE){
        angle1 += INCREMENT;
        servo_write(SERVO1PIN, angle1);     
      }else{
        h_state = STATE_UP_EXPAND;
      }
    }else if(h_state == STATE_UP_EXPAND){
      if(angle1 > MIN_ANGLE){
        angle1 -= INCREMENT;
        servo_write(SERVO1PIN, angle1);
      }else{
        h_state = STATE_DOWN_CONTRACT;
      }
    }else if(h_state == STATE_DOWN_CONTRACT){
      if(angle2 < MAX_ANGLE){
        angle2 += INCREMENT;
        servo_write(SERVO2PIN, angle2);
      }else{
        h_state = STATE_DOWN_EXPAND;
      }
    }else if(h_state == STATE_DOWN_EXPAND){
      if(angle2 > MIN_ANGLE){
        angle2 -= INCREMENT;
        servo_write(SERVO2PIN, angle2);
      }else{
        if(pwr_flag){
          h_state = STATE_UP_CONTRACT;
        }else{
          h_state = STATE_OFF;          
        }        
      }
    }
  }
}
