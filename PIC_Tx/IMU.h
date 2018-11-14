#ifndef IMU_H
#define	IMU_H

UWORD sampling_counter;
UBYTE sampling_counterH;
UBYTE sampling_counterL;

void readIMUsequence_ICM(unsigned char, unsigned char, unsigned char, int);
void readIMUsequence_adxl_ITG(unsigned char, unsigned char, unsigned char, int);


#endif	/* ADXL_H */