#include <RCSwitch.h>

#define NUM_OF_RETRANSMITS 2

#define RC_SWITCH_LEN 24
char buf[RC_SWITCH_LEN + 1];

char temp_rcv_buf[RC_SWITCH_LEN+1];
static int num_of_retransmits = NUM_OF_RETRANSMITS;


char*  dec2binWcharfill(unsigned long Dec, unsigned int bitLength, char fill){
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : fill;
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = fill;
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}

void rc_switch_init()
{
  
  // Receiver
  //pinMode(9, INPUT_PULLUP);
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2

  // Transmitter
  mySwitch.enableTransmit(3);
  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(20);
  
  //mySwitch.setReceiveTolerance(75);
}

boolean rc_switch_receive()
{
  boolean ret;
  
  if (mySwitch.available())
  {
    int value = mySwitch.getReceivedValue();

    if (value == 0)
    {
          Serial.println("[E]Received_unknown_encoding");
    }
    else
    {

      ret = true;
      
      if ((mySwitch.getReceivedBitlength() == 24) && (mySwitch.getReceivedProtocol() == 1))
      {
          memcpy(temp_rcv_buf, dec2binWcharfill(mySwitch.getReceivedValue(), RC_SWITCH_LEN, '0'), RC_SWITCH_LEN+1);
        
          if (validate_received_data(temp_rcv_buf))
          {
              Serial.print("[R]");
              Serial.println(temp_rcv_buf);
          }
    
      }
      else
      {
          Serial.println("[E]Received_unhandled_protocol");
      }
      
      //output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    }

    mySwitch.resetAvailable();
  }
  else
  {
    ret = false;
  }
  
  return ret;
}

void set_num_of_retransmits(int num)
{
  num_of_retransmits = num;
}

void set_default_num_of_retransmits()
{
  num_of_retransmits = NUM_OF_RETRANSMITS;
}

#if 0
String parse_num_of_retransmissions(String msg)
{
  
    set_default_num_of_retransmits();
    return msg.substring(3);
}
#else


// A simple atoi() function
int myAtoi(char *str)
{
    int res = 0; // Initialize result
 
    // Iterate through all characters of input string and update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
 
    // return result.
    return res;
}

String parse_num_of_retransmissions(String msg)
{
    int pos = msg.indexOf(']');
    
    String ret;
    
    
    if (pos >= 0 )
    {
        if ((pos - msg.indexOf('[')) > 2 )
        {
            int num;
            ret = msg.substring(2, pos);
            ret.toCharArray(buf, ret.length() + 1);
            num = myAtoi(buf);
            set_num_of_retransmits(num);
        }
        else
        {
            set_default_num_of_retransmits();
        }
    }
    
  
    return msg.substring(pos+1);
}
#endif

void rc_switch_transmit(String msg)
{
  
  msg = parse_num_of_retransmissions(msg);
  
  msg.toCharArray(buf, RC_SWITCH_LEN + 1);
  
  for (int i = 0; i < num_of_retransmits; i++)
  {
      mySwitch.send(buf);
      watchdog_clear();
      delay(200);
  }
}

static unsigned long last_rcv_cmd_time = millis();
static bool rcv_command_cnt = 0;

boolean validate_received_data(char * buf)
{
  
  unsigned long curr_time = millis();
  
  if (curr_time - last_rcv_cmd_time > 200)
  {
    last_rcv_cmd_time = curr_time;
    return 1;
  }
  else
  {
    last_rcv_cmd_time = curr_time;
    return 0;
  }
  
  //return (!memcmp("0100", &buf[20], 4) ||
  //        !memcmp("0001", &buf[20], 4)
  //        );
}


/////////
void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {

  if (decimal == 0) {
    Serial.print("Unknown encoding.");
  } else {
    char* b = dec2binWzerofill(decimal, length);
    Serial.print("Decimal: ");
    Serial.print(decimal);
    Serial.print(" (");
    Serial.print( length );
    Serial.print("Bit) Binary: ");
    Serial.print( b );
    Serial.print(" Tri-State: ");
    Serial.print( bin2tristate( b) );
    Serial.print(" PulseLength: ");
    Serial.print(delay);
    Serial.print(" microseconds");
    Serial.print(" Protocol: ");
    Serial.println(protocol);
  }
  
  Serial.print("Raw data: ");
  for (int i=0; i<= length*2; i++) {
    Serial.print(raw[i]);
    Serial.print(",");
  }
  Serial.println();
  Serial.println();
}


static char* bin2tristate(char* bin) {
  char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = (Dec & 1 > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}
