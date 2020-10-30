#include <xc.h>
#include "EPS.h"
#include "time.h"

/*
 * �yEPS���Z�b�g�@�\�̏������z
 *  1. EPS���Z�b�g�X�C�b�`�̒[�q��'�o��'�ɐݒ�
 *  2. EPS���Z�b�g�X�C�b�`�̃|�[�g��'LOW'�ɂ���
 */

/*
 * �yEPS���Z�b�g�X�C�b�`�����z
 *  1. EPS���Z�b�g�X�C�b�`�̒[�q��0��1��0�ƕω�������
 */
void Reset_EPS(void){
    SEP_SW = 1;
    RBF_SW = 1;
    __delay_ms(5000);
    SEP_SW = 0;
    RBF_SW = 0;
    __delay_ms(5000);
}

