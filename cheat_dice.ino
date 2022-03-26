#include <Wire.h>
#include "MMA8452.h"

int dice(int dip, int num, double *g);

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
   HIGH,       HIGH}			//ダイスの目の2次元配列 1-6
};
const int DIP[3] = {A3, A2, A1};
const int C[6] = {7, 9, 10, 11, 12, 8};
const int R[7] = {0, 1, 2, 3, 4, 5, 6};	//ピンの設定

void setup() {
	//Serial.begin(38400);
	Wire.begin();
	MMA8452_setup();
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
	}
	delay(1000);

	

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



int dice(int dip, int num, double *g) {
	if(dip == 0 || dip == 7) {
		return num;
	}
	dip--;
	int pos = 0;

	if(g[2] < -7) {			//上面が上
		pos = 0;
	} else if(g[1] > 7) {	//前面が上
		pos = 1;
	} else if(g[0] < -7) {	//左面が上
		pos = 2;
	} else if(g[1] < -7) {	//背面が上
		pos = 3;
	} else if(g[0] > 7) {	//右面が上
		pos = 4;
	} else if(g[2] > 7) {	//底面が上
		pos = 5;
	}
	num = dip - pos + num + 6;

	while(num>5) {
		num -= 6;
	}
	return num;
}
