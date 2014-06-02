/* Simple function to sent a char at a time to 
** rtty_txbyte function. 
** NB Each char is one byte (8 Bits)
*/
void rtty_txstring (char * string)
{

  char c;
  c = *string++;
  while ( c != '\0')
  {
    rtty_txbyte (c);
    c = *string++;
  }
}



/* Simple function to sent each bit of a char to 
** rtty_txbit function. 
** NB The bits are sent Least Significant Bit first
**
** All chars should be preceded with a 0 and 
** proceded with a 1. 0 = Start bit; 1 = Stop bit
**
*/
void rtty_txbyte (char c)
{

  int i;
  rtty_txbit (0); // Start bit
  // Send bits for for char LSB first
  for (i=0;i<7;i++) // Change this here 7 or 8 for ASCII-7 / ASCII-8
  {
    if (c & 1) rtty_txbit(1);
    else rtty_txbit(0);
    c = c >> 1;
  }
  rtty_txbit (1); // Stop bit
}



void rtty_txbit (int bit)
{
  if (bit)
  {
    // high
    digitalWrite(RADIOPIN, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
  }
  else
  {
    // low
    digitalWrite(RADIOPIN, LOW);
    digitalWrite(YELLOW_LED, LOW);
  }
//   delayMicroseconds(3370); // 300 baud
  delayMicroseconds(10000); // For 50 Baud uncomment this and the line below. 
  delayMicroseconds(10150); // For some reason you can't do 20150 it just doesn't work.
}
