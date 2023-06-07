#include <iostream>
#include <string.h>
#include <cstdio>
#define endl '\n';


int codeword_cnt = 0;
int error_codeword_cnt = 0;


using namespace std;

int error_check(char *cw, char *gen);
void CRC_DECODE(FILE *input_fp, FILE *output_fp, FILE *result_fp, int pad_size, char *gen, int dw_size);
int main(int argc, char *argv[])
{
    char *input_file, *output_file, *result_file;
    int dw_size;
    int gen = 0,i;
    int pow = 1, pad_size;

    FILE *input_fp, *output_fp, *result_fp;

    input_file = argv[1]; output_file = argv[2]; result_file = argv[3];
    dw_size = argv[5][0] - 0x30;
    //cout <<dw_size << endl;

    if(argc != 6)
        cout << "usage: ./crc_decoder input_file output_file result_file generator dataword_"<<endl;

    if(dw_size != 4 && dw_size != 8)
        cout << "dataword size must be 4 or 8."<<endl;

    for (i = strlen(argv[4]) - 1; i >= 0; i--)
    {
        gen = gen + pow * (argv[3][i] - 0x30);
        pow *= 2;
    }

    input_fp = fopen(input_file,"rb");
    if(input_fp == NULL)
        cout << "input file open error." << endl;

    output_fp = fopen(output_file, "w");
    if(output_fp == NULL)
        cout << "output file open error." << endl;

    result_fp = fopen(result_file, "w");
    if(result_fp == NULL)
        cout << "result file open error." << endl;

    char p;
    p = fgetc(input_fp);
    
    //printf("%0x\n%lu\n",p,sizeof(p));

    pad_size = p;
  //  cout << pad_size << endl;
    CRC_DECODE(input_fp, output_fp, result_fp, pad_size,argv[4],dw_size);


}
void CRC_DECODE(FILE *input_fp, FILE *output_fp, FILE *result_fp,int pad_size, char *gen, int dw_size)
{
    char ch,ch2, ch_tmp, data[8];
    char *code_word, *encode_data;
    char *tmp;
    int bi = 1;
    int cw_size, div_size;
    int encode_size;
    int i, j;
    int eof_flag = 0;
    int remainder;
    
    div_size = strlen(gen);
    cw_size = dw_size + div_size - 1;
    code_word = new char[cw_size];
    if (dw_size == 4)
    {
        encode_size = cw_size * 2 + pad_size;
        encode_data = new char[encode_size / 8];

        tmp = new char[encode_size];
       // cout <<"\nencode_size is"<< encode_size << endl;
        while(1){
            bi = 1;
            for (i = 0; i < encode_size / 8; i++)
            {
                if (eof_flag == 1)
                {
                    ch = ch2;
                    eof_flag = 0;
                }
                else if (eof_flag == 0)
                    ch = fgetc(input_fp);
                if (ch == EOF)
                {
                   // printf("\n\n!!!%x!!!!\n\n", ch);
                    eof_flag = 1;
                    ch2 = fgetc(input_fp);
                    if (ch2 == EOF)
                    {
                       // printf("\n\n!!!%x!!!!\n\n", ch2);
                        fclose(input_fp);
                        fclose(output_fp);
                        fprintf(result_fp, "%d %d", codeword_cnt, error_codeword_cnt);
                        fclose(result_fp);
                        return;
                    }
                }
                for (j = 7; j >= 0; j--)
                {
                    tmp[8 * i + j] = (ch & bi) + 0x30;
                    ch = ch >> 1;
                }
            }
            // for (i = 0; i < encode_size; i++)
            //     cout << tmp[i];
            int save_val = 0;
            bi = 1;
            for (i = pad_size; i < pad_size + cw_size; i++)
                code_word[i - pad_size] = tmp[i];
            int error_num = error_check(code_word, gen);
            int data_idx =0;

            for(i =0;i<=cw_size-div_size;i++)
            {
                data[i] = code_word[i];
            }
            // for(i = cw_size-div_size ; i>=0;i--)
            // {
            //     save_val = save_val + (bi * (code_word[i]-0x30));
            //     bi*=2;
            // }
           // cout <<"save val"<< save_val << endl;
//data 8 bit 으로 저장ㅇㅇㅇㅇㄹ ㅓㅏ누라ㅓㅇ누라ㅓㄴ우러ㅏ ㄴㅇㄴ
            //fputc((char)save_val, output_fp);
            // save_val = 0;
            bi = 1;
            for (i = pad_size+cw_size; i < pad_size + 2*cw_size; i++)
                code_word[i - pad_size - cw_size] = tmp[i];
            error_num = error_check(code_word, gen);
            for (i = 0; i <= cw_size - div_size; i++)
            {
                data[i+cw_size-div_size+1] = code_word[i];
            }
            for (i = 7; i >= 0; i--)
            {
                save_val = save_val + (bi * (data[i] - 0x30));
                bi *= 2;
            }
           // cout << "save val"<<save_val << endl;

            fputc((char)save_val, output_fp);
        }
    }
    else if(dw_size == 8)
    {   
        encode_size = cw_size + pad_size;
        encode_data = new char[encode_size / 8];
        tmp = new char[encode_size];
        while(1){
            for (i = 0; i < encode_size / 8; i++)
            {
                if(eof_flag == 1){
                    ch = ch2;
                    eof_flag = 0;
                }
                else if(eof_flag == 0)
                    ch = fgetc(input_fp);
                if (ch == EOF)
                {
                    eof_flag = 1;
                    ch2 = fgetc(input_fp);
                    if(ch2 == EOF){
                   //     printf("\n\n!!!%d!!!!\n\n", ch);
                    fclose(input_fp);
                    fclose(output_fp);
                    fprintf(result_fp,"%d %d", codeword_cnt,error_codeword_cnt);
                    fclose(result_fp);
                    return;
                    }
                }
                bi = 1;
                for (j = 7; j >= 0; j--)
                {
                    tmp[8 * i + j] = (ch & bi) + 0x30;
                    ch = ch >> 1;
                }
            }
            // for (i = 0; i < encode_size; i++)
            //     cout << tmp[i];
            int save_val = 0;
            bi = 1;
            for (i = pad_size; i < pad_size + cw_size; i++)
                code_word[i - pad_size] = tmp[i];
            // code_word[0] = '1';
            
            // cout<<endl;
            // cout<< code_word<<endl;

            int error_num = error_check(code_word, gen);
            //cout<<"error_num is "<<error_num<<endl;
            for (i = cw_size - div_size; i >= 0; i--)
            {
                save_val = save_val + (bi * (code_word[i] - 0x30));
                bi *= 2;
            }
         //   cout << "save val" << save_val << endl;

            fputc((char)save_val, output_fp);
        }
    }
}


int error_check(char *cw, char *gen)
{
    char *window;
    int cw_size = strlen(cw);
    int gen_size = strlen(gen);
    window = new char[gen_size];
    //cout << cw_size << ' ' << gen << endl;
    int i, j;
    for (i = 0; i < gen_size; i++)
        window[i] = cw[i];
    for (i = 0; i < cw_size - gen_size + 1; i++)
    {

        if (window[0] == '0')
        {
            // cout << "number 0" << endl;
            for (j = 0; j < gen_size - 1; j++)
            {
                window[j] = window[j + 1];
            }
            if ((i + gen_size) >= cw_size)
                window[j] = 0;
            else
                window[j] = cw[i + gen_size];
        }
        else if (window[0] == '1')
        {
            // cout <<"number 1"<<endl;
            for (j = 0; j < gen_size - 1; j++)
            {
                if (window[j + 1] == gen[j + 1])
                    window[j] = '0';
                else
                    window[j] = '1';
            }
            if ((i + gen_size) >= cw_size)
                window[j] = 0;
            else
                window[j] = cw[i + gen_size];
        }
        // cout<<window<<endl;
    }
    codeword_cnt++;
    
    for (i = 0; i < gen_size - 1; i++)
    {
        if(window[i] != '0'){
            error_codeword_cnt++;
            return -1;
        }
    }
    return 0;
}
// for(int i =0;i<8;i++)
// {   
//     int j = 1;
//     printf("%d",p&1);
//     p= p>>1;
// }
