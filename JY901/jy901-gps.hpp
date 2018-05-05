#pragma once

class JY901_GPS 
    : public JY901
{
public:
    JY901_GPS (I2C *bus): JY901(bus) {
        set_gps_mode();
    }
    JY901_GPS (I2C *bus,  char address): JY901(bus, address) {
        set_gps_mode();
    }
    void set_GPS_baud(JY_Serial_Baud gps_baud){
        this->write(0x1C, (char)gps_baud);
    }



   /** set_pin_mode
    * @bref set pin mode
    * @param pin_ID   : pin number that modify pin mode
    * @param pin_mode : pin mode , select form JY_Pin_Mode enum (EX Anolog Input. JY901_Type::JY_ANALOG)
    * @remarks If you use with GPS, you can't use pin1.
    */
     void set_pin_mode (int pin_ID, JY_Pin_Mode pin_mode){
        if(pin_ID > 3) return;
        if(pin_ID == 1) return;
        this->write( (char)(0x0e + pin_ID), (char)pin_mode);
    }

  
   /** set_pin_write
    * @bref in Digital out pin , set voltage High(1) or Low(0)
    * @param pin_ID   : pin number that modify pin voltage
    * @param state    : pin Voltage High or Low (1 / 0)
    * @remarks : Distinction of 1/0 is done with lower 1 bit.
    * @remarks If you use with GPS, you can't use pin1.
    */
    void set_pin_write(int pin_ID, int state){
        if(pin_ID > 3) return;
        if(pin_ID == 1) return;
        this->write( (char)(0x0e + pin_ID), (char)(0x10|(state&1)));
    }

   /** set_pwm_width
    * @bref set pwm width length
    * @param pin_ID   : pin number that modify pin voltage
    * @param high_width_us    : length pin out high voltage in micro sec
    * @remarks If you use with GPS, you can't use pin1.
    */
    void set_pwm_width (int pin_ID, unsigned short high_width_us){
        char dum[2];
        if(pin_ID > 3) return;
        if(pin_ID == 1) return;

        dum[0] = high_width_us & 0x00FF;
        dum[1] = high_width_us >> 8;
        this->write( (char)(0x12 + pin_ID), dum, 2);
    }

   /** set_pwm_power
    * @bref set pwm with duty rate
    * @param pin_ID   : pin number that modify pin voltage
    * @param duty_rate: setting duty rate in range 0 to 1
    * @remarks This is third pirson Method. Even if this dosn't work well, There is NO warranty. I recommend you to use set_pwm_width method.
    * @remarks If you use with GPS, you can't use pin1.
    */
    void set_pwm_power (int pin_ID, float duty_rate){
        if(pin_ID > 3) return;
        if(pin_ID == 1) return;
        if(duty_rate < 0) duty_rate = 0;
        if(duty_rate > 1) duty_rate = 1;
        set_pwm_width(pin_ID, (unsigned short)(duty_rate * periods[pin_ID]));
    }

   /** set_pwm_period
    * @bref set pwm period length
    * @param pin_ID   : pin number that modify pin voltage
    * @param period_us    : pwm period length
    * @remarks If you use with GPS, you can't use pin1.
    */
    void set_pwm_period (int pin_ID, unsigned short period_us){
        char dum[2];
        if(pin_ID > 3) return;
        if(pin_ID == 1) return;
        dum[0] = period_us & 0x00FF;
        dum[1] = period_us >> 8;
        this->write( (char)(0x16 + pin_ID), dum, 2);
        periods[pin_ID] = period_us;
    }



    JY_Geographical_Position get_geographical_position(void){
        JY_Geographical_Position position_state;
        char buff[9];
        this->read(0x57, buff, 9);

        position_state.longitude = ( buff[3]<<24)| ( buff[2]<<16)| ( buff[1]<<8)| buff[0]; 
        position_state.latitude   = ( buff[7]<<24)| ( buff[6]<<16)| ( buff[5]<<8)| buff[4]; 
        return position_state;  
    }
private:
    void set_gps_mode(void){

    }
};