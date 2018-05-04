#pragma once
#include "my-i2c.hpp"
#include "jy901-type.hpp"
static const char JY_ADDR                = 0x50;
static const char JY_901_DEFAULT_CONTENT = 0x8F;


using namespace JY901_Type;
class JY901 
    :public MyI2C
{
public:
    JY901(I2C *bus): MyI2C(bus, JY_ADDR) { have_GPS = false; }
    JY901(I2C *bus,  char address): MyI2C(bus, address) { have_GPS = false; }
    
    /** ---------------

        Setter methods

    -------------------*/
    
    // setter about carlibration
    void enter_gyroscope_carlibration(void){
        this->write(0x01, 1);
    }
    void enter_magnetic_carlibration(void){
        this->write(0x01, 2);
    }
    void exit_carlibration(void){
        this->write(0x01, 0);
    }
    void reset_carlibration(void){
        this->write(0x01, 3);
    }
    
    // setter about sample speed 
    void set_return_rate(JY_Sampling_Rate rate){
        this->write(0x03, (char)rate);
    }
    
    void set_GPS_baud(JY_GPS_Baud gps_baud){
        if(gps_baud > 9){
            have_GPS = false;

            this->write(0x0f, 0x05);
            return;
        }
        
        have_GPS = true;

        this->write(0x0f, 0x05);
        this->write(0x1C, (char)gps_baud);
    }

    
    // setter about ic configure
    void set_led(bool state){
        if(state){
            this->write(0x1b, 0x00);
        }else{
            this->write(0x1b, 0x01);
        }
    }
    void set_new_i2c_address(char new_address){
        this->write(0x1a, new_address);
        this->set_address(new_address);
    }

   
    // setter about io pin 
    void set_pin_mode (int pin_ID, JY_Pin_Mode pin_mode){
        if(pin_ID > 3) return;
        if(pin_ID == 1) have_GPS = false;
        this->write( (char)(0x0e + pin_ID), (char)pin_mode);
    }
    void set_pin_write(int pin_ID, int state){
        if(pin_ID > 3) return;
        if(pin_ID == 1) have_GPS = false;
        this->write( (char)(0x0e + pin_ID), (char)(0x10|(state&1)));
    }
    void set_pwm_width (int pin_ID, unsigned short duty_rate){
        char dum[2];
        if(pin_ID > 3) return;

        dum[0] = duty_rate & 0x00FF;
        dum[1] = duty_rate >> 8;
        this->write( (char)(0x12 + pin_ID), dum, 2);
    }

    void set_pwm_period (int pin_ID, unsigned short period){
        char dum[2];
        if(pin_ID > 3) return;

        dum[0] = period & 0x00FF;
        dum[1] = period >> 8;
        this->write( (char)(0x16 + pin_ID), dum, 2);
    }

    /** ---------------------
    
        getter methods
    
    -------------------------*/

    JY_Pitting_Angle get_pitting_angle(void){

        char buff[6];
        this->read(0x3D, buff, 6);
        angle_state.roll     = char_array_to_int(&buff[0]) / 32768.0f * 180;
        angle_state.pitch    = char_array_to_int(&buff[2]) / 32768.0f * 180;
        angle_state.yow      = char_array_to_int(&buff[4]) / 32768.0f * 180;
        return angle_state;
    }
    
    void get_pitting_angle(float *roll, float *pitch, float *yow){
        char buff[6];
        this->read(0x3D, buff, 6);
        angle_state.roll     = char_array_to_int(&buff[0]) / 32768.0f * 180;
        angle_state.pitch    = char_array_to_int(&buff[2]) / 32768.0f * 180;
        angle_state.yow      = char_array_to_int(&buff[4]) / 32768.0f * 180;
       
        *roll     = angle_state.roll;
        *pitch    = angle_state.pitch;
        *yow      = angle_state.yow;
    }

    float get_roll(void){
        char buff[2];
        this->read(0x3D, buff, 2);
        return char_array_to_int(buff) / 32768.0f * 180;
    }
    float get_pitch(void){
        char buff[2];
        this->read(0x3E, buff, 2);
        return char_array_to_int(buff) / 32768.0f * 180;
    }
    float get_yow(void){
        char buff[2];
        this->read(0x3F, buff, 2);
        return char_array_to_int(buff) / 32768.0f * 180;
    }


    JY_Pressure_Height get_pressure_height(void){
        char buff[9];
        this->read(0x56, buff, 9);
        
        height_state.pressure = ( buff[3]<<24)| ( buff[2]<<16)| ( buff[1]<<8)| buff[0]; 
        height_state.height   = ( buff[7]<<24)| ( buff[6]<<16)| ( buff[5]<<8)| buff[4]; 
        return height_state;  
    }
    

    JY_Geographical_Position get_geographical_position(void){
        char buff[9];
        if(!have_GPS) return position_state;
        this->read(0x57, buff, 9);

        position_state.longitude = ( buff[3]<<24)| ( buff[2]<<16)| ( buff[1]<<8)| buff[0]; 
        position_state.latitude   = ( buff[7]<<24)| ( buff[6]<<16)| ( buff[5]<<8)| buff[4]; 
        return position_state;  
    }

    JY_Time get_time(void){
        JY_Time ret ;
        char buff[9];
        this->read(0x50, buff, 9);
        ret.year = 2000 + buff[0];
        ret.month = buff[1];
        ret.day = buff[2];
        ret.hour = buff[3];
        ret.min = buff[4];
        ret.sec = buff[5];
        ret.milli_sec = (buff[7] << 8) | buff[6];
        return ret;
    }
    JY_Dim_3D get_Acceleration(void){
        JY_Dim_3D ret;
        char buff[8];
        this->read(0x51, buff, 8);
        ret.x = char_array_to_int(&buff[0]) / 32768.0f * 16;
        ret.y = char_array_to_int(&buff[2]) / 32768.0f * 16;
        ret.z = char_array_to_int(&buff[4]) / 32768.0f * 16;
        ret.temp = char_array_to_int(&buff[6]) /100.0f;
        return ret;
    }
    JY_Dim_3D get_Angular_valocity(void){
        JY_Dim_3D ret;
        char buff[8];
        this->read(0x52, buff, 8);
        ret.x = char_array_to_int(&buff[0]) / 32768.0f * 16;
        ret.y = char_array_to_int(&buff[2]) / 32768.0f * 16;
        ret.z = char_array_to_int(&buff[4]) / 32768.0f * 16;
        ret.temp = char_array_to_int(&buff[6]) /100.0f;
        return ret;
    }
    JY_Dim_3D get_Magnetic_output(void){
        JY_Dim_3D ret;
        char buff[8];
        this->read(0x54, buff, 8);
        ret.x = char_array_to_int(&buff[0]) / 32768.0f * 16;
        ret.y = char_array_to_int(&buff[2]) / 32768.0f * 16;
        ret.z = char_array_to_int(&buff[4]) / 32768.0f * 16;
        ret.temp = char_array_to_int(&buff[6]) /100.0f;
        return ret;
    }
    JY_Pin_Status get_pin_status(void){
        JY_Pin_Status ret;
        char buff[8];
        this->read(0x55, buff, 8);
        ret.P0 = char_array_to_int(&buff[0]);
        ret.P1 = char_array_to_int(&buff[2]);
        ret.P2 = char_array_to_int(&buff[4]);
        ret.P3 = char_array_to_int(&buff[6]);
        return ret;
    }
    float convert_pinstatus_to_voltage(short pin_status, float suply_voltage = 3.5){
        return pin_status/(suply_voltage - 0.2f);
    }
    JY_Quaternion get_quaternion(void){
    	JY_Quaternion ret;
        char buff[8];
        this->read(0x59, buff, 8);
        ret.quat0 = char_array_to_int(&buff[0]) / 32768.0f;
        ret.quat1 = char_array_to_int(&buff[2]) / 32768.0f;
        ret.quat2 = char_array_to_int(&buff[4]) / 32768.0f;
        ret.quat3 = char_array_to_int(&buff[6]) / 32768.0f;
        return ret;
    }
private:

    int char_array_to_int(char get_data[]) {
        return((get_data[1]<< 8 ) | get_data[0] );
    }
    JY_Pitting_Angle angle_state;
    JY_Pressure_Height height_state;
    JY_Geographical_Position position_state;
    bool have_GPS;
};