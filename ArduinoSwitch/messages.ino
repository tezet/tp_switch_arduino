
void uart_handle_rx(char ch)
{
  #define UART_RX_BUF_LEN 256
  static int uart_rx_buf_cnt = 0;
  static char uart_rx_buf[UART_RX_BUF_LEN];
  
  if (ch == '\r' || ch == '\n')
  {
      uart_rx_buf[uart_rx_buf_cnt++] = '\0';
      uart_rx_buf_cnt = 0;
      
      switch (uart_rx_buf[1])
      {
        case 'T':
          if (is_rc_message(uart_rx_buf))
          {
             msg_queue.push(&uart_rx_buf[0]);
          }
          else
          {
            Serial.print("Error: invalid length");
            Serial.println(uart_rx_buf);   
          }
          break;
        default:
          Serial.print("Error: unknown command: ");
          Serial.println(uart_rx_buf);   
        break;
      }
      

  }
  else
  {
      uart_rx_buf[uart_rx_buf_cnt++] = ch;
  }

  if (uart_rx_buf_cnt >= UART_RX_BUF_LEN)
  {
      uart_rx_buf_cnt = 0;
  }
  
}

#define RC_SWITCH_MSG_LEN 27
boolean is_rc_message(char * msg)
{
  return 1;
  //return strlen(msg) == RC_SWITCH_MSG_LEN;
}

