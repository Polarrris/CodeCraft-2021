//
//  main.cpp
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/12.
//


#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <utility>

#include "Input.h"
#include "DataStructure.h"
#include "HostPlanning.h"
#include "VMMigration.h"
#include "VMDeployment.h"
using namespace std;



int main(){
    clock_t start, finish;
    start = clock();
//TEST
    const string filePath = "/Users/jesse/Desktop/CodeCraft/training-data/training-1.txt";
    //输入
    if(freopen(filePath.c_str(),"rb",stdin)){
        //解析hosts信息
        int hostCount = 0;
        scanf("%d",&hostCount);
        string hostType,cpu,mm,hostCost,dailyCost;
        for(int i =0;i<hostCount;i++){
            cin>>hostType>>cpu>>mm>>hostCost>>dailyCost;
            generateHost(hostType,cpu,mm,hostCost,dailyCost);
        }
        
        //解析vmProperties信息
        int vmCount = 0;
        scanf("%d",&vmCount);
        string vmType,isDuet;
        for(int i = 0; i < vmCount; i++){
            cin>>vmType>>cpu>>mm>>isDuet;
            generateVm(vmType,cpu,mm,isDuet);
        }
        
        //解析用户请求天数
        int dayCount = 0, requestCount = 0;
        scanf("%d",&dayCount);
        string infoType,vmId;
        //开始处理每天的请求
        for(int day = 0; day < dayCount; day++){
            
            /*
             
                HostPlanning???
             
            */
            
            /*
             
                VMMigration???
             
             */
            
            scanf("%d",&requestCount);
            requestInfos.clear();
            for(int i = 0; i < requestCount; i++){
                cin>>infoType;
                if(infoType[1] == add){
                    cin>>vmType>>vmId;
                    generateRequest(vmType,vmId);
                }
                else{
                    cin>>vmId;
                    generateRequest(vmId);
                }
            }
            /*
             
                VMDeployment???
                输入：HostPlanning的结果
                    当天用户的所有请求
             
             */
        }
    }
    else cerr <<"file not found" << endl;
    
    fclose(stdin);
    finish = clock();
    printf("\nusr time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
    
    return 0;
}

