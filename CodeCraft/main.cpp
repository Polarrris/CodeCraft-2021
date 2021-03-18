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
#include <cstdio>
#include <algorithm>
#include <math.h>
#include <cmath>

#include "Input.h"
#include "Output.h"
#include "DataStructure.h"
#include "HostPlanning.h"
#include "VMMigration.h"
#include "VMDeployment.h"
using namespace std;





int main(){
    list<string> buyMsg;
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
        string tmp,infoType,vmId;
        //开始处理每天的请求
        for(int day = 0; day < dayCount; day++){
            buyMsg.clear();
            
            scanf("%d",&requestCount);
            requestInfos.clear();
            for(int i = 0; i < requestCount; i++){
                cin>>infoType;
                if(infoType[1] == 'a'){
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
            for(int i = 0; i < requestCount; ++i){
                if(requestInfos[i].infoType == add)
                {
                    bool isDuetOfVm = vms[requestInfos[i].vmType].isDuet;
                    float cmRatioOfVm = (float)vms[requestInfos[i].vmType].cpu / vms[requestInfos[i].vmType].mm;
                    float mincmRatioDiff = 10000.0;
                    PurchasedHost* bestHost = nullptr;
                    int bestHostIndex = -1;
                    Node bestHostNode;          // A节点或B节点

                    for(int j = 0; j < purchasedHosts->size(); ++j)
                    {
                        if(isDuetOfVm && purchasedHosts->at(j).isDuet)
                        {
                            if(purchasedHosts->at(j).isRemainResourceAvailForBoth(vms[requestInfos[i].vmType].cpu, vms[requestInfos[i].vmType].mm))
                            {
                                float thiscmRatioDiff = fabs(purchasedHosts->at(j).remainCMRatioBoth / (double)cmRatioOfVm - 1);
                                if(thiscmRatioDiff < mincmRatioDiff)
                                {
                                    mincmRatioDiff = thiscmRatioDiff;
                                    bestHost = &purchasedHosts->at(j);
                                    bestHostIndex = j;
                                    bestHostNode = Both;
                                }
                            }
                            // else
                            // {
                            //     // TODO
                            //     // try to replace and optimize
                            // }
                        }
                        if(!isDuetOfVm && !purchasedHosts->at(j).isDuet)
                        {
                            // TODO
                            // single deployment
                            if(purchasedHosts->at(j).isRemainResourceAvailForA(vms[requestInfos[i].vmType].cpu, vms[requestInfos[i].vmType].mm))
                            {
                                float thiscmRatioDiff = fabs(purchasedHosts->at(j).remainCMRatioA / (double)cmRatioOfVm - 1);
                                if(thiscmRatioDiff < mincmRatioDiff)
                                {
                                    mincmRatioDiff = thiscmRatioDiff;
                                    bestHost = &purchasedHosts->at(j);
                                    bestHostIndex = j;
                                    bestHostNode = A;
                                }
                            }
                            if(purchasedHosts->at(j).isRemainResourceAvailForB(vms[requestInfos[i].vmType].cpu, vms[requestInfos[i].vmType].mm))
                            {
                                float thiscmRatioDiff = fabs(purchasedHosts->at(j).remainCMRatioB / (double)cmRatioOfVm - 1);
                                if(thiscmRatioDiff < mincmRatioDiff)
                                {
                                    mincmRatioDiff = thiscmRatioDiff;
                                    bestHost = &purchasedHosts->at(j);
                                    bestHostIndex = j;
                                    bestHostNode = B;
                                }
                            }
                        }
                    }

                    //没找到，要买了
                    if(bestHostIndex == -1)
                    {
                        // cout<<requestInfos[i].infoType<<requestInfos[i].vmType<<vms[requestInfos[i].vmType].isDuet<<endl;
                        /////////////////////////////////////////////////////////////////////
                        // TODO
                        // purchase new host and deploy vm on that
                        float mincmRatioDiff = 10000.0;
                        int purchasedBestHostIndex = -1;
                        string purchasedBestHostType;
                        for(int j = 0; j < hosts.size(); ++j)
                        {
                            float thiscmRatioDiff = fabs(hosts[j].cmRatio / (float)cmRatioOfVm - 1);
                            if(thiscmRatioDiff < mincmRatioDiff)
                            {
                                mincmRatioDiff = thiscmRatioDiff;
                                purchasedBestHostIndex = j;
                            }
                        }
                        bestHost = new PurchasedHost(hosts[purchasedBestHostIndex].cpu, hosts[purchasedBestHostIndex].mm);
                        purchasedHosts->push_back(*bestHost);
                        // cout<<"vector size: "<<purchasedHosts->size()<<endl;
                        // cout<<purchasedHosts->back().listDVMA->size();
                        // cout<<purchasedHosts->back().listDVMB->size();
                        // cout<<purchasedHosts->back().listDVMBoth->size()<<endl;
                        // cout<<&purchasedHosts->back().listDVMA<<endl<<&bestHost->listDVMA<<endl;


                        // delete bestHost;
                        
                        
                        // cout<<purchasedHosts->back().listDVMA->size();
                        // cout<<purchasedHosts->back().listDVMB->size();
                        // cout<<purchasedHosts->back().listDVMBoth->size();

                        bestHost = &purchasedHosts->back();
                        // cout<<purchasedHosts->back().listDVMA->size();
                        // cout<<purchasedHosts->back().listDVMB->size();
                        // cout<<purchasedHosts->back().listDVMBoth->size();
                        bestHostIndex = purchasedHosts->size() - 1;
                        buyMsg.push_back("("+hosts[purchasedBestHostIndex].hostType+",1)\n");
    
                        if(isDuetOfVm)
                        {
                            bestHost->isDuet = true;
                            bestHost->addBoth(requestInfos[i].vmType, bestHostIndex, day, requestInfos[i].vmId);
                        }
                        else
                        {
                            bestHost->isDuet = false;
                            bestHost->addSingleA(requestInfos[i].vmType, bestHostIndex, day, requestInfos[i].vmId);
                        }
                    }

                    // TODO
                    // deploy
                    else
                    {
                        if(isDuetOfVm)
                            bestHost->addBoth(requestInfos[i].vmType, bestHostIndex, day, requestInfos[i].vmId);
                        else
                        {
                            if(bestHostNode == A)
                                bestHost->addSingleA(requestInfos[i].vmType, bestHostIndex, day, requestInfos[i].vmId);
                            else if(bestHostNode == B)
                                bestHost->addSingleB(requestInfos[i].vmType, bestHostIndex, day, requestInfos[i].vmId);
                        }
                        
                    }
                }
                else
                {
                    purchasedHosts->at(vmIdToDeployedVM[requestInfos[i].vmId]->host).delvm(*vmIdToDeployedVM[requestInfos[i].vmId]);
                }
            }
            printf("(purchase,%lu)\n",buyMsg.size());
            for (auto it = buyMsg.begin(); it != buyMsg.end(); it++)
            {
                printf("%s",(*it).c_str());
            }
            
            // cout<<day<<"day"<<endl;
            printf("(migration, 0)\n");
            outputDeploymentResult();
        }
        
    }
    else
    {
        cerr <<"file not found" << endl;
    }

    // fclose(stdin);
    finish = clock();
    printf("\nuse time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
    return 0;
}

