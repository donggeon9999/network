#include <iostream>
#include <cstdio>

#include <string.h>
#define endl '\n';

using namespace std;
void CRC_ENCODE(FILE *input_fp, FILE *output_fp, char *gen, int dw_size);
void make_cw(char *cw, char *gen);
int main(int argc, char *argv[])
{
    char *input_file, *output_file;
    int gen = 0, dw_size;
    int i, pow = 1;
    int div_size ;
    FILE *input_fp, *output_fp;
    if(argc > 5)
    {
        cout << "usage: ./crc_encoder input_file output_file generator dataword_size"<<endl;
    }
    
    input_file = argv[1];
    output_file = argv[2];
    dw_size = argv[4][0] - 0x30;
    div_size = strlen(argv[3]);
    for (i = strlen(argv[3]) - 1; i >= 0; i--)
    {
        gen = gen + pow * (argv[3][i] - 0x30);
        pow *= 2;
    }
    //argument input

    input_fp = fopen(input_file, "rb");
    if (input_fp == NULL)
        cout << "input file open error."<<endl;
    output_fp = fopen(output_file, "wb");
    if (output_fp == NULL)
        cout << "output file open error."<<endl;
    if(dw_size != 4 && dw_size != 8)
        cout << "dataword size must be 4 or 8. "<<endl;

    CRC_ENCODE(input_fp, output_fp, argv[3], dw_size);
}

void CRC_ENCODE(FILE *input_fp, FILE *output_fp,char *gen, int dw_size)
{

    char ch,ch_tmp, data[8];
    char  *code_word, *encode_data;
    char *tmp;
    int bi = 1;
    int cw_size, div_size;
    int pad_size, encode_size;
    int i , j;
    int remainder;
    div_size = strlen(gen);
    if (dw_size == 4){
        cw_size = dw_size + div_size - 1;
        pad_size = 8 - (cw_size * 2) % 8;

        encode_size = cw_size*2+pad_size;
        encode_data = new char[encode_size/8];
        tmp = new char[encode_size];
    
    }
    else if (dw_size == 8)
    {
        cw_size = dw_size + div_size - 1;
        pad_size = 8 - (cw_size) % 8;

        encode_size = cw_size + pad_size;
        encode_data = new char[encode_size / 8];
        tmp = new char[encode_size];
    }
   // cout <<"cw size is "<<cw_size<< ' '<<encode_size<<endl;
    code_word = new char[cw_size];
    ch = '1';
    //ch_tmp = 'A';
    //cout <<(int)ch_tmp<<endl;
    //printf("pad size is %d %X %c\n", (char)(pad_size), (char)(pad_size), (char)(pad_size));
    fputc((char)pad_size, output_fp); // padding info input
    
    while(1){
        
        ch = fgetc(input_fp); // 8bit 받아오기
        if(ch == EOF){
            fputc(EOF,output_fp);
            fclose(output_fp);
            fclose(input_fp);
            break;
        }
        // else
        //     cout << "ch  is "<<ch <<endl;
        bi = 1;
        for (i = 7; i >= 0; i--)
        {
            data[i] = (ch & bi) + 0x30;
            ch = ch >> 1;
          //  cout << data[i]; // 입력받은 변수를 이진수로 저장
        }
        //cout << endl;
        if (dw_size == 4)
        {
            for (i = 0; i < dw_size; i++) // 앞 4bit에 해당 encode
                code_word[i] = data[i];
            for (i = dw_size; i < cw_size; i++) // 뒷부분은 다 0으로
                code_word[i] = '0';
            // for(i=0;i<cw_size;i++)
            //  cout << "cw  is " << code_word[i] << endl;
            make_cw(code_word, gen);
            for (i = 0; i < pad_size; i++)
                tmp[i] = '0';
            for (i = 0; i < cw_size; i++)
            {
                tmp[i + pad_size] = code_word[i];
            }
            for (i = 0; i < dw_size; i++) // 뒤 4bit에 해당 encode
                code_word[i] = data[i + 4];
            for (i = dw_size; i < cw_size; i++) // 뒷부분은 다 0으로
                code_word[i] = '0';
            // for (i = 0; i < cw_size; i++)
            //     cout << "cw  is " << code_word[i] << endl;
            make_cw(code_word, gen);
            for (i = 0; i < cw_size; i++)
            {
                tmp[cw_size + pad_size + i] = code_word[i];
            }
            // for (i = 0; i < encode_size; i++)
            //     cout << tmp[i];
            // cout << endl;
            int k = 0;
            for (k = 0; k < encode_size / 8; k++)
            {
                int save_val = 0;
                bi = 1;

                for (i = k * 8 + 7; i >= k * 8; i--)
                {
                    save_val = save_val + ((tmp[i] - 0x30) * bi);
                    bi *= 2;
                  //  cout << (int)save_val << endl;
                }
                // cout <<"save!!"<< (int)save_val;
                fputc(save_val, output_fp);
            }
        }
        else if (dw_size == 8)
        {
            // cout << "dw_Size is 8!\n";
            for (i = 0; i < dw_size; i++) // 앞 4bit에 해당 encode
                code_word[i] = data[i];
            for (i = dw_size; i < cw_size; i++) // 뒷부분은 다 0으로
                code_word[i] = '0';
            // for (i = 0; i < cw_size; i++)
            //  cout << "cw  is " << code_word[i] << endl;
            make_cw(code_word, gen);
            for (i = 0; i < pad_size; i++)
                tmp[i] = '0';
            for (i = 0; i < cw_size; i++)
            {
                tmp[i + pad_size] = code_word[i];
            }
            // for (i = 0; i < encode_size; i++)
            //     cout << tmp[i];
            // cout << endl;
            int k = 0;
            for (k = 0; k < encode_size / 8; k++)
            {
                int save_val = 0;
                bi = 1;

                for (i = k * 8 + 7; i >= k * 8; i--)
                {
                    save_val = save_val + ((tmp[i] - 0x30) * bi);
                    bi *= 2;
               //     cout << (int)save_val << endl;
                }
                // cout <<"save!!"<< (int)save_val;
                fputc(save_val, output_fp);
            }
        }
    }
    
}

void make_cw(char *cw, char *gen)
{
    char *window;
    int cw_size = strlen(cw);
    int gen_size = strlen(gen);
    window = new char[gen_size];
    //cout << cw_size << ' ' << gen << endl;
    int i,j;
    for(i =0;i<gen_size;i++)
        window[i] = cw[i];
    for(i=0;i<cw_size - gen_size+1;i++)
    {

        if (window[0] == '0')
        {
            // cout << "number 0" << endl;
            for (j = 0; j < gen_size-1; j++)
            {
                window[j] = window[j+1];
            }
            if ((i + gen_size) >= cw_size)
                window[j] = 0;
            else
                window[j] = cw[i+gen_size];
        }
        else if (window[0] == '1')
        {
            // cout <<"number 1"<<endl;
            for (j = 0; j < gen_size - 1; j++)
            {
                if(window[j+1] == gen[j+1])
                    window[j] = '0';
                else
                    window[j] = '1';
                
            }
            if ((i + gen_size) >= cw_size)
                window[j] = 0;
            else
                window[j] = cw[i + gen_size];
        }
        //cout<<window<<endl;
    }
    for (i = cw_size-gen_size+1; i < cw_size; i++) // 뒷부분은 다 0으로
        cw[i] = window[i - cw_size + gen_size-1];
    //cout<<"HERHERHERH"<<endl;
    // for (i =0 ; i < cw_size; i++) // 뒷부분은 다 0으로
    //     cout<<cw[i];
    // cout <<endl;
    // for(i =0;i<gen_size;i++)
    //     cout <<"!!!!!!\n"<< window[i];
    // cout << "\n???????"<<endl;

        
}
