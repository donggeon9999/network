#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


#define endl '\n'
#define INT_MAX 2000000



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
    int cur_idx;
    int distance;
    int parent_idx;

    int next_idx;

}Dp;

typedef struct{
    int start;
    int end;
    char msg[1000];
}Msg;

typedef struct{

    int node_idx;
    Dest dest[100];

}Table;

int link_cnt = 0;
vector<Msg> vec_msg;
vector<Link> vec_link;
// vector<Dp> test;
// vector<Dp> vec_dp;
Table node_table[100];


void make_dijkstra(int node_cnt, int idx) ;
void table_init(int node_cnt);
void link_change(int start, int end, int weight);
void print_router(FILE *output_fp) ;

int table_update(Table *table1, Table *table2, int node_cnt, int distance) ;
void make_table(int node_cnt) ;


int main(int argc, char* argv[])
{
    FILE* topo_fp;
    FILE *mss_fp;
    FILE * chng_fp;
    FILE *output_fp;
    int i,j;

    int node_cnt;
    if(argc != 4){
        cout << "usage: linkstate topologyfile messagesfile changesfile\n";
        return -1;
    }

    topo_fp = fopen(argv[1], "r+");
    if(topo_fp == NULL){
        cout << "Error: open input file.\n";
        return -1;
    }
    mss_fp = fopen(argv[2], "r+");
    if (mss_fp == NULL){
        cout << "Error: open input file.\n";
        return -1;
    }
    chng_fp = fopen(argv[3], "r+");
    if (chng_fp == NULL){
        cout << "Error: open input file.\n";
        return -1;
    }
    output_fp = fopen("output_ls.txt","w");


    fscanf(topo_fp,"%d",&node_cnt);
    // cout << 'a'<<node_cnt;

    int start, end, weight;
    // printf("a11\n");
    fscanf(chng_fp, "%d %d %d", &start, &end, &weight);
    // printf("a11\n");
    // printf("%d %d %d\n",start,end,weight);

    // 각 노드 별 테이블 생성
    for (i = 0; i < node_cnt; i++)
    {
        node_table[i].node_idx = i;
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
        if(feof(topo_fp)){

            // printf("is finish!!\n");
            break;

        }
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
    // printf("topfo fp is error!2222\n");

    while (1)
    {
        Msg msg_tmp;

        fscanf(mss_fp, "%d %d", &msg_tmp.start, &msg_tmp.end);

        if (feof(mss_fp)){
            break;

        }
        fgetc(mss_fp);
        fgets(msg_tmp.msg, 100, mss_fp);
        vec_msg.push_back(msg_tmp);
    }

    //change 이전 상태 출력
    for (i = 0; i < node_cnt; i++)
        make_dijkstra(node_cnt, i);
    for (i = 0; i < node_cnt; i++)
    {

        for (j = 0; j < node_cnt; j++)
        {
            if (node_table[i].dest[j].next != -1)
                fprintf(output_fp, "%d %d %d\n", j, node_table[i].dest[j].next, node_table[i].dest[j].distance);

        }
        fprintf(output_fp,"\n");
    }
    print_router(output_fp);

    while(1)
    {
        //change
        if(feof(chng_fp))
        {

            // printf("is finish!!\n");
            break;
        }

        link_change(start, end, weight);

        table_init(node_cnt);

        fprintf(output_fp, "\n");

        for (i = 0; i < node_cnt; i++)
            make_dijkstra(node_cnt, i);
        for (i = 0; i < node_cnt; i++)
        {

            for (j = 0; j < node_cnt; j++)
            {
                if (node_table[i].dest[j].next != -1)
                    fprintf(output_fp, "%d %d %d\n", j, node_table[i].dest[j].next, node_table[i].dest[j].distance);
            }
            fprintf(output_fp,"\n");
        }
        print_router(output_fp);
        // printf("a11\n");

        fscanf(chng_fp, "%d %d %d", &start, &end, &weight);
        // printf("a11\n");
        if (feof(chng_fp))
            break;
        
    }
   
}

//idx의 노드에 해당하는 다익스트라 생성!!!!!!!!
void make_dijkstra(int node_cnt,int idx)
{
    // printf("make dijkstra %d \n",idx);
    // int min_idx, min_dis = INT_MAX;
    // int min_vec_idx;
    int i;
    int checked[100] ;
    // int table[100];
    int none_flag = 0;
    Dp dp[100];
    for(i = 0;i<node_cnt;i++)
    {
        checked[i] = 0;
        dp[i].distance = INT_MAX;
        dp[i].parent_idx = -1;
        dp[i].cur_idx = i;
        dp[i].next_idx = i;
    }
    
    checked[idx] = 1;
    dp[idx].distance = 0;
    dp[idx].parent_idx = idx;
    dp[idx].cur_idx = idx;
    dp[i].next_idx = idx;

    for(i=0;i<link_cnt;i++)
    {
        Link tmp = vec_link[i];
        if(tmp.start == idx&& checked[tmp.end] == 0)
        {
            dp[tmp.end].distance = tmp.weight;
            dp[tmp.end].parent_idx = tmp.start;
            dp[tmp.end].next_idx = tmp.end;
            // test.push_back(dp[i]);
            none_flag =1;
        }
        else if (tmp.end == idx && checked[tmp.start] == 0)
        {
            dp[tmp.start].distance = tmp.weight;
            dp[tmp.start].parent_idx = tmp.end;
            dp[tmp.start].next_idx = tmp.start;
            // test.push_back(dp[i]);
            none_flag = 1;
        } 
        
    }
    if(none_flag == 0)
        return;
    // for (i = 0; i < test.size(); i++)
    // {
    //     printf("test%d %d %d\n", test[i].cur_idx, test[i].distance, test[i].parent_idx);
    // }
    int all_search_cnt = 0;
    while(1)
    {
        
        int min = INT_MAX;
        int min_idx;
        int flag = 0, flag2 = 0;
    

        for (i = 0; i < node_cnt; i++)
        {
            if (checked[i] != 1)
                flag = 1;
        }
        if(flag == 0||all_search_cnt >node_cnt)
            break;


        // tie breaking 2 -> 낮은 idx 노드부터 검색하며 최소값 찾으면 해결
        for (i = 0; i < node_cnt; i++)
        {
            if(dp[i].distance < min && checked[i] == 0)
            {
                min = dp[i].distance;
                min_idx = i;
                // printf("mm\n");
                flag2 = 1;
            }
        }
        if(flag2 == 0)
            break;
        checked[min_idx] = 1;
        node_table[idx].dest[min_idx].distance = min;
        node_table[idx].dest[min_idx].next = dp[min_idx].next_idx;
        all_search_cnt++;
        // printf("%d %d\n",min,min_idx);
        // vec_dp.push_back(dp[min_idx]);

        //SPT update

        // for(i=0;i<vec_link.size();i++)
        // {
        //     printf("vec %d %d %d\n",vec_link[i].start,vec_link[i].end,vec_link[i].weight);
        // }
        

        for (i = 0; i < link_cnt; i++)
        {
            Link tmp = vec_link[i];
            
            // printf(" ");//,link_cnt,tmp.start,tmp.end,tmp.weight);
            if (tmp.start == min_idx && checked[tmp.end] == 0)
            {
               int new_distance = min + tmp.weight;
               if(new_distance < dp[tmp.end].distance){
                // printf("%d !new is %d d is %d\n",i, new_distance, dp[tmp.end].distance);
                dp[tmp.end].distance = new_distance;
                dp[tmp.end].parent_idx = tmp.start;
                dp[tmp.end].next_idx = dp[min_idx].next_idx;
               }
               else if (new_distance < dp[tmp.end].distance && dp[tmp.end].parent_idx > tmp.start)
               {
                dp[tmp.end].parent_idx = tmp.start;
                dp[tmp.end].next_idx = dp[min_idx].next_idx;
               }
            }
            else if (tmp.end == min_idx && checked[tmp.start] == 0)
            {
               int new_distance = min + tmp.weight;
               if (new_distance < dp[tmp.start].distance)
               {
                // printf("%d new is %d d is %d\n", i, new_distance, dp[tmp.start].distance);
                dp[tmp.start].distance = new_distance;
                dp[tmp.start].parent_idx = tmp.end;
                dp[tmp.start].next_idx = dp[min_idx].next_idx;
               }
               else if (new_distance < dp[tmp.start].distance && dp[tmp.start].parent_idx > tmp.end)
               {
                dp[tmp.start].parent_idx = tmp.end;
                dp[tmp.start].next_idx = dp[min_idx].next_idx;
               }
            }
        }
    }
        // for(i = 0 ; i<vec_dp.size();i++)
        // {
        //     printf("%d %d %d\n",vec_dp[i].cur_idx,vec_dp[i].distance,vec_dp[i].parent_idx);
        // }
    // delete checked;
    // delete table;
    // delete dp;
 
    
}
void table_init(int node_cnt)
{
    int i;
    for (i = 0; i < node_cnt; i++)
    {
        int j;
        for (j = 0; j < node_cnt; j++)
        {
            node_table[i].dest[j].distance = -999;
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
    for(i = 0;i<(int)(vec_msg.size());i++)
    {
        
        Msg tmp = vec_msg[i];
        if (node_table[tmp.start].dest[tmp.end].distance < 0 )
            fprintf(output_fp, "from %d to %d cost infinite hops unreachable ", tmp.start, tmp.end);
        else{
            fprintf(output_fp ,"from %d to %d cost %d hops ", tmp.start, tmp.end, node_table[tmp.start].dest[tmp.end].distance);
            mid = tmp.start;
            while (1)
            {
               if (mid == tmp.end){
                break;

               }
                fprintf(output_fp,"%d ",mid);

               mid = node_table[mid].dest[tmp.end].next;
            }   
        }
        fprintf(output_fp ,"message %s", tmp.msg);
    }
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

int table_update(Table *table1, Table *table2, int node_cnt, int distance)
{
    int i;
    int is_changed = 0;
    int idx1 = table1->node_idx; // table 1의 idx
    int idx2 = table2->node_idx; // table 2의 idx
    // int distance = table1->dest[idx2].distance; // 두 노드 사이의 거리
    // table1->table2
    // cout << idx1 << ' ' << idx2 << ' ' << "changing" << distance << endl;

    for (i = 0; i < node_cnt; i++)
    {
        if (i == idx2 || i == idx1)
            continue;
        if (table1->dest[i].distance > 0)
        {
            int new_distance;
            new_distance = distance + table1->dest[i].distance;
            if (table2->dest[i].distance == -1 || table2->dest[i].distance > new_distance)
            {

                table2->dest[i].distance = new_distance;
                table2->dest[i].next = idx1;
                is_changed = 1;
            }
        }
    }
    for (i = 0; i < node_cnt; i++)
    {

        if (i == idx2 || i == idx1)
            continue;
        if (table2->dest[i].distance > 0)
        {
            int new_distance;
            new_distance = distance + table2->dest[i].distance;

            if (table1->dest[i].distance == -1 || table1->dest[i].distance > new_distance)
            {


                table1->dest[i].distance = new_distance;
                table1->dest[i].next = idx2;
                is_changed = 1;
            }
        }
    }
    return is_changed;
}

void make_table(int node_cnt)
{
    int i;
    int is_changed, is_final_changed;
    while (1)
    {
        is_final_changed = 0;
        for (i = 0; i < (int)(vec_link.size()); i++)
        {
            int a, b, distance;
            a = vec_link[i].start;
            b = vec_link[i].end;
            distance = vec_link[i].weight;
            is_changed = table_update(&node_table[a], &node_table[b], node_cnt, distance);
            if (is_changed == 1)
                is_final_changed = is_changed;
        }

        if (is_final_changed == 0)
            return;
    }
}