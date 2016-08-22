const int LED_PIN = 9;
const int IN_1 = A0;
const int PULL_1 = 8;


void switch_led(bool led_on)
{
  if(led_on)
    digitalWrite(LED_PIN, false);
  else
    digitalWrite(LED_PIN, true);
}

uint16_t analog_read_avg()
{
  uint16_t result = 0;
  for(int i = 0; i < 1 ; i++)
    result += analogRead(IN_1);
  return result / 1;
}

void calc_RMS(int value)
{
   static const int rms_buff_len = 64;
//#define RMS_BUFF_LEN
   static uint16_t counter = 0;
   static int value_buff[rms_buff_len]; 
   static int avg_sum = 0;
   avg_sum += value;
   value_buff[counter] = value;
   counter++;
   float avg = (float)avg_sum / rms_buff_len;
   if(counter == rms_buff_len)
   {
     int32_t rms_sum_sq = 0;
     for(uint16_t i = 0; i << rms_buff_len; i++)
      {
         float diff = value_buff[i] - avg;
         rms_sum_sq += diff * diff;        
      }
     float rms = sqrt(rms_sum_sq / rms_buff_len);
     Serial.print("RMS  ");
     Serial.println(rms);
     counter = 0;
     avg_sum = 0;     
   }   
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(PULL_1, OUTPUT);
  digitalWrite(PULL_1, HIGH);
  pinMode(IN_1, INPUT);
  Serial.begin(57600);
}

bool s_even = false;
uint32_t time_last = millis();
int counts = 0;

void loop() {
  digitalWrite(PULL_1, LOW);
  delayMicroseconds(15);
  int dark_val = analog_read_avg();
//  analogWrite(LED_PIN, (255 * (33 - 13)) / 33);
  switch_led(true);  
  // from led rise to proper analog rise 20us /fall 10us
  delayMicroseconds(25);
  int bright_val = analog_read_avg();
  digitalWrite(PULL_1, HIGH);  
  switch_led(false);
  int reflected = bright_val - dark_val;
  static int reflected_last = 0;
//  Serial.print("now ");
//  Serial.print(reflected);  
/*  Serial.print(" low ");
  Serial.print(dark_val);  
  Serial.print(" hi ");
  Serial.println(bright_val);  */
  
  int thresh = 50;
  int refl_diff = reflected - reflected_last;
  Serial.print("diff ");
  Serial.print(refl_diff);  
  Serial.print(" now ");
  Serial.println(reflected);  

  if(abs(refl_diff) > thresh)
    Serial.println("U ");
  reflected_last = reflected;
  delay(50);
/*  if(s_even == true)
  {
    pinMode(PULLS, OUTPUT);
    digitalWrite(PULLS, HIGH);
    s_even = false;
  }
  else
  {
    pinMode(PULLS, INPUT);
    s_even = true;
  }
  Serial.println(s_even);
  */
  counts++;
  if(millis() - time_last >= 1000)
  {
//    Serial.println(counts);
    counts = 0;
    time_last = millis();
  }
}
