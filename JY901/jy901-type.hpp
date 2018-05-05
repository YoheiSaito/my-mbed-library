#pragma once
namespace JY901_Type{
    extern "C"{
        typedef struct{
            float roll;
            float pitch;
            float yow;
        } JY_Pitch_Angle;
        typedef struct{
            float pressure;
            float height;
        } JY_Pressure_Height;
        typedef struct{
            float longitude;
            float latitude;
        } JY_Geographical_Position;

        typedef struct{
            short year;
            char month;
            char day;
            char hour;
            char min;
            char sec;
            short ms;
        } JY_Time;

        typedef struct{
            float x, y, z, temp;
        } JY_Dim_3D;

        typedef struct{
            union{
                struct{
                    short P0, P1, P2, P3;
                };
                short P[4];
            };
        } JY_Pin_Status;

        typedef struct{
            struct{
                    float quat0, quat1, quat2, quat3;
            };
            float quat[4];
        } JY_Quaternion;

    }
    typedef enum{
        RATE_HZ01 = 0x01,
        RATE_HZ05 = 0x02,
        RATE_HZ1 = 0x03,
        RATE_HZ2 = 0x04,
        RATE_HZ5 = 0x05,
        RATE_HZ10 = 0x06,
        RATE_HZ20 = 0x07,
        RATE_HZ50 = 0x08,
        RATE_HZ100 = 0x09,
        RATE_HZ200 = 0x0a,
        RATE_SINGLE = 0x0b,
        RATE_NOT_OUTPUT = 0x0c,
        RATE_DEFAULT = 0x06
    } JY_Sampling_Rate;
    typedef enum{
        GPS_2400    = 0x00,
        GPS_4800    = 0x01,
        GPS_9600    = 0x02,
        GPS_19200   = 0x03,
        GPS_38400   = 0x04,
        GPS_57600   = 0x05,
        GPS_115200  = 0x06,
        GPS_230400  = 0x07,
        GPS_460800  = 0x08,
        GPS_921600  = 0x09,
        GPS_DEFAULT = 0x02,
        GPS_NO_DEVICE = 0xFF
    } JY_Serial_Baud;

    typedef enum{
        JY_ANALOG = 0x00,
        JY_DIGITAL_IN,
        JY_DIGITAL_OUTPUT_H,
        JY_DIGITAL_OUTPUT_L,
        JY_PWM_OUT ,
        JY_PIN_MODE_DEFAULT = 0x00
    } JY_Pin_Mode;

}