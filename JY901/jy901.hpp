#pragma once
#include "jy901-type.hpp"

/** @file
 *
 * JY901 library.
 *
 * To use with UART, 
 * insert "#define JY901_SERIAL" before include.
 */
#ifndef M_PI
#define M_PI 3.1415926535897932384626
#endif

static const char JY_ADDR                = 0x50;
static const char JY_901_DEFAULT_CONTENT = 0x8F;


#define INTERFACE I2C
#define EXTEND_INTERFACE_CLASS MyI2C


/** hogehage class
 *
 * This is the class.
 */

using namespace JY901_Type;

/** JY901 Class
 * This is the JY901 9 axis sensors library class.
 */
class JY901 
    :public EXTEND_INTERFACE_CLASS
{
public:
	/** constructor 
	* @bref Create an instance from the address of I2C intance.
	* @remarks  Slave address is automatically settled 0x50.
	* @param bus : Pointer of I2C Instance (or Serial Instance).
	*/
    JY901(INTERFACE *bus): EXTEND_INTERFACE_CLASS(bus, JY_ADDR) {}

    /** constructor 
	* @bref Create an instance from the address of I2C intance and I2C Address.
	* @param bus  	 :	Pointer of I2C Instance (or Serial Instance).
	* @param address : 	Slave address number that you settled before.
	*/
    JY901(INTERFACE *bus,  char address): EXTEND_INTERFACE_CLASS(bus, address) {}
    

    /** set_default_setting
     * @bref set IC configure to default. 
     */
    void set_default_setting(){
    	char cmd[2] = {0x01, 0x00};
    	this->write(0x00, cmd, 2);
    }


    /**  enter_gyroscope_calibration
     * @bref start gyroscope calibration mode 
	 * @param interval_ms : wait interval_ms in this method <br>and exit carlibration automatically.
	 * @remarks If you abbreviate parametor or set parametor to zero,<br> you should call exit_calibration method 
     */
	void enter_gyroscope_carlibration(int interval_ms = 0){
        char cmd[2] = {0x01, 0x00};
        this->write(0x01, cmd, 2);
        if(!ms){
        	wait_ms(interval_ms);
        	exit_carlibration();
        }
    }

    /**  enter_magnetic_calibration
     * @bref start magnetic calibration mode 
	 * @param interval_ms : wait interval_ms in this method <br>and exit carlibration automatically.
	 * @remarks If you abbreviate parametor or set parametor to zero,<br> you should call exit_calibration method 
     */
    void enter_magnetic_carlibration(int interval_ms = 0){
        char cmd[2] = {0x02, 0x00};
        this->write(0x01, cmd, 2);

         if(!ms){
        	wait_ms(interval_ms);
        	exit_carlibration();
        }
    }

    /**  enter_height_calibration
     * @bref start height calibration mode 
	 * @param interval_ms : wait interval_ms in this method <br>and exit carlibration automatically.
	 * @remarks This is unique function of JY901B <br>(or other series that has baromator)
	 * @remarks If you abbreviate parametor or set parametor to zero, <br>you should call exit_calibration method 
     */
    void enter_height_carlibration(int interval_ms = 0){
    	char cmd[2] = {0x03, 0x00};
        this->write(0x01, cmd, 2);
        if(!ms){
        	wait_ms(interval_ms);
        	exit_carlibration();
        }
    }

    /** exit_carlibration
     * @bref Exit calibration mode.
     * @remarks You don't have to call this function <br>if you call enter carlibration method with parametor.
     */
    void exit_carlibration(void){
        char cmd[2] = {0x03, 0x00};
        this->write(0x01, cmd, 2);
    }

/**
	todo : add set return content method
*/
    /** set_return_rate
     * @bref set sample speed 
     * @param rate : sampling rate 
     * @remark You have to select parametor form JY_Sampling_Rate enum. <br>(EX 200Hz. JY901_Type::RATE_HZ200)
     */
    void set_return_rate(JY_Sampling_Rate rate){
    	char cmd[2] = {(char)rate, 0x00};
        this->write(0x03, cmd, 2);
    }
   
   /** set_serial_baudrate
    * @bref set serial baundrate
   	* @param rate : serial baundrate , select form JY_Serial_Baud enum. <br>(EX 115200bps JY901_Type::JY_SERIAL_115200)
    * @remarks After re-power ,it will take effect.
    */
    void set_serial_baudrate(JY_Serial_Baud rate){
    	char cmd[2] = {(char)rate, 0x00};
        this->write(0x01, cmd, 2);
        save_settings();
    }

/**
	todo : bias settings
*/

   /** set_pin_mode
    * @bref set pin mode
   	* @param pin_ID   : pin number that modify pin mode
   	* @param pin_mode : pin mode , select form JY_Pin_Mode enum <br>(EX Anolog Input. JY901_Type::JY_ANALOG)
    */
     void set_pin_mode (int pin_ID, JY_Pin_Mode pin_mode){
        if(pin_ID > 3) return;
        this->write( (char)(0x0e + pin_ID), (char)pin_mode);
    }

  
   /** set_pin_write
    * @bref in Digital out pin , set voltage High(1) or Low(0)
   	* @param pin_ID   : pin number that modify pin voltage
   	* @param state    : pin Voltage High or Low (1 / 0)
   	* @remarks : Distinction of 1/0 is done with lower 1 bit.
    */
    void set_pin_write(int pin_ID, int state){
        if(pin_ID > 3) return;
        this->write( (char)(0x0e + pin_ID), (char)(0x10|(state&1)));
    }

   /** set_pwm_width
    * @bref set pwm width length
    * @param pin_ID   : pin number that modify pin voltage
   	* @param high_width_us    : length pin out high voltage in micro sec
    */
    void set_pwm_width (int pin_ID, unsigned short high_width_us){
        char dum[2];
        if(pin_ID > 3) return;

        dum[0] = high_width_us & 0x00FF;
        dum[1] = high_width_us >> 8;
        this->write( (char)(0x12 + pin_ID), dum, 2);
    }

   /** set_pwm_power
    * @bref set pwm with duty rate
    * @param pin_ID   : pin number that modify pin voltage
   	* @param duty_rate: setting duty rate in range 0 to 1
   	* @remarks This is third pirson Method. Even if this dosn't work well, There is NO warranty. <br>I recommend you to use set_pwm_width method.
    */
    void set_pwm_power (int pin_ID, float duty_rate){
        if(pin_ID > 3) return;
        if(duty_rate < 0) duty_rate = 0;
        if(duty_rate > 1) duty_rate = 1;
        set_pwm_width(pin_ID, (unsigned short)(duty_rate * periods[pin_ID]));
    }

   /** set_pwm_period
    * @bref set pwm period length
   	* @param pin_ID   : pin number that modify pin voltage
   	* @param period_us    : pwm period length
   	*/
    void set_pwm_period (int pin_ID, unsigned short period_us){
        char dum[2];
        if(pin_ID > 3) return;

        dum[0] = period_us & 0x00FF;
        dum[1] = period_us >> 8;
        this->write( (char)(0x16 + pin_ID), dum, 2);
        periods[pin_ID] = period_us;
    }

   /** set_new_i2c_address
    *  @bref set new slave ID
    *  @param new I2C address (7bit)
    *  @remarks After re-power ,it will take effect.
    *  @remarks If you forget new slaveAddress <br>you should connect with Serial or do Restore factory setting.
    *  @remarks Resotre setting -> short D2 to VCC and power on
    */
    void set_new_i2c_address(char new_address){
    	char cmd[2] = { new_address, 0x00};
        this->write(0x01, cmd, 2);
        this->save_settings();
    }

   /** set_led
    *  @bref set led state to ON or OFF
    *  @param state LED state (True - on , False - off)
    *  @remarks After re-power ,it will take effect.
    */
    void set_led(bool state){
        if(state){
            this->write(0x1b, 0x00);
        }else{
            this->write(0x1b, 0x01);
        }
        this->save_settings();
    }



	/** get_pressure_height
	 * @bref Get Pressure and Height form barometor
	 * @return JY901_Type::JY_Pressure_Height
	 * @retval .pressure can get float pressure.
	 * @retval .height can get float heigth. 
	 * @remarks This is unique function of JY901B (or other series that has baromator)
	 */
    JY_Pressure_Height get_pressure_height(void){
        char buff[9];
        this->read(0x45, buff, 9);
        
        height_state.pressure = ( buff[3]<<24)| ( buff[2]<<16)| ( buff[1]<<8)| buff[0]; 
        height_state.height   = ( buff[7]<<24)| ( buff[6]<<16)| ( buff[5]<<8)| buff[4]; 
        return height_state;  
    }
    
	/** get_time
	 * @bref Get Time.
	 * @return JY901_Type::JY_Time
	 * @retval .year can get year now in short.
	 * @retval .month can get month now in char.  
	 * @retval .day can get day now in char. 
	 * @retval .hour can get hour now in char. 
	 * @retval .min can get minuite now in char. 
	 * @retval .sec can get secound now in char. 
	 * @retval .ms can get milli secound now in short. 
	 * @remarks If there are no GPS <br> time of the module will be set January 1, 2015 0:00'00"00 when power on.
	 */
    JY_Time get_time(void){
        JY_Time ret ;
        char buff[9];
        this->read(0x30, buff, 9);
        ret.year = 2000 + buff[0];
        ret.month = buff[1];
        ret.day = buff[2];
        ret.hour = buff[3];
        ret.min = buff[4];
        ret.sec = buff[5];
        ret.ms = (buff[7] << 8) | buff[6];
        return ret;
    }


    /** 
	 * get_acceleration
	 *	@bref get 3axis acceleration
	 *	@return JY901_Type::JY_Dim_3D 
	 *	@retval .x can get x component in float
	 *	@retval .y can get y component in float
	 *	@retval .z can get z component in float
	 *	@retval .data can get as float array which has 3 length.
    */
    JY_Dim_3D get_acceleration(void){
        JY_Dim_3D ret;
        char buff[6];
        this->read(0x34, buff, 6);
        ret.x 		= ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * 16 * 9.8;
        ret.y 		= ((buff[3]<< 8 ) | buff[2] ) / 32768.0f * 16 * 9.8;
        ret.z 		= ((buff[5]<< 8 ) | buff[4] ) / 32768.0f * 16 * 9.8;
        return ret;
    }

    /** 
	 * get_acceleration_x
	 *	@bref get x-axis acceleration
	 *	@return x-axis acceleration in float
	 *  @remarks When you need 3 axis data get_acceleration is better in terms of speed.
    */
    float get_acceleration_x(void){
        char buff[2];
        this->read(0x34, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] )/ 32768.0f * 16 * 9.8;
    }

    /** 
	 * get_acceleration_y
	 *	@bref get y-axis acceleration
	 *	@return y-axis acceleration in float
	 *  @remarks When you need 3 axis data get_acceleration is better in terms of speed.
    */
    float get_acceleration_y(void){
        char buff[2];
        this->read(0x35, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] )/ 32768.0f * 16 * 9.8;
    }

    /** 
	 * get_acceleration_z
	 *	@bref get z-axis acceleration
	 *	@return z-axis acceleration in float
	 *  @remarks When you need 3 axis data get_acceleration is better in terms of speed.
    */
    float get_acceleration_z(void){
        char buff[2];
        this->read(0x36, buff, 2); 
        return ((buff[1]<< 8 ) | buff[0] )/ 32768.0f * 16 * 9.8;
	}

    /** 
	 * get_angular_velocity
	 *	@bref get 3axis angular_velocity
	 *	@return JY901_Type::JY_Dim_3D 
	 *	@retval .x can get x component in float
	 *	@retval .y can get y component in float
	 *	@retval .z can get z component in float
	 *	@retval .data can get as float array which has 3 length.
    */
    JY_Dim_3D get_angular_velocity(void){
        JY_Dim_3D ret;
        char buff[6];
        this->read(0x37, buff, 6);
        ret.x 	= ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * 2000;
        ret.y 	= ((buff[3]<< 8 ) | buff[2] ) / 32768.0f * 2000;
        ret.z 	= ((buff[5]<< 8 ) | buff[4] ) / 32768.0f * 2000;
        return ret;
    }

    /** 
	 * get_angular_velocity_x
	 *	@bref get x-axis angular velocity
	 *	@return x-axis angular velocity in float
	 *  @remarks When you need 3 axis data get_angular_velocity is better in terms of speed.
     */
    float get_angular_velocity_x(void){
        char buff[2];
        this->read(0x37, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] )/ 32768.0f * 2000;
    }

   /** 
	 * get_angular_velocity_y
	 *	@bref get y-axis angular velocity
	 *	@return y-axis angular velocity in float
	 *  @remarks When you need 3 axis data get_angular_velocity is better in terms of speed.
     */
    float get_angular_velocity_y(void){
        char buff[2];
        this->read(0x38, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] )/ 32768.0f * 2000;
    }

   /** 
	 * get_angular_velocity_z
	 *	@bref get z-axis angular velocity
	 *	@return z-axis angular velocity in float
	 *  @remarks When you need 3 axis data get_angular_velocity is better in terms of speed.
     */
    float get_angular_velocity_z(void){
        char buff[2];
        this->read(0x39, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] )/ 32768.0f * 2000;
    }


    /** 
	 * get_magnetic
	 *	@bref get 3axis get_magnetic
	 *	@return JY901_Type::JY_Dim_3D 
	 *	@retval .x can get x component in float
	 *	@retval .y can get y component in float
	 *	@retval .z can get z component in float
	 *	@retval .data can get as float array which has 3 length.
    */
    JY_Dim_3D get_magnetic(void){
        JY_Dim_3D ret;
        char buff[6];
        this->read(0x3a, buff, 6);
        ret.x 		= ((buff[1]<< 8 ) | buff[0] );
        ret.y 		= ((buff[3]<< 8 ) | buff[2] );
        ret.z 		= ((buff[5]<< 8 ) | buff[4] );
        return ret;
    }
   /** 
	 * get_magnetic_x
	 *	@bref get x-axis magnetic
	 *	@return x-axis magnetic in float
	 *  @remarks When you need 3 axis data get_magnetic is better in terms of speed.
     */
    float get_magnetic_x(void){
        char buff[2];
        this->read(0x3a, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] );
    }

       /** 
	 * get_magnetic_y
	 *	@bref get y-axis magnetic
	 *	@return y-axis magnetic in float
	 *  @remarks When you need 3 axis data get_magnetic is better in terms of speed.
     */
    float get_magnetic_y(void){
        char buff[2];
        this->read(0x3b, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] );
    }

   /** 
	 * get_magnetic_z
	 *	@bref get z-axis magnetic
	 *	@return z-axis magnetic in float
	 *  @remarks When you need 3 axis data get_magnetic is better in terms of speed.
     */
    float get_magnetic_z(void){
        char buff[2];
        this->read(0x3c, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] );
    }


    /** 
	 * get_pitch_angle
	 *	@bref get pitch angle in radian.
	 *	@return JY901_Type::JY_Dim_3D .
	 *	@retval .roll can get roll angle in float.
	 *	@retval .pitch can get pitch angle in float.
	 *	@retval .yow can get yow anggle in float.
    */
    JY_Pitch_Angle get_pitch_angle(void){
        char buff[6];
        this->read(0x3D, buff, 6);
        angle_state.roll     = ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * M_PI;
        angle_state.pitch    = ((buff[3]<< 8 ) | buff[2] ) / 32768.0f * M_PI;
        angle_state.yow      = ((buff[5]<< 8 ) | buff[4] ) / 32768.0f * M_PI;
        return angle_state;
    }
   /** 
	 * get_pitch_angle 
	 * @bref get pitch angle in radian.
	 * @param roll is float pointer that save roll angle value.
	 * @param pitch is float pointer that save pitch angle value.
	 * @param yow is float pointer that save yow angle value.
    */
    void get_pitch_angle(float *roll, float *pitch, float *yow){
        char buff[6];
        this->read(0x3D, buff, 6);
        angle_state.roll     = ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * M_PI;
        angle_state.pitch    = ((buff[3]<< 8 ) | buff[2] ) / 32768.0f * M_PI;
        angle_state.yow      = ((buff[5]<< 8 ) | buff[4] ) / 32768.0f * M_PI;
       
        *roll     = angle_state.roll;
        *pitch    = angle_state.pitch;
        *yow      = angle_state.yow;
    }

   /** 
	 * get_roll
	 *	@bref get roll angle.
	 *	@return roll angle in float.
	 *  @remarks When you need all of pitch angle data get_pitch_angle is better in terms of speed.
     */
    float get_roll(void){
        char buff[2];
        this->read(0x3D, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * M_PI;
    }

   /** 
	 * get_pitch
	 *	@bref get pitch angle.
	 *	@return pitch angle in float.
	 *  @remarks When you need all of pitch angle data get_pitch_angle is better in terms of speed.
     */    
    float get_pitch(void){
        char buff[2];
        this->read(0x3E, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * M_PI;
    }

   /** 
	 * get_yow
	 *	@bref get yow angle.
	 *	@return yow angle in float.
	 *  @remarks When you need all of pitch angle data get_pitch_angle is better in terms of speed.
     */
    float get_yow(void){
        char buff[2];
        this->read(0x3F, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] ) / 32768.0f * M_PI;
    }

   /** 
	 * get_temperture
	 *	@bref get temperture in degree.
	 *	@return degree temperture in float.
     */
    float get_temperture(void){
        char buff[2];
        this->read(0x40, buff, 2);
        return ((buff[1]<< 8 ) | buff[0] ) / 100.0f;
    }

    /** 
	 * get_pin_status
	 *	@bref getter of all pin statuses
	 *	@return JY901_Type::JY_Pin_Status. 
	 *	@retval .P0 D0 pin status in short.
	 *	@retval .P1 D1 pin status in short.
	 *	@retval .P2 D2 pin status in short.
	 *	@retval .P3 D3 pin status in short.
	 *	@retval .P  short type array store pin status that four length.
    */    
    JY_Pin_Status get_pin_status(void){
        JY_Pin_Status ret;
        char buff[8];
        this->read(0x41, buff, 8);
        ret.P0 = ((buff[1]<< 8 ) | buff[0] );
        ret.P1 = ((buff[3]<< 8 ) | buff[2] );
        ret.P2 = ((buff[5]<< 8 ) | buff[4] );
        ret.P3 = ((buff[7]<< 8 ) | buff[6] );
        return ret;
    }

    /** 
	 * get_pin_status
	 *	@bref getter of one pin status
	 *	@return pin status in unsigned short.
    */    
    unsigned short get_pin_status(int pin_ID){
        if(pin_ID > 3) return 0;
        char buff[2];
        this->read((char)(0x41 + pin_ID), buff, 2);
        return ((buff[1]<< 8 ) | buff[0] );
    }

    /**
    * convert_pin_status_to_voltage
    * @bref calculate analog voltage from pin status short value.
    * @param pin_status  pin_status got by get_pin_status method. 
    * @param suply_voltage suply_voltage in volt. 
    */
    static float convert_pin_status_to_voltage(unsigned short pin_status, float suply_voltage = 3.5){
    	if(suply_voltage > 3.5) suply_voltage = 3.5;
        return pin_status/(suply_voltage - 0.2f);
    }

   /** 
	 * get_quaternion 
	 * @bref get_quaternion.
	 * @return JY_Type::JY_Quaternion.
	 * @retval .quat0 turning angle in radian(?)
	 * @retval .quat1 vecotr x component
	 * @retval .quat2 vecotr y component
	 * @retval .quat3 vecotr z component
	 * @retval .quat  4 length array that store quaternion.
    */
    JY_Quaternion get_quaternion(void){
    	JY_Quaternion ret;
        char buff[8];
        this->read(0x51, buff, 8);
        ret.quat0 	= ((buff[1]<< 8 ) | buff[0] ) / 32768.0f;
        ret.quat1 	= ((buff[3]<< 8 ) | buff[2] ) / 32768.0f;
        ret.quat2 	= ((buff[5]<< 8 ) | buff[4] ) / 32768.0f;
        ret.quat3	= ((buff[7]<< 8 ) | buff[6] ) / 32768.0f;
        return ret;
    }

protected:

    void save_settings(void){
    	char cmd[2] = {0x00, 0x00};
    	this->write(0x00, cmd, 2);
    }
    int clear_member_var(void){
    	angle_state.x = 0;
    	angle_state.y = 0;
    	angle_state.z = 0;
    	angle_state.temp = 0;

    }
    unsigned short periods[4];
};

#include "jy901-gps.hpp"