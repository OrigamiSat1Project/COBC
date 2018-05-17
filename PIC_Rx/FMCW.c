
#include <xc.h>
#include "FMCW.h"

/* Do not change!! */
#define FMTX_   1
#define CWTX_   2
#define FMRX_   3

#include "time.h"

/*
 * ãFMCWè¨­å®ã?®åæåã??
 *  FMåä¿¡?¼FMéä¿¡?¼CWéä¿¡ãããããè¨­å®?
 *  1. CLKç«¯å­ï¼ã¯ã­ã?ã¯?¼ï¼DATç«¯å­ï¼ãã¼ã¿?¼ï¼STBç«¯å­ï¼ã¹ãã­ã¼ãï¼ãåºåã¨ãã¦ä½¿ç¨
 *  2. å¨ã¦ã®ãã?¼ããLowã«ãã
 */
void Init_FMCW(void){
    /* Initialize (turn ports Low) */
    FMRX_CLK = 0;
    FMRX_DAT = 0;
    FMRX_STB = 0;
    FMTX_CLK = 0;
    FMTX_DAT = 0;
    FMTX_STB = 0;
    FMTX_PTT = 0;
    CWRX_CLK = 0;
    CWTX_DAT = 0;
    CWTX_STB = 0;
    CWTX_KEY = 0;
}


/*
 * ãç¡ç·æ©ã«'Low'ãé?ãã?
 *  1. ã©ã®ç¡ç·æ©ã«éããé¸æï¼?FMTX or FMRX or CWTX?¼?
 *  2. DATç«¯å­ãLowã«ãã
 *  3. CLKç«¯å­ã0â?1â?0ã¨å¤åããã?
 */
void L_OUT(int fmcwtxrx){
    if(fmcwtxrx == FMTX_)
    {
        FMTX_DAT = 0;
        _NOP();
        FMTX_CLK = 1;
        _NOP();
        FMTX_CLK = 0;
    }
    if(fmcwtxrx == CWTX_)
    {
        CWTX_DAT = 0;
        _NOP();
        CWRX_CLK = 1;
        _NOP();
        CWRX_CLK = 0;
    }
    if(fmcwtxrx == FMRX_)
    {
        FMRX_DAT = 0;
        _NOP();
        FMRX_CLK = 1;
        _NOP();
        FMRX_CLK = 0;
    }
}


/*
 * ãç¡ç·æ©ã«'High'ãé?ãã?
 *  1. ã©ã®ç¡ç·æ©ã«éããé¸æï¼?FMTX or FMRX or CWTX?¼?
 *  2. DATç«¯å­ãHighã«ãã
 *  3. CLKç«¯å­ã0â?1â?0ã¨å¤åããã? 
 */
void H_OUT(int fmcwtxrx){
    if(fmcwtxrx == FMTX_)
    {
        FMTX_DAT = 1;
        _NOP();
        FMTX_CLK = 1;
        _NOP();
        FMTX_CLK = 0;
    }
    if(fmcwtxrx == CWTX_)
    {
        CWTX_DAT = 1;
        _NOP();
        CWRX_CLK = 1;
        _NOP();
        CWRX_CLK = 0;
    }
    if(fmcwtxrx == FMRX_)
    {
        FMRX_DAT = 1;
        _NOP();
        FMRX_CLK = 1;
        _NOP();
        FMRX_CLK = 0;
    }
}


/*
 * ãç¡ç·æ©ã«STBä¿¡å·ãé?ãã?
 *  1. ã©ã®ç¡ç·æ©ã«éããé¸æï¼?FMTX or FMRX or CWTX?¼?
 *  2. STBç«¯å­ã0â?1â?0ã¨å¤åããã?
 */
void STBOUT(int fmcwtxrx){
    if(fmcwtxrx == FMTX_)
    {
        FMTX_STB = 1;
        _NOP();
        FMTX_STB = 0;
    }
    if(fmcwtxrx == CWTX_)
    {
        CWTX_STB = 1;
        _NOP();
        CWTX_STB = 0;
    }
    if(fmcwtxrx == FMRX_)
    {
        FMRX_STB = 1;
        _NOP();
        FMRX_STB = 0;
    }
}


/*
 * ãç¡ç·æ©ã?®ãã­ã°ã©ããã«ã«ã¦ã³ã¿ãè¨­å®ããã??
 *  1. å¼æ°ããèª­ã¿è¾¼ãã ãã­ã°ã©ããã«ã«ã¦ã³ã¿ã?2é²æ°ã«å¤æ?¼é?å?ã¨ãã¦æ ¼ç´ï¼?
 *  2. æ ¼ç´ãã?2é²æ°ã«åããã¦HighãLowãç¡ç·æ©ã«éã?¼è¨­å®ã?®èï¼?
 *  3. ã°ã«ã¼ãã³ã¼ããéã'10'
 *  4. STBä¿¡å·ãé?ã
 */
void OUTFQ(int fmcwtxrx, int *Nprg){
    int count = 0;
    int Nprg_b[17];
    
    for(int i=0; i<17; i++){
        Nprg_b[i] = 0;
    }
    
    //Nprg transforms decimal to binary
    for(int i = 0; i < 17; i++){
        for(int j = 0; j<5; j++){
            if(Nprg[j] % 2 == 0) {
                if(j == 4){
                    Nprg[j] = Nprg[j] / 2;
                    Nprg_b[count] = 0;
                    count++;
                }
                else{
                    Nprg[j] = Nprg[j] / 2;
                }
            }
            else if(Nprg[j] % 2 == 1) {
                if(j == 4){
                    Nprg[j] = Nprg[j] / 2;
                    Nprg_b[count] = 1;
                    count++;
                }
                else{
                    Nprg[j] = Nprg[j] / 2;
                    Nprg[j+1] = Nprg[j+1] + 10;
                }
            }
        }
    }
    
    //Send Nprg data(binay) to communication module
    for (int i=0; i<17; i++)
    {
        if(Nprg_b[i] == 0)
        {
            L_OUT(fmcwtxrx);
        }
        if(Nprg_b[i] == 1)
        {
            H_OUT(fmcwtxrx);
        }
    }
    
    //GroupCode'10' is TX.DEV(?)
    H_OUT(fmcwtxrx);
    L_OUT(fmcwtxrx);
    
    //STB Signal
    STBOUT(fmcwtxrx);
}


/*
 * ãç¡ç·æ©ã?®ãªãã¡ã¬ã³ã¹ã«ã¦ã³ã¿ãè¨­å®ããã??
 *  1. å¼æ°ããèª­ã¿è¾¼ãã ãªãã¡ã¬ã³ã¹ã«ã¦ã³ã¿ã?2é²æ°ã«å¤æ?¼é?å?ã¨ãã¦æ ¼ç´ï¼?
 *  2. æ ¼ç´ãã?2é²æ°ã«åããã¦HighãLowãç¡ç·æ©ã«éã?¼è¨­å®ã?®èï¼?
 *  3. ã°ã«ã¼ãã³ã¼ããéã'11'
 *  4. STBä¿¡å·ãé?ã
 */
void RFDOUT(int fmcwtxrx, int Nref){
    int Nref_b[12];
    
    for(int i=0; i<12; i++){
        Nref_b[i] = 0;
    }
    
    //Nref transforms decimal to binary
    for(int i=0; Nref>0; i++){
        Nref_b[i] = Nref % 2;
        Nref = Nref / 2;
    }
    
    //Send Nref data(binay) to communication module
    for (int i=0; i<12; i++)
    {
        if(Nref_b[i] == 0)
        {
            L_OUT(fmcwtxrx);
        }
        if(Nref_b[i] == 1)
        {
            H_OUT(fmcwtxrx);
        }
    }
    
    //GroupCode'11' is REF.DEV
    H_OUT(fmcwtxrx);
    H_OUT(fmcwtxrx);
    
    //STB Signal
    STBOUT(fmcwtxrx);
}


/*
 * ãç¡ç·æ©ã?®ãªãã·ã§ã³ã¬ã¸ã¹ã¿ãè¨­å®ããï¼å?±éPLLè¨­å®ï¼ã??
 *  1. (T1, T2, T3, CpT1, CpT2, Cpr1, Cpr2, LD1, LD2, Tx, Rx) = (0,0,0,1,1,0,0,0,0,0,1)ãé?ã
 *  2. ã°ã«ã¼ãã³ã¼ããéã'00'
 *  3. STBä¿¡å·ãé?ã
 */
void OPINIT(int fmcwtxrx){
    //Send PLL Common DataSet to communiction module
    L_OUT(fmcwtxrx);//T1
    L_OUT(fmcwtxrx);//T2
    L_OUT(fmcwtxrx);//T3
    H_OUT(fmcwtxrx);//CpT1
    H_OUT(fmcwtxrx);//CpT2
    L_OUT(fmcwtxrx);//Cpr1
    L_OUT(fmcwtxrx);//Cpr2
    L_OUT(fmcwtxrx);//LD1
    L_OUT(fmcwtxrx);//LD2
    L_OUT(fmcwtxrx);//Tx
    H_OUT(fmcwtxrx);//Rx
    
    //GroupCode'00' is option reg.
    L_OUT(fmcwtxrx);
    L_OUT(fmcwtxrx);
    
    //STB Signal
    STBOUT(fmcwtxrx);
}


/*
 * ãFMTXã®PLLè¨­å®ãè¡ãã?
 *  1. ãªãã·ã§ã³ã¬ã¸ã¹ã¿ã®è¨­å®?
 *  2. ãªãã¡ã¬ã³ã¹ã«ã¦ã³ã¿ã®è¨­å®?
 *  3. ãã­ã°ã©ããã«ã«ã¦ã³ã¿ã®è¨­å®?
 */
void FMTX(int Nref, int *Nprg){
    int fmtx = FMTX_;
    OPINIT(fmtx);
    RFDOUT(fmtx, Nref);
    OUTFQ(fmtx, Nprg);
}


/*
 * ãCWTXã®PLLè¨­å®ãè¡ãã?
 *  1. ãªãã·ã§ã³ã¬ã¸ã¹ã¿ã®è¨­å®?
 *  2. ãªãã¡ã¬ã³ã¹ã«ã¦ã³ã¿ã®è¨­å®?
 *  3. ãã­ã°ã©ããã«ã«ã¦ã³ã¿ã®è¨­å®?
 */
void CWTX(int Nref, int *Nprg){
    int cwtx = CWTX_;
    OPINIT(cwtx);
    RFDOUT(cwtx, Nref);
    OUTFQ(cwtx, Nprg);
}


/*
 * ãFMRXã®PLLè¨­å®ãè¡ãã?
 *  1. ãªãã·ã§ã³ã¬ã¸ã¹ã¿ã®è¨­å®?
 *  2. ãªãã¡ã¬ã³ã¹ã«ã¦ã³ã¿ã®è¨­å®?
 *  3. ãã­ã°ã©ããã«ã«ã¦ã³ã¿ã®è¨­å®?
 */
void FMRX(int Nref, int *Nprg){
    int fmrx = FMRX_;
    OPINIT(fmrx);
    RFDOUT(fmrx, Nref);
    OUTFQ(fmrx, Nprg);
}


/*
 * ãPLLè¨­å®ãè¡ãã?
 */
void SetPLL(int FMTX_Nref, int FMTX_Nprg, int CWTX_Nref, int CWTX_Nprg, int FMRX_Nref, int FMRX_Nprg){
    FMTX(FMTX_Nref, FMTX_Nprg);
    CWTX(CWTX_Nref, CWTX_Nprg);
    FMRX(FMRX_Nref, FMRX_Nprg);
}



/*
 * ãã¢ã¼ã«ã¹ä¿¡å·ã®'V'ãé?ãã?
 *  1. CWKEYç«¯å­ã0â?1â?0ã¨å¤åããã?
 *  2. â»1.ãè¨ï¼åè¡ã
 */
void Morse_V(void){
    CWTX_KEY = 1;
    __delay_ms(50);
    CWTX_KEY = 0;
    __delay_ms(50);

    CWTX_KEY = 1;
    __delay_ms(50);
    CWTX_KEY = 0;
    __delay_ms(50);

    CWTX_KEY = 1;
    __delay_ms(50);
    CWTX_KEY = 0;
    __delay_ms(50);

    CWTX_KEY = 1;
    __delay_ms(150);
    CWTX_KEY = 0;
    __delay_ms(50);
}


/*
 * ãä½ãå¦ç?ãè¡ããªã?¼å¾?æ©ï¼ã??
 *  5å¦ç?å?å¾?æ©ãã?
 */
void _NOP(void) {
    for(int i=0; i<5; i++){
        NOP();
    }
}
