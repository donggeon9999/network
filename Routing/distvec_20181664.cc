#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
using namespace std;


#define endl '\n'




typedef struct{
    int start;
    int end;
    int weight;

}Link;

typedef struct{
    int dest; // 목적지
    int distance; // 거리
    int next; // 바로 다음 노드
}Dest;

typedef struct{
    int start;
    int end;
    char msg[100];
}Msg;

typedef struct{

    int node_idx;
    Dest *dest;

}Table;

int link_cnt = 0;
vector<Msg> vec_msg;
vector<Link> vec_link;
Table *node_table;

void table_init(int node_cnt) ;
void link_change(int start, int end, int weight);
void print_router(FILE *) ;
int table_update(Table *table1, Table *table2, int node_cnt, int distance)
{
    int i;
    int is_changed = 0;
    int idx1 = table1->node_idx;//table 1의 idx
    int idx2 = table2->node_idx;//table 2의 idx
    // int distance = table1->dest[idx2].distance; // 두 노드 사이의 거리
    //table1->table2
    // cout << idx1 << ' ' << idx2 << ' ' << "changing" << distance << endl;

    for(i=0;i<node_cnt;i++)
    {
        if(i == idx2 || i == idx1)
            continue;
        if(table1->dest[i].distance >0)
        {
            int new_distance;
            new_distance = distance + table1->dest[i].distance;
            // printf("%d new distanceAA : %d\n", i, new_distance);
            if (table2->dest[i].distance == new_distance && table2->dest[i].next > idx1)
            {
                table2->dest[i].next = idx1;
            }
            if (table2->dest[i].distance == -1 || table2->dest[i].distance > new_distance)
            {
                // cout << idx1 << ' ' << idx2 << ' ' << "changed!!" << distance << endl;

                table2->dest[i].distance = new_distance;
                table2->dest[i].next = idx1;
                is_changed = 1;
            }
        }


    }
    for (i = 0; i < node_cnt; i++)
    {
        // printf("distance is %d %d\n", table2->dest[i].distance, node_cnt);

        if (i == idx2 || i == idx1)
            continue;
         if (table2->dest[i].distance > 0)
        {
            int new_distance;
            new_distance = distance + table2->dest[i].distance;
            // printf("%d new distanceBB : %d\n",i, new_distance);


            if (table1->dest[i].distance == -1 || table1->dest[i].distance > new_distance)
            {

                // cout << idx1 << ' ' << idx2 << ' ' << "changed!@@@!" << distance << endl;

                table1->dest[i].distance = new_distance;
                table1->dest[i].next = idx2;
                is_changed = 1;
            }
        }
    }
    return is_changed;
}
void make_table( int node_cnt)
{
    int i;
    int is_changed,is_final_changed;
    while(1)
    {
        is_final_changed = 0;
        for (i = 0; i < (int)(vec_link.size()); i++)
        {  
            int a, b, distance;
            a = vec_link[i].start;
            b = vec_link[i].end;
            distance = vec_link[i].weight;
            // printf("nlink is %d %d %d \n",a,b,distance);
            is_changed = table_update(&node_table[a], &node_table[b], node_cnt, distance);
            if(is_changed == 1)
                is_final_changed = is_changed;
        }

        if (is_final_changed == 0)
            return;
    }



}

int main(int argc, char* argv[])
{
    FILE* topo_fp;
    FILE *mss_fp;
    FILE * chng_fp;
    FILE *output_fp;
    int i,j;

    int node_cnt;
    if (argc != 4)
    {
        cout << "usage: linkstate topologyfile messagesfile changesfile\n";
        return -1;
    }
    topo_fp = fopen(argv[1], "r+");
    if (topo_fp == NULL)
    {
        cout << "Error: open input file.\n";
        return -1;
    }
    mss_fp = fopen(argv[2], "r+");
    if (mss_fp == NULL)
    {
        cout << "Error: open input file.\n";
        return -1;
    }
    chng_fp = fopen(argv[3], "r+");
    if (chng_fp == NULL)
    {
        cout << "Error: open input file.\n";
        return -1;
    }
    output_fp = fopen("output_dv.txt", "w");

    fscanf(topo_fp,"%d",&node_cnt);
    node_table = new Table[node_cnt];
    // cout << 'a'<<node_cnt;
    // 각 노드 별 테이블 생성
    for (i = 0; i < node_cnt; i++)
    {
        node_table[i].node_idx = i;
        node_table[i].dest = new Dest[node_cnt];
        int j;
        for(j=0;j<node_cnt;j++){
            node_table[i].dest[j].distance = -1;
            node_table[i].dest[j].dest = j;
            node_table[i].dest[j].next = -1;
        }
        node_table[i].dest[i].distance = 0;
        node_table[i].dest[i].dest = i;
        node_table[i].dest[i].next = i;
    }
    // cout << 'b';
    //link 받아오기
    while(1)
    {
        Link link;

        fscanf(topo_fp, "%d %d %d", &link.start, &link.end, &link.weight);
        if(feof(topo_fp))
            break;
        //cout << link.start << ' ' << link.end << ' ' << link.weight << endl;
        vec_link.push_back(link);

        link_cnt++;
        node_table[link.start].dest[link.end].dest = link.end;
        node_table[link.start].dest[link.end].distance = link.weight;
        node_table[link.start].dest[link.end].next = link.end;
        node_table[link.end].dest[link.start].dest = node_table[link.end].dest[link.start].next = link.start;
        node_table[link.end].dest[link.start].distance = link.weight;
    }
    // cout << 'c';'
    //cout << node_table[0].dest[1].distance << endl  << node_table[0].dest[3].distance << endl;

    //message txt 저장
    while (1)
    {
        Msg msg_tmp;
        fscanf(mss_fp, "%d %d", &msg_tmp.start, &msg_tmp.end);
        if (feof(mss_fp))
            break;
        fgetc(mss_fp);
        fgets(msg_tmp.msg, 100, mss_fp);
        // cout << msg_tmp.start << msg_tmp.end << endl;
        // cout << msg_tmp.msg << endl;
        vec_msg.push_back(msg_tmp);
        // cout << vec_msg[0].start << vec_msg[0].end << endl;
        // cout << vec_msg[0].msg << endl;
    }
    while(1)
    {

        make_table( node_cnt);
        for (i = 0; i < node_cnt; i++)
        {

            for (j = 0; j < node_cnt; j++)
            {
                if (node_table[i].dest[j].next != -1)
                    fprintf(output_fp, "%d %d %d\n", j, node_table[i].dest[j].next, node_table[i].dest[j].distance);
            }
            fprintf(output_fp, "\n");
        }
        print_router(output_fp);
        // cout << 'a';
        int start, end, weight;
        fscanf(chng_fp, "%d %d %d", &start, &end, &weight);
        if (feof(chng_fp))
            break;
        link_change(start, end, weight);
        table_init(node_cnt);
    }
   
}

void table_init(int node_cnt)
{
    int i;
    for (i = 0; i < node_cnt; i++)
    {
        int j;
        for (j = 0; j < node_cnt; j++)
        {
            node_table[i].dest[j].distance = -1;
            node_table[i].dest[j].dest = j;
            node_table[i].dest[j].next = -1;
        }
        node_table[i].dest[i].distance = 0;
        node_table[i].dest[i].dest = i;
        node_table[i].dest[i].next = i;
    }

    for(i=0;i<(int)(vec_link.size());i++)
    {
        Link link = vec_link[i];
        node_table[link.start].dest[link.end].dest = link.end;
        node_table[link.start].dest[link.end].distance = link.weight;
        node_table[link.start].dest[link.end].next = link.end;
        node_table[link.end].dest[link.start].dest = node_table[link.end].dest[link.start].next = link.start;
        node_table[link.end].dest[link.start].distance = link.weight;
    }
    return;
}

void print_router(FILE *output_fp)
{
    int i;
    int mid;
    // printf("print router! in %d \n", vec_msg.size());
    for (i = 0; i < (int)(vec_msg.size()); i++)
    {

        Msg tmp = vec_msg[i];
        if (node_table[tmp.start].dest[tmp.end].distance < 0)
            fprintf(output_fp, "from %d to %d cost infinite hops unreachable ", tmp.start, tmp.end);
        else
        {
            fprintf(output_fp, "from %d to %d cost %d hops ", tmp.start, tmp.end, node_table[tmp.start].dest[tmp.end].distance);
            mid = tmp.start;
            while (1)
            {
                if (mid == tmp.end)
                {

                    break;
                }

                fprintf(output_fp, "%d ", mid);
                mid = node_table[mid].dest[tmp.end].next;
            }
        }
        fprintf(output_fp, "message %s", tmp.msg);
    }
    fprintf(output_fp,"\n");
    // cout <<"router out"<< endl;
    // cout << "router out" << endl;
    return;
}
void link_change(int start, int end, int weight)
{
    int i;
    Link link;
    // printf("change!!!%d %d %d\n",start,end,weight);
    // printf("%d %d\n",link_cnt, vec_link.size());
    for(i=0;i<link_cnt;i++)
    {
        if(vec_link[i].start == start && vec_link[i].end == end)
        {
            if(weight == -999)
            {
                vec_link.erase(vec_link.begin()+i);
                link_cnt--;
            }
            else
            {
                vec_link[i].weight = weight;
            }
            // printf("%d %d %daa\n", vec_link[i].start, vec_link[i].end, vec_link[i].weight);
            return;
        }
        if (vec_link[i].start == end && vec_link[i].end == start)
        {
            if (weight == -999)
            {
                vec_link.erase(vec_link.begin() + i);
                link_cnt--;
            }
            else
            {
                vec_link[i].weight = weight;
            }
            // printf("%d %d %dbb\n", vec_link[i].start, vec_link[i].end, vec_link[i].weight);
            return;
        }
    }
    // printf("plus");
    if(weight != -999)
    {
        link.start = start;link.end = end;link.weight = weight;
        link_cnt++;
        vec_link.push_back(link);
    }
    // printf("%d %d %dcc\n", vec_link[i].start, vec_link[i].end, vec_link[i].weight);

    return;

}