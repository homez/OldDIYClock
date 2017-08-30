#include "bmxx80.h"
#include "i2c.h"
#include "delay.h"
#include "sys.h"

bmp180CalData cd;
bmx280CalData cd2;

uint8_t bmxx80_Address = BMXX80_ADDR0;
uint8_t bmxx80Sensor = BMP_NONE;
int16_t temperature = 0;
int16_t pressure = 0;
int16_t humidity = 0;

int32_t	t_fine;
float	ftemp;

void bmxx80write8(uint8_t reg, uint8_t value)
{
	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/);
	I2CswWriteByte(reg);
	I2CswWriteByte(value);
	I2CswStop();

	return;
}

uint8_t bmxx80read8(uint8_t reg)
{
	uint8_t result;

	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/);
	I2CswWriteByte(reg);
	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/ | I2C_READ);
	result = I2CswReadByte(I2C_NOACK);
	I2CswStop();

	return result;
}

uint16_t bmxx80read16(uint8_t reg)
{
	uint16_t result;

	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/);
	I2CswWriteByte(reg);
	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/ | I2C_READ);
	result = ((uint16_t)I2CswReadByte(I2C_ACK)) << 8;
	result |= I2CswReadByte(I2C_NOACK);
	I2CswStop();

	return result;
}

#ifndef _TEST_LE_
uint16_t bmxx80read16_LE(uint8_t reg)
{
	uint16_t result;

	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/);
	I2CswWriteByte(reg);
	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/ | I2C_READ);
	result = I2CswReadByte(I2C_ACK);
	result |= ((uint16_t)I2CswReadByte(I2C_NOACK)) << 8;
	I2CswStop();

	return result;
}
#else

uint16_t bmxx80read16_LE(uint8_t reg)
{
	uint16_t result = bmxx80read16(reg);

	return (result >> 8) | (result << 8);
}
#endif

int16_t bmxx80readS16(uint8_t reg)
{
	return (int16_t)bmxx80read16(reg);
}

int16_t bmxx80readS16_LE(uint8_t reg)
{
	return (int16_t)bmxx80read16_LE(reg);
}

uint32_t bmx280read24(uint8_t reg)
{
	uint32_t result;

	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/);
	I2CswWriteByte(reg);
	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/ | I2C_READ);
	result = ((uint32_t)I2CswReadByte(I2C_ACK)) << 12;
	result |= ((uint32_t)I2CswReadByte(I2C_ACK)) << 4;
	result |= I2CswReadByte(I2C_NOACK) >> 4;
	I2CswStop();

	return result;
}

void bmp180_get_calib_data(void)
{
	cd.ac1 = bmxx80read16(BMP180_REGISTER_DIG_AC1);
	cd.ac2 = bmxx80read16(BMP180_REGISTER_DIG_AC2);
	cd.ac3 = bmxx80read16(BMP180_REGISTER_DIG_AC3);
	cd.ac4 = bmxx80read16(BMP180_REGISTER_DIG_AC4);
	cd.ac5 = bmxx80read16(BMP180_REGISTER_DIG_AC5);
	cd.ac6 = bmxx80read16(BMP180_REGISTER_DIG_AC6);
	cd.b1 = bmxx80read16(BMP180_REGISTER_DIG_B1);
	cd.b2 = bmxx80read16(BMP180_REGISTER_DIG_B2);
	cd.mb = bmxx80read16(BMP180_REGISTER_DIG_MB);
	cd.mc = bmxx80read16(BMP180_REGISTER_DIG_MC);
	cd.md = bmxx80read16(BMP180_REGISTER_DIG_MD);
}

void bmp280_get_calib_data(void)
{
	cd2.dig_T1 = bmxx80read16_LE(BMX280_REGISTER_DIG_T1);
	cd2.dig_T2 = bmxx80readS16_LE(BMX280_REGISTER_DIG_T2);
	cd2.dig_T3 = bmxx80readS16_LE(BMX280_REGISTER_DIG_T3);

	cd2.dig_P1 = bmxx80read16_LE(BMX280_REGISTER_DIG_P1);
	cd2.dig_P2 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P2);
	cd2.dig_P3 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P3);
	cd2.dig_P4 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P4);
	cd2.dig_P5 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P5);
	cd2.dig_P6 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P6);
	cd2.dig_P7 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P7);
	cd2.dig_P8 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P8);
	cd2.dig_P9 = bmxx80readS16_LE(BMX280_REGISTER_DIG_P9);
}

void bme280_get_calib_data(void)
{
	bmp280_get_calib_data();
	cd2.dig_H1 = bmxx80read8(BME280_REGISTER_DIG_H1);
	cd2.dig_H2 = bmxx80readS16_LE(BME280_REGISTER_DIG_H2);
	cd2.dig_H3 = bmxx80read8(BME280_REGISTER_DIG_H3);
	cd2.dig_H4 = (bmxx80read8(BME280_REGISTER_DIG_H4) << 4 ) | (bmxx80read8(BME280_REGISTER_DIG_H4+1) & 0x0F);
	cd2.dig_H5 = (bmxx80read8(BME280_REGISTER_DIG_H5+1) << 4) | (bmxx80read8(BME280_REGISTER_DIG_H5) >> 4);
	cd2.dig_H6 = (int8_t)bmxx80read8(BME280_REGISTER_DIG_H6);
}

void bmp280_set_sampling(void)
{
	bmxx80write8(BME280_REGISTER_CONFIG, 0x00);
	bmxx80write8(BMXX80_REGISTER_CONTROL, 0xAB);
}
void bme280_set_sampling(void)
{
	bmxx80write8(BME280_REGISTER_CONTROLHUMID, 0x05);
	bmp280_set_sampling();
}

uint16_t bmp180GetRawData(uint8_t param)
{
	uint16_t ret;

	I2CswStart(bmxx80_Address/*BMXX80_ADDR*/);
	I2CswWriteByte(BMXX80_REGISTER_CONTROL);
	I2CswWriteByte(param);
	I2CswStop();

	delay_ms(BMP085_CONV_TIME);

	ret = bmxx80read16(BMP180_CONV_REG);

	return ret;
}

bit isReadingCalibration(void)
{
  uint8_t const rStatus = bmxx80read8(BME280_REGISTER_STATUS);

  return (rStatus & (1 << 0)) != 0;
}

void bmxx80Init(void)
{
	bit BMxx80Pres = !I2CswStartACKS(BMXX80_ADDR0);
	I2CswStop();
	if(BMxx80Pres) {
		bmxx80_Address = BMXX80_ADDR0;
	}
	else {
		BMxx80Pres = !I2CswStartACKS(BMXX80_ADDR1);
		I2CswStop();
		if(BMxx80Pres) {
			bmxx80_Address = BMXX80_ADDR1;
		}
	}
	if(BMxx80Pres) {
		bmxx80Sensor = bmxx80read8(BMXX80_REGISTER_CHIPID);
	}
	I2CswStop();
	if( bmxx80Sensor == 0x00 || bmxx80Sensor == 0xFF ) {
		bmxx80Sensor = BMP_NONE;
		return;
	}

	if( bmxx80Sensor != BMP180_CHIP_ID ) {
		bmxx80write8(BMXX80_REGISTER_SOFTRESET, 0xB6);
		
		delay_ms(149);
		delay_ms(149);

		while (isReadingCalibration())
			delay_ms(100);
	}

	switch( bmxx80Sensor ) {
		case BMP180_CHIP_ID: {
			bmp180_get_calib_data();
			break;
		}
		case BMP280_CHIP_ID1:
		case BMP280_CHIP_ID2:
		case BMP280_CHIP_ID3: {
			bmp280_get_calib_data();
			bmp280_set_sampling();
			break;
		}
		case BME280_CHIP_ID: {
			bme280_get_calib_data();
			bme280_set_sampling();
			break;
		}
		default: break;
	}
}

void bmp180convert (void)
{
	int32_t x1, x2, x3, b3, b5, b6, b8, p;
	uint32_t b4, b7;

	uint16_t ut = bmp180GetRawData(BMP180_TEMP_MEASURE);
	uint16_t up = bmp180GetRawData(BMP180_PRESSURE_MEASURE);

	if ( bmxx80Sensor == BMP180_CHIP_ID ) {
		x1 = ((int32_t)ut - cd.ac6) * cd.ac5 >> 15;
		x2 = ((int32_t)cd.mc << 11) / (x1 + cd.md);
		b5 = x1 + x2;

		temperature = (b5 + 8) >> 4;

		b6 = b5 - 4000;
		b8 = (b6 * b6) >> 12;
		x3 = (b8 * cd.b2 + b6 * cd.ac2) >> 11;
		b3 = (x3 +  4 * (int32_t)cd.ac1 + 2) >> 2;
		x1 = (b6 * cd.ac3) >> 13;
		x2 = (b8 * cd.b1) >> 16;
		x3 = ((x1 + x2) + 2) >> 2;
		b4 = (cd.ac4 * (uint32_t)(x3 + 32768)) >> 15;
		b7 = (((uint32_t) up - b3) * (50000));

		p = (b7 / b4) << 1;

		x1 = (p >> 8) * (p >> 8);
		x1 = (x1 * 3038) >> 16;
		x2 = (-7357 * p) >> 16;

		x3 = p + ((x1 + x2 + 3791) >> 4);
		ftemp = (float)x3 / 13.3321995F;
		pressure = (int16_t)ftemp;
	}
}

void bmx280compensate_temperature(void)
{
	int32_t var1, var2;
	uint32_t adc_T = bmx280read24(BMX280_REGISTER_TEMPDATA);
	if (adc_T == 0x80000)
		return;
	
	var1 = ((((adc_T>>3) - ((int32_t)cd2.dig_T1 <<1))) * ((int32_t)cd2.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)cd2.dig_T1)) * ((adc_T>>4) - ((int32_t)cd2.dig_T1))) >> 12) * ((int32_t)cd2.dig_T3)) >> 14;

	t_fine = var1 + var2;
	temperature = (t_fine * 5 + 128) >> 8; // 0.01

	return;
}

void bmx280compensate_pressure(void)
{
	uint32_t adc_P;
	int32_t var1, var2;
	uint32_t press;

	bmx280compensate_temperature(); // must be done first to get t_fine
	adc_P = bmx280read24(BMX280_REGISTER_PRESSUREDATA);
 
	var1 = (t_fine >> 1) - 64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * cd2.dig_P6;
	var2 = var2 + ((var1 * cd2.dig_P5) << 1);
	var2 = (var2 >> 2) + ((int32_t)cd2.dig_P4 << 16);
	var1 = (((cd2.dig_P3 * (((var1 >> 2)*(var1 >> 2)) >> 13)) >> 3) + ((cd2.dig_P2 * var1) >> 1)) >> 18;
	var1 = ((32768 + var1) * cd2.dig_P1) >> 15;
	if (var1 == 0) {
		return /*0*/;
	}
	press = (((1048576 - adc_P) - (var2 >> 12))) * 3125;
	if(press < 0x80000000) {
		press = (press << 1) / var1;
	} else {
		press = (press / var1) * 2;
	}
	var1 = ((int32_t)cd2.dig_P9 * ((int32_t)(((press >> 3) * (press >> 3)) >> 13))) >> 12;
	var2 = (((int32_t)(press >> 2)) * (int32_t)cd2.dig_P8) >> 13;
	press = (press + ((var1 + var2 + cd2.dig_P7) >> 4));
	ftemp = (float)press / 13.3321995F;
	pressure = (int16_t)ftemp;

	return;
}

void bme280compensate_humidity(void)
{
	int32_t adc_H;
	int32_t v_x1_u32r;
	bmx280compensate_temperature(); // must be done first to get t_fine
	adc_H = bmxx80read16(BME280_REGISTER_HUMIDDATA);
	if (adc_H == 0x8000) // value in case humidity measurement was disabled
		return;
	v_x1_u32r = (t_fine - ((int32_t)76800));

	v_x1_u32r = (((((adc_H << 14) - (((int32_t)cd2.dig_H4) << 20) -
			(((int32_t)cd2.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
			(((((((v_x1_u32r * ((int32_t)cd2.dig_H6)) >> 10) *
			(((v_x1_u32r * ((int32_t)cd2.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
			((int32_t)2097152)) * ((int32_t)cd2.dig_H2) + 8192) >> 14));

	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			((int32_t)cd2.dig_H1)) >> 4));

	v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	v_x1_u32r >>= 12;
	ftemp = (float)v_x1_u32r/10.24F;
	humidity = (int16_t)ftemp;

	return;
}

void bmp280convert (void)
{
	bmx280compensate_pressure();

	return;
}

void bme280convert (void)
{
	bmx280compensate_pressure();
	bme280compensate_humidity();

	return;
}

void bmxx80Convert(void)
{
	switch( bmxx80Sensor ) {
		case BMP180_CHIP_ID: {
			bmp180convert();
			break;
		}
		case BMP280_CHIP_ID1:
		case BMP280_CHIP_ID2:
		case BMP280_CHIP_ID3: {
			bmp280convert();
			break;
		}
		case BME280_CHIP_ID: {
			bme280convert();
			break;
		}
		default: break;
	}

	return;
}

int16_t bmxx80GetTemp(void)
{
	return temperature;
}

int16_t bmxx80GetPressure(void)
{
	return pressure;
}

int16_t bme280GetHumidity(void)
{
	return humidity;
}

uint8_t bmxx80HaveSensor(void)
{
	return bmxx80Sensor;
}

/*

    uint16_t    dig_T1;
    int16_t     dig_T2, dig_T3;
    uint16_t    dig_P1;
    int16_t     dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint16_t    dig_H1, dig_H3;
    int16_t     dig_H2, dig_H4, dig_H5, dig_H6;


 
float BMP280::getTemperature()
{
    uint32_t temp_raw;
    float tempf;
    char cmd[4];
 
    cmd[0] = 0xfa; // temp_msb
    i2c.write(address, cmd, 1);
    i2c.read(address, &cmd[1], 3);
 
    temp_raw = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);
 
    int32_t temp;
 
    temp =
        (((((temp_raw >> 3) - (dig_T1 << 1))) * dig_T2) >> 11) +
        ((((((temp_raw >> 4) - dig_T1) * ((temp_raw >> 4) - dig_T1)) >> 12) * dig_T3) >> 14);
 
    t_fine = temp;
    temp = (temp * 5 + 128) >> 8;
    tempf = (float)temp;
 
    return (tempf/100.0f);
}
 
float BMP280::getPressure()
{
    uint32_t press_raw;
    float pressf;
    char cmd[4];
 
    cmd[0] = 0xf7; // press_msb
    i2c.write(address, cmd, 1);
    i2c.read(address, &cmd[1], 3);
 
    press_raw = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);
 
    int32_t var1, var2;
    uint32_t press;
 
    var1 = (t_fine >> 1) - 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * dig_P6;
    var2 = var2 + ((var1 * dig_P5) << 1);
    var2 = (var2 >> 2) + (dig_P4 << 16);
    var1 = (((dig_P3 * (((var1 >> 2)*(var1 >> 2)) >> 13)) >> 3) + ((dig_P2 * var1) >> 1)) >> 18;
    var1 = ((32768 + var1) * dig_P1) >> 15;
    if (var1 == 0) {
        return 0;
    }
    press = (((1048576 - press_raw) - (var2 >> 12))) * 3125;
    if(press < 0x80000000) {
        press = (press << 1) / var1;
    } else {
        press = (press / var1) * 2;
    }
    var1 = ((int32_t)dig_P9 * ((int32_t)(((press >> 3) * (press >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(press >> 2)) * (int32_t)dig_P8) >> 13;
    press = (press + ((var1 + var2 + dig_P7) >> 4));
 
    pressf = (float)press;
    return (pressf/100.0f);
}

static int32_t compensate_temperature(const struct bme280_uncomp_data *uncomp_data,
						struct bme280_calib_data *calib_data)
{
	int32_t var1;
	int32_t var2;
	int32_t temperature;
	int32_t temperature_min = -4000;
	int32_t temperature_max = 8500;

	var1 = (int32_t)((uncomp_data->temperature / 8) - ((int32_t)calib_data->dig_T1 * 2));
	var1 = (var1 * ((int32_t)calib_data->dig_T2)) / 2048;
	var2 = (int32_t)((uncomp_data->temperature / 16) - ((int32_t)calib_data->dig_T1));
	var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data->dig_T3)) / 16384;
	calib_data->t_fine = var1 + var2;
	temperature = (calib_data->t_fine * 5 + 128) / 256;

	if (temperature < temperature_min)
		temperature = temperature_min;
	else if (temperature > temperature_max)
		temperature = temperature_max;

	return temperature;
}

static uint32_t bme280compensate_pressure(const struct bme280_uncomp_data *uncomp_data,
						const struct bme280_calib_data *calib_data)
{
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	uint32_t var5;
	uint32_t pressure;
	uint32_t pressure_min = 30000;
	uint32_t pressure_max = 110000;

	var1 = (((int32_t)calib_data->t_fine) / 2) - (int32_t)64000;
	var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)calib_data->dig_P6);
	var2 = var2 + ((var1 * ((int32_t)calib_data->dig_P5)) * 2);
	var2 = (var2 / 4) + (((int32_t)calib_data->dig_P4) * 65536);
	var3 = (calib_data->dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
	var4 = (((int32_t)calib_data->dig_P2) * var1) / 2;
	var1 = (var3 + var4) / 262144;
	var1 = (((32768 + var1)) * ((int32_t)calib_data->dig_P1)) / 32768;
	 //* avoid exception caused by division by zero /
	if (var1) {
		var5 = (uint32_t)((uint32_t)1048576) - uncomp_data->pressure;
		pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;
		if (pressure < 0x80000000)
			pressure = (pressure << 1) / ((uint32_t)var1);
		else
			pressure = (pressure / (uint32_t)var1) * 2;

		var1 = (((int32_t)calib_data->dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
		var2 = (((int32_t)(pressure / 4)) * ((int32_t)calib_data->dig_P8)) / 8192;
		pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + calib_data->dig_P7) / 16));

		if (pressure < pressure_min)
			pressure = pressure_min;
		else if (pressure > pressure_max)
			pressure = pressure_max;
	} else {
		pressure = pressure_min;
	}

	return pressure;
}

static uint32_t bme280compensate_humidity(const struct bme280_uncomp_data *uncomp_data,
						const struct bme280_calib_data *calib_data)
{
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	uint32_t humidity;
	uint32_t humidity_max = 100000;

	var1 = calib_data->t_fine - ((int32_t)76800);
	var2 = (int32_t)(uncomp_data->humidity * 16384);
	var3 = (int32_t)(((int32_t)calib_data->dig_H4) * 1048576);
	var4 = ((int32_t)calib_data->dig_H5) * var1;
	var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
	var2 = (var1 * ((int32_t)calib_data->dig_H6)) / 1024;
	var3 = (var1 * ((int32_t)calib_data->dig_H3)) / 2048;
	var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
	var2 = ((var4 * ((int32_t)calib_data->dig_H2)) + 8192) / 16384;
	var3 = var5 * var2;
	var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
	var5 = var3 - ((var4 * ((int32_t)calib_data->dig_H1)) / 16);
	var5 = (var5 < 0 ? 0 : var5);
	var5 = (var5 > 419430400 ? 419430400 : var5);
	humidity = (uint32_t)(var5 / 4096);

	if (humidity > humidity_max)
		humidity = humidity_max;

	return humidity;
}


*/