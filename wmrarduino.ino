const int LED_PIN = 9;
const int IN_1 = A0;
const int PULL_1 = 8;
int current_data;
int max_data ;
int min_data ;
int last_data;
int average;
int c;
int x;
const int buffer_len = 256;
//int rms[buffer_len];
int avg_rms;
bool current_state;
bool last_state;
int impulss;
int last_impulss;

uint16_t data[buffer_len];

int count;
int sample = 0;

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

void print_bool(bool data)
{
  if (data == true)
  {
    Serial.print("high");
  }
  else
  {
    Serial.print("low");
  }
}

void calibration(int data_now)
{
  int rms;
  int avg[buffer_len];
  static int i = 0;
  int f;
  
  data[i] = data_now;
  i++;
  if (i == buffer_len)
  {
    uint32_t average_sum = 0;
    for (int x = 0; x < buffer_len; x++)
    { 
      delay(1);
      average_sum += data[x];
     
      //Serial.println(data[x]);
     
      //Serial.print("average sum : ");
      //Serial.println(average_sum);
    }
     
    average = average_sum / buffer_len;
    Serial.print("average");
    Serial.println(average);
    uint64_t rms_sq_sum = 0;
    for(int c = 0; c < buffer_len; c++)
    {
      rms_sq_sum = rms_sq_sum + (data[c] - average)*(data[c] - average);
    }
    
    avg_rms = sqrt(rms_sq_sum/buffer_len);
    
     
    Serial.print("average rms : ");
    Serial.println(avg_rms);
    i = x = 0;
  }
  
  
}

void calculation(int current_data)
{
  int sample_turn;
  
  /*if (current_data > max_data)
    {
   max_data = current_data;
    }
   
   if (current_data < min_data)
    {
   min_data = current_data;
    }
   
   if ((current_data > last_data) & (current_data-last_data > (max_data-min_data)/2))
    {
   current_state = high;
    }
   
   if ((current_data < last_data) & ((last_data - current_data) > (max_data-min_data)/2))
    {
   current_state = low;
    }
   
   
    */
  
  //Serial.print("current-average");
  //Serial.println(current_data-average);
  if (avg_rms/3 > 6)
  {
    if (current_data-average > avg_rms/3)
    {
       current_state = true;
    }
  
    if (current_data-average < avg_rms/3)
    {
      current_state = false;
    }
    
    if (current_state != last_state)
    {
      count++;
    }
    if(current_state != last_state)
    {
      Serial.print( "current state: ");
      print_bool(current_state);
      Serial.println("");
    }
    last_data = current_data;
    last_state = current_state;
    impulss = count / 2;
    if (last_impulss != impulss)
    {
      Serial.print( "impulss : ");
      Serial.println(impulss);
    }
    last_impulss = impulss;
    sample_turn ++;
    if(sample_turn>=1)
    {
      //Serial.println(current_data);
      calibration(current_data);
    
    }
   }
  if (avg_rms/3 <= 6)
  {
    
    //Serial.println(current_data);
    
    calibration(current_data);
    
  }
  
}

/*
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
*/
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
  
  int refl_diff = reflected - reflected_last;
  if (sample < buffer_len)
    {
      sample++;
      calibration(reflected);
    }
    if (sample == buffer_len)
    {
      calculation(reflected);
      
    } 

 
  reflected_last = reflected;
  delay(1);
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
