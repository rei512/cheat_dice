#include <Wire.h>

#define MMA8452_ADRS 0x1D	//アドレス

#define MMA8452_OUT_X_MSB 0x01	//データ呼び出しの先頭レジスタ
#define MMA8452_XYZ_DATA_CFG 0x0E	//XYZ軸のデータの出力設定、加速度の範囲もここで選択
#define MMA8452_CTRL_REG1 0x2A	//
#define MMA8452_CTRL_REG1_ACTV_BIT 0x01
#define MMA8452_G_SCALE 2	//加速度の範囲,2or4or8

void MMA8452_setup() {
	byte tmp;
	Wire.begin();

	tmp = MMA8452_ReadByte(MMA8452_CTRL_REG1);
	MMA8452_WriteByte(MMA8452_CTRL_REG1, tmp & ~(MMA8452_CTRL_REG1_ACTV_BIT));

	MMA8452_WriteByte(MMA8452_XYZ_DATA_CFG, (MMA8452_G_SCALE >> 2));

	tmp = MMA8452_ReadByte(MMA8452_CTRL_REG1);
	MMA8452_WriteByte(MMA8452_CTRL_REG1, tmp | MMA8452_CTRL_REG1_ACTV_BIT);
}



void MMA8452_ReadByteArray(byte adrs, int datlen, byte *dest){
	Wire.beginTransmission(MMA8452_ADRS);
	Wire.write(adrs);
	Wire.endTransmission(false);

	Wire.requestFrom(MMA8452_ADRS, datlen);

	//while(Wire.available() < datlen);

	for(int x = 0; x < datlen; x++){
	dest[x] = Wire.read();
	}
}

byte MMA8452_ReadByte(byte adrs){
	Wire.beginTransmission(MMA8452_ADRS);
	Wire.write(adrs);
	Wire.endTransmission(false);

	Wire.requestFrom(MMA8452_ADRS, 1);

	//while(!Wire.available());
	return(Wire.read());
}

void MMA8452_WriteByte(byte adrs, byte dat){
	Wire.beginTransmission(MMA8452_ADRS);
	Wire.write(adrs);
	Wire.write(dat);
	Wire.endTransmission();
}