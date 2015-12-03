#include <RCSwitch.h>
#include <QueueList.h>

#define RED_LED 13

String msg;
RCSwitch mySwitch = RCSwitch();
QueueList <String> msg_queue;

void setup()
{
  watchdog_init();
    
  //pinMode(GREEN_LED, OUTPUT);  
  pinMode(RED_LED, OUTPUT);  
  //pinMode(BLUE_LED, OUTPUT);  
  
  Serial.begin(115200); 
  
  rc_switch_init();
  Serial.print("RCswitch initialized");
}

void loop()
{
  boolean rc_received;
  
  watchdog_clear();
  
  rc_received = rc_switch_receive();
  digitalWrite(RED_LED, rc_received);  
  
  while (Serial.available() > 0)
  {
      uart_handle_rx(Serial.read());
  }
  
  if (!msg_queue.isEmpty())
  {
      digitalWrite(RED_LED, HIGH);
      msg = msg_queue.pop();
      
      Serial.print("Msgs in queue:");
      Serial.println(msg_queue.count());
      Serial.println(msg);
      rc_switch_transmit(msg);

      digitalWrite(RED_LED, LOW);
  }
}


