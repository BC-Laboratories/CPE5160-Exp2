#include "main.h"
#include "PORT.h"
#include "Timer0_hardware_delay_1ms.h"

//****** Timer 0 is used for blocking delays *******//

// ***** Caution: Uses Timer0 *******//
void Timer0_DELAY_1ms(uint16_t duration)
{
   // Configure Timer 0 as a 16-bit timer 
   TMOD &= 0xF0; // Clear all T0 bits (T1 left unchanged)
   TMOD |= 0x01; // Set required T0 bits (T1 left unchanged) 

	  
   ET0 = 0;  // No interupts
      
   do
   {
     // Values for 1 ms delay
      TH0 = Timer_Reload_1ms_THx;  // Timer 0 initial value (High Byte)
      TL0 = Timer_Reload_1ms_TLx;  // Timer 0 initial value (Low Byte)

      TF0 = 0;          // Clear overflow flag
      TR0 = 1;          // Start timer 0

      while (TF0 == 0); // Loop until Timer 0 overflows (TF0 == 1)

      TR0 = 0;          // Stop Timer 0
	  duration--;
	}while(duration>0);
}




