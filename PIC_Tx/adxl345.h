#ifndef ADXL_H
#define	ADXL_H


int readADXLAddr(char);
int initADXL();
int readADXL(unsigned char *, int);

#endif	/* ADXL_H */