 #ifndef SUPER_REAL_TIME_H
 #define SUPER_REAL_TIME_H

int gpio_res; 
spi_t *spi;
gpio_t *gpio_in;
uint8_t buf[27]={0};
uint8_t zero27[27]={0x00};
uint32_t package [8]={0};  
uint32_t data_test = 0x7FFFFF;
uint32_t data_check = 0xFFFFFF;
uint8_t zero3[3] = {0x00, 0x00,0x00}; 
uint8_t zero1[1] = {0x00};

           void write_reg(uint8_t reg_address, uint8_t val_hex)
           {                                            
           uint8_t reg_address_shift = 0x40 | reg_address;
           uint8_t write [3] = {reg_address_shift, 0x00,val_hex};
           int spi_reg = spi_transfer (spi, write, zero3, 3);
           }
           
         void send_command(uint8_t command)
           {           
           uint8_t write_command [1] = {command};                    
           spi_transfer (spi, write_command, zero1, 1);

           }  

        void prepare ()
        {

                                         
        spi = spi_new();
        spi_open_advanced(spi, "/dev/spidev0.0", 0x02, 20000000, MSB_FIRST, 8, 0);   //spi_open_advanced(spi, "/dev/spidev0.0", 0x01, 4000000, MSB_FIRST, 8, 1);        
          
        gpio_in = gpio_new();
        gpio_open (gpio_in, "/dev/gpiochip0", 26, GPIO_DIR_IN);
        gpio_set_edge (gpio_in, GPIO_EDGE_FALLING);  
                                             
          write_reg (0x14,0x80);//led
          write_reg (0x05,0x00);//ch1
          write_reg (0x06,0x00);//ch2
          write_reg (0x07,0x00);//ch3
          write_reg (0x08,0x00);//ch4
          
           write_reg (0x0D,0xFF);//bias N
          
           write_reg (0x0E,0x00);//bias P
          
          write_reg (0x09,0x00);//ch5
          write_reg (0x0A,0x00);//ch6
          write_reg (0x0B,0x00);//ch7
          write_reg (0x0C,0x00);//ch8
          write_reg (0x15,0x20);// mics
          write_reg (0x01,0x96);// reg1
          write_reg (0x02,0xD4);// reg2 D4 
          write_reg (0x03,0xE0);// reg3 E0
          send_command (0x10);//sdatc
          send_command (0x08);//start
          
}

   
                
   int real () 
   { 
    
     int timeout = 1000;
        gpio_res = gpio_poll (gpio_in, timeout);  
        
        if (gpio_res == 1)
        {   
            gpio_edge_t edge = GPIO_EDGE_NONE;
            gpio_read_event (gpio_in, &edge, NULL);
            spi_transfer (spi, zero27, buf, 27);
            
            if (buf[0]==192)
                {  
                for (int i = 1; i<9; i++)  
                {                                
                int offset = 3*i;
                int32_t voltage = (buf[offset] << 8) | buf[offset+1];
                voltage = (voltage << 8) | buf[offset+2];


                uint32_t voltage_test = voltage | data_test;

                    if (voltage_test == data_check)
                    {
                    voltage = 16777214 - voltage;                   
                    }
                  
                  
                  package[i-1]=0.27*voltage;   // (4.5*1000000/16777214)=0.27 
                  
                } 
         //       int test = 234;
                  if (package[0]>0 && package[1]>0 && package[2]>0 && package[3]>0 && package[4]>0 && package[5]>0 && package[6]>0 && package[7]>0)
                  {
                  int test = package[0];  
                  return (test);
                  }
                  usleep(100);
            }
          
        } 
        
   //int test = 234;
   //return (package[2]);
   
    }
   #endif
