#include <Wire.h>
#include <stdlib.h>
#define MMA8452_ADRS 0x1D	//アドレス

#define MMA8452_OUT_X_MSB 0x01	//データ呼び出しの先頭レジスタ
#define MMA8452_XYZ_DATA_CFG 0x0E	//XYZ軸のデータの出力設定、加速度の範囲もここで選択
#define MMA8452_CTRL_REG1 0x2A	//
#define MMA8452_CTRL_REG1_ACTV_BIT 0x01
#define MMA8452_G_SCALE 2	//加速度の範囲,2or4or8

const bool me[6][7] = {
  {LOW,        LOW,
   LOW, HIGH,  LOW,
   LOW,        LOW},

  {LOW,        HIGH,
   LOW,  LOW,  LOW,
   HIGH,       LOW},

  {LOW,        HIGH,
   LOW, HIGH,  LOW,
   HIGH,       LOW},

  {HIGH,       HIGH,
   LOW,  LOW,  LOW,
   HIGH,       HIGH},

  {HIGH,       HIGH,
   LOW, HIGH,  LOW,
   HIGH,       HIGH},

  {HIGH,       HIGH,
   HIGH, LOW,  HIGH,
   HIGH,       HIGH}			//ダイスの目の2次元配列 0-6
};
const int DIP[3] = {A1, A2, A3};
const int C[6] = {0, 1, 2, 3, 4, 5};
const int R[7] = {6, 7, 8, 9, 10, 11, 12};	//ピンの設定

void setup() {
	byte tmp;

	//Serial.begin(38400);
	Wire.begin();
	//pinMode(A0, OUTPUT);
	//pinMode(A1, OUTPUT);
	//pinMode(A2, OUTPUT);

	//for (int i = 0; i < 7; ++i)	{
	//	pinMode(i+6, OUTPUT);
	//}

	for (int i = 0; i < 6; ++i) {
		pinMode(C[i], OUTPUT);
		digitalWrite(C[i], LOW);
	}
	for (int i = 0; i < 7; ++i) {
		pinMode(R[i], OUTPUT);
		digitalWrite(R[i], HIGH);
	}
	for (int i = 0; i < 3; ++i)	{
		pinMode(DIP[i], INPUT);
		digitalWrite(DIP[i], HIGH);
	}
	delay(3000);

	tmp = MMA8452_ReadByte(MMA8452_CTRL_REG1);
	MMA8452_WriteByte(MMA8452_CTRL_REG1, tmp & ~(MMA8452_CTRL_REG1_ACTV_BIT));

	MMA8452_WriteByte(MMA8452_XYZ_DATA_CFG, (MMA8452_G_SCALE >> 2));

	tmp = MMA8452_ReadByte(MMA8452_CTRL_REG1);
	MMA8452_WriteByte(MMA8452_CTRL_REG1, tmp | MMA8452_CTRL_REG1_ACTV_BIT);

}

void loop() {
	byte buf[6];
	double g[3];

	MMA8452_ReadByteArray(MMA8452_OUT_X_MSB, 6, buf);	//MMA8452Qからデータを読み込み

	g[0] = -(float((int((buf[0] << 8) | buf[1]) >> 4)) *9.8 / ((1 << 11) / MMA8452_G_SCALE));	//
	g[1] = -(float((int((buf[2] << 8) | buf[3]) >> 4)) *9.8 / ((1 << 11) / MMA8452_G_SCALE));	//
	g[2] = -(float((int((buf[4] << 8) | buf[5]) >> 4)) *9.8 / ((1 << 11) / MMA8452_G_SCALE));	//	加速度(単位m/s^2)を取得

	//Serial.print(g[0], 4);
	//Serial.print("\t");
	//Serial.print(g[1], 4);
	//Serial.print("\t");
	//Serial.print(g[2], 4);
	//Serial.println("\t");
	//for (int i = 0; i < 3; ++i) {
	//	analogWrite(i+14, abs(g[i]));
	//}

	//for(int i=0;i<7;++i) {
	//	digitalWrite(i+6, me[num(g)][i]);
	//}
	//delay(200);


	int a =!digitalRead(A1), b = !digitalRead(A2), c = !digitalRead(A3);
	int dip_num = (a|(b<<1)|(c<<2));	//dipの2進数足し算

	for(int i=0;i<6;++i) {
		digitalWrite(C[i], LOW);	//i面をアクティブに
		for(int j=0;j<7;++j) {
			digitalWrite(R[j], me[dice(dip_num, i, g)][j]);	//j個目のLEDをme配列でON/OFF
		}
		delay(5);
		digitalWrite(C[i], HIGH);	//i面をインアクティブに
	}
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

int dice(int dip, int num, double *g) {
	if(dip == 0 || dip == 7) {
		return num;
	}
	dip--;
	int pos;

	if(g[2] < -5) {			//上面が上
		pos = 0;
	} else if(g[1] < -5) {	//前面が上
		pos = 1;
	} else if(g[0] < -5) {	//左面が上
		pos = 2;
	} else if(g[1] > 5) {	//背面が上
		pos = 3;
	} else if(g[0] > 5) {	//右面が上
		pos = 4;
	} else if(g[2] > 5) {	//底面が上
		pos = 5;
	}
	num = dip - pos + num + 6;

	while(num>5) {
		num -= 6;
	}
	return num;
}