#include <ssd1306.h>
#include <EEPROM.h>


#define SERVO1PIN     1
#define SERVO2PIN     2
#define PWR_ON_PIN    0
#define OLED_SCL_PIN  3
#define OLED_SDA_PIN  4

/* Servo pulse duration 1000us to 2000us */
#define MIN_ANGLE   1200
#define MAX_ANGLE   1800
#define INCREMENT   5

#define STATE_OFF           0
#define STATE_UP_CONTRACT   1
#define STATE_UP_EXPAND     2
#define STATE_DOWN_CONTRACT 3
#define STATE_DOWN_EXPAND   4
#define SCREEN_OFFSET       32
#define IMG_START           40
#define IMG_TRAIL           70

#define CMD_TIMEOUT       (100) 
#define CMD_MAX           (5)
#define CMD_MIN           (1)

uint32_t angle1 = MIN_ANGLE;
uint32_t angle2 = MIN_ANGLE;
uint8_t speed = INCREMENT;    
bool pwr_flag = false;
uint8_t h_state = STATE_OFF;
uint8_t cursorx = 0;
uint8_t yoffset;
uint8_t cmd = 0;
uint32_t cmd_timestamp = 0;


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

void draw_ecg(){
  uint8_t y;
  uint8_t x = cursorx & 127; 
  uint8_t d = (cursorx - IMG_TRAIL) & 127;

  if(x < IMG_START){     
    yoffset = 0;
  }else if(x < (IMG_START+2)){
    yoffset += 2;   
  }else if(x < (IMG_START+3)){
    yoffset += 1;   
  }else if(x < (IMG_START+5)){ 

  }else if(x < (IMG_START+6)){
    yoffset -= 1;     
  }else if(x < (IMG_START+8)){
    yoffset -= 2;  
  }else if(x < (IMG_START+14)){

  }else if(x < (IMG_START+15)){
    yoffset -= 3;  
  }else if(x < (IMG_START+20)){
    yoffset += 6;  
  }else if(x < (IMG_START+26)){
    yoffset -= 6;  
  }else if(x < (IMG_START+27)){
    yoffset += 6;  
  }else if(x < (IMG_START+28)){
    yoffset += 3;  
  }else if(x < (IMG_START+32)){ 

  }else if(x < (IMG_START+34)){
    yoffset += 2;   
  }else if(x < (IMG_START+35)){
    yoffset += 1;   
  }else if(x < (IMG_START+37)){ 

  }else if(x < (IMG_START+38)){
    yoffset -= 1;     
  }else if(x < (IMG_START+40)){
    yoffset -= 2;  
  }

  y = SCREEN_OFFSET + yoffset;
  uint8_t clear_buff[16] = {0};

  ssd1306_drawVLine(x, y, y+3);
  ssd1306_drawBuffer(d, 0, 2, 64, clear_buff);   
  cursorx++; 
}

void draw_screensaver(){
  ssd1306_fillScreen(0x00); 
  int x, y, r, dist;
  r = 16;
  
  for (x = 0; x <= r*2; x++){
    for (y = 0; y <= r * 2; y++){
      dist = sqrt((r-x) * (r-x) + (r-y) * (r-y));
    
      if (dist==r){
        ssd1306_drawVLine(x+64-r, y+SCREEN_OFFSET-r, y+SCREEN_OFFSET-r+2);
      }          
    }   
    
  }
  ssd1306_drawHLine(64-r, SCREEN_OFFSET, 64);
  ssd1306_drawHLine(64-r, SCREEN_OFFSET+1, 64);
  ssd1306_drawHLine(64-r, SCREEN_OFFSET-1, 64);
}

uint8_t cmd_rx(){  
  if(pwr_flag){ 
    if(cmd_timestamp && ((millis() - cmd_timestamp) > CMD_TIMEOUT) ){
      if(cmd > CMD_MAX){
        cmd = CMD_MAX;
      }
      if(cmd < CMD_MIN){
        cmd = CMD_MIN;
      }

      speed = INCREMENT * cmd;     

      EEPROM.begin();
      EEPROM.update(0, cmd);
      EEPROM.end();  

      cmd_timestamp = 0;
      cmd = 0;
    }
  }else{    
    if(cmd_timestamp == 0){
      cmd_timestamp = millis();
    }

    /* Changing speed or shutting down */
    while((digitalRead(PWR_ON_PIN) == LOW) && ((millis() - cmd_timestamp) < CMD_TIMEOUT)){};     
    
    cmd++;

    if((millis() - cmd_timestamp) >= CMD_TIMEOUT){
      cmd_timestamp = 0;
      cmd = 0;      
    } 
  }
}
   
void setup() {
  pinMode(PWR_ON_PIN, INPUT); 
  pinMode(SERVO1PIN, OUTPUT); 
  pinMode(SERVO2PIN, OUTPUT); 
  digitalWrite(SERVO1PIN, LOW);
  digitalWrite(SERVO2PIN, LOW); 

  ssd1306_128x64_i2c_initEx(3,4,0);
  ssd1306_fillScreen(0x00);  
  draw_screensaver();

  EEPROM.begin();
  cmd = EEPROM.read(0);
  EEPROM.end();

  if(cmd > CMD_MAX){
    cmd = CMD_MAX;
  }
  if(cmd < CMD_MIN){
    cmd = CMD_MIN;
  }

  speed = INCREMENT * cmd;  
}

void loop()  {
  pwr_flag = (digitalRead(PWR_ON_PIN) == HIGH);
  cmd_rx();

  if(!pwr_flag){
    if(h_state != STATE_OFF){
      h_state = STATE_OFF;
      draw_screensaver();     
    }
  }else{
    if(h_state == STATE_OFF){
      h_state = STATE_UP_CONTRACT;
      ssd1306_fillScreen(0x00); 
    }else{  
      draw_ecg();    

      if(h_state == STATE_UP_CONTRACT){
        if(angle1 < MAX_ANGLE){
          angle1 += speed;
          servo_write(SERVO1PIN, angle1);     
        }else{
          h_state = STATE_UP_EXPAND;
        }
      }else if(h_state == STATE_UP_EXPAND){
        if(angle1 > MIN_ANGLE){
          angle1 -= speed;
          servo_write(SERVO1PIN, angle1);
        }else{
          h_state = STATE_DOWN_CONTRACT;
        }
      }else if(h_state == STATE_DOWN_CONTRACT){
        if(angle2 < MAX_ANGLE){
          angle2 += speed;
          servo_write(SERVO2PIN, angle2);
        }else{
          h_state = STATE_DOWN_EXPAND;
        }
      }else if(h_state == STATE_DOWN_EXPAND){
        if(angle2 > MIN_ANGLE){
          angle2 -= speed;
          servo_write(SERVO2PIN, angle2);
        }else{
          h_state = STATE_UP_CONTRACT;            
        }
      }
    }
  }
}
