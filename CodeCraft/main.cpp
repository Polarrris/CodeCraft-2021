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
#include <map>
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
using namespace std;

float getCostPerFormance(int index,int day,int totalday){
    return (hosts[index].cpu+hosts[index].mm)/float(hosts[index].hostCost+hosts[index].dailyCost*(totalday-day));
}

int getBestHost(const vector<RequestInfo>& requestInfos,int i,int day,int totalDay){
    bool isDuetOfVm = vms[requestInfos[i].vmType].isDuet;
    
    float cmRatioOfVm = (float)vms[requestInfos[i].vmType].cpu / vms[requestInfos[i].vmType].mm;
    // cm比差距 index 性价比
    vector<pair<float,pair<int,float>>> hostVec;
    hostVec.push_back(pair<float,pair<int,float>>(10000,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10001,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10002,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10003,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10004,pair<int,float>(-1,0)));
    for(int j = 0; j < hosts.size(); ++j)
        {
            float thiscmRatioDiff = fabs(hosts[j].cmRatio - (float)cmRatioOfVm);
            if(!isDuetOfVm)
            {
                    if(thiscmRatioDiff < hostVec.back().first&&hosts[j].cpu>=vms[requestInfos[i].vmType].cpu*2&&hosts[j].mm>=vms[requestInfos[i].vmType].mm*2)
                {
                    bool flag =false;
                    hostVec.pop_back();
                    for (auto it = hostVec.begin(); it != hostVec.end(); it++)
                    {
                        if (it->first > thiscmRatioDiff)
                        {
                            flag = true;
                            hostVec.insert(it,pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(j,day,totalDay))));
                            break;
                        }
                    }
                    if (flag != true)
                    {
                        hostVec.push_back(pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(j,day,totalDay))));
                    }
                    
                }
            }else{
                if(thiscmRatioDiff < hostVec.back().first&&hosts[j].cpu>=vms[requestInfos[i].vmType].cpu&&hosts[j].mm>=vms[requestInfos[i].vmType].mm)
                {
                    hostVec.pop_back();
                    bool flag = false;
                    for (auto it = hostVec.begin(); it != hostVec.end(); it++)
                    {
                        if (it->first > thiscmRatioDiff)
                        {
                            flag = true;
                            hostVec.insert(it,pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(j,day,totalDay))));
                            break;
                        }
                        
                    }
                    if (flag != true)
                    {
                        hostVec.push_back(pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(j,day,totalDay))));
                    }
                    
                }
            }
            
        }
        int returnIndex = hostVec[0].second.first;
        float best = hostVec[0].second.second;
        for (size_t l = 0; l < hostVec.size(); l++)
        {
            if (hostVec[l].second.second>best)
            {
                best = hostVec[l].second.second;
                returnIndex = hostVec[l].second.first;
            }
        }

        // for (size_t l = 0; l < hostVec.size(); l++)
        // {
        //     cout<<hostVec[l].first<<" "<<hostVec[l].second.first<<" "<<hostVec[l].second.second<<endl;
        // }
        // cout<<returnIndex<<endl;
        return returnIndex;
        
}


int main(){
    map<int, int> todayPurchasePlan;
    list<string> buyMsg;
    clock_t start, finish;
    start = clock();
    int count = 0;
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
        int lastDayPurchasedHostCount = purchasedHosts->size();
        //开始处理每天的请求
        for(int day = 0; day < dayCount; day++){
            //buyMsg.clear();
            //cout << "this is "<< day <<endl;
            
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

            vector<RequestInfo> requestInfos_backup(requestInfos);

            /*
             
                VMDeployment???
                输入：HostPlanning的结果
                    当天用户的所有请求
             
             */
            int lastDelRequestEndIndex = -1;
            //int lastAddRequestStartIndex = 0;
            for(int k = 0; k < requestCount; ++k)
            {
                if(k < requestCount - 1 && requestInfos[k].infoType == add)
                    continue;
                else
                {
                    int endpos;
                    if(k == requestCount - 1 && requestInfos[k].infoType == add)
                        endpos = k + 1;
                    else
                        endpos = k;
                    // 为连续的AddRequest进行部署，并处理后来的一个del请求。
                    // 如果最后一个请求是add，那么需要带上这个add请求一起排序和处理。
                    //cout << "turn start"<<endl;

                    sort(requestInfos_backup.begin() + lastDelRequestEndIndex + 1, requestInfos_backup.begin() + endpos, vmResCmp);

                    for(int i = lastDelRequestEndIndex + 1; i < endpos; ++i)
                    {
                        if (requestInfos_backup[i].infoType == add)
                        {
                            bool isDuetOfVm = vms[requestInfos_backup[i].vmType].isDuet;
                        float cmRatioOfVm = (float)vms[requestInfos_backup[i].vmType].cpu / vms[requestInfos_backup[i].vmType].mm;
                        float mincmRatioDiff = 10000.0;
                        PurchasedHost* bestHost = nullptr;
                        int bestHostIndex = -1;
                        Node bestHostNode;          // A节点或B节点

                        // 尝试在已有的主机上部署
                        for(int j = 0; j < purchasedHosts->size(); ++j)
                        {
                            if (j>300&&j<purchasedHosts->size()*0.9) {
                                j = j + rand()%6;
                            }
                            if(isDuetOfVm && purchasedHosts->at(j).isDuet)
                            {
                                if(purchasedHosts->at(j).isRemainResourceAvailForBoth(vms[requestInfos_backup[i].vmType].cpu, vms[requestInfos_backup[i].vmType].mm))
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
                                if(purchasedHosts->at(j).isRemainResourceAvailForA(vms[requestInfos_backup[i].vmType].cpu, vms[requestInfos_backup[i].vmType].mm))
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
                                if(purchasedHosts->at(j).isRemainResourceAvailForB(vms[requestInfos_backup[i].vmType].cpu, vms[requestInfos_backup[i].vmType].mm))
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

                            int newHostIndex = getBestHost(requestInfos_backup,i,day,dayCount);
                            // cout<<newHostIndex<<endl;
                            // TODO
                            bestHost = new PurchasedHost(newHostIndex, hosts[newHostIndex].cpu, hosts[newHostIndex].mm);

                            purchasedHosts->push_back(*bestHost);


                            // delete bestHost;
                            
                            
                            // cout<<purchasedHosts->back().listDVMA->size();
                            // cout<<purchasedHosts->back().listDVMB->size();
                            // cout<<purchasedHosts->back().listDVMBoth->size();

                            bestHost = &purchasedHosts->back();
                            // cout<<purchasedHosts->back().listDVMA->size();
                            // cout<<purchasedHosts->back().listDVMB->size();
                            // cout<<purchasedHosts->back().listDVMBoth->size();
                            bestHostIndex = purchasedHosts->size() - 1;
                            //buyMsg.push_back("("+hosts[purchasedBestHostIndex].hostType+",1)\n");
        
                            if(isDuetOfVm)
                            {
                                bestHost->isDuet = true;
                                bestHost->addBoth(requestInfos_backup[i].vmType, bestHostIndex, day, requestInfos_backup[i].vmId);
                            }
                            else
                            {
                                bestHost->isDuet = false;
                                bestHost->addSingleA(requestInfos_backup[i].vmType, bestHostIndex, day, requestInfos_backup[i].vmId);
                            }
                        }

                        // TODO
                        // deploy
                        else
                        {
                            if(isDuetOfVm)
                                bestHost->addBoth(requestInfos_backup[i].vmType, bestHostIndex, day, requestInfos_backup[i].vmId);
                            else
                            {
                                if(bestHostNode == A)
                                    bestHost->addSingleA(requestInfos_backup[i].vmType, bestHostIndex, day, requestInfos_backup[i].vmId);
                                else if(bestHostNode == B)
                                    bestHost->addSingleB(requestInfos_backup[i].vmType, bestHostIndex, day, requestInfos_backup[i].vmId);
                            }
                            
                        }
                    }else{
                        purchasedHosts->at(vmIdToDeployedVM[requestInfos_backup[i].vmId]->host).delvm(*vmIdToDeployedVM[requestInfos[i].vmId]);

                        }
                        
                        //cout << "cpu="<< vms[requestInfos_backup[i].vmType].cpu <<",mm=" <<vms[requestInfos_backup[i].vmType].mm <<",sum="<<vms[requestInfos_backup[i].vmType].cpu + vms[requestInfos_backup[i].vmType].mm<<endl;
                        
                                                                   
                    }
                    if(endpos == k && requestInfos[k].infoType == del)   {
                        count++;
                        purchasedHosts->at(vmIdToDeployedVM[requestInfos[k].vmId]->host).delvm(*vmIdToDeployedVM[requestInfos[k].vmId]);

                    }   // 为了防止最后一条请求是add
                    lastDelRequestEndIndex = k;      // ！！！pay attention！！！
                    
                    //cout << "turn finish"<<endl;
                }
            }
            sort(purchasedHosts->begin() + lastDayPurchasedHostCount, purchasedHosts->end(), purchasedHostCmp);

            todayPurchasePlan.clear();
            for(int i = lastDayPurchasedHostCount; i < purchasedHosts->size(); ++i)
            {
                purchasedHosts->at(i).updateListsHostIndex(i);
                if(todayPurchasePlan.find(purchasedHosts->at(i).hostId) != todayPurchasePlan.end())
                    ++todayPurchasePlan[purchasedHosts->at(i).hostId];
                else
                    todayPurchasePlan[purchasedHosts->at(i).hostId] = 1;
            }

            lastDayPurchasedHostCount = purchasedHosts->size();

       

            printf("(purchase,%lu)\n", todayPurchasePlan.size());

            for (auto it = todayPurchasePlan.begin(); it != todayPurchasePlan.end(); it++)
            {
                printf("(%s, %d)\n", hosts[(*it).first].hostType.c_str(), (*it).second);
            }
            
            printf("(migration, 0)\n");
            outputDeploymentResult();
         }
        //  for (auto it  = purchasedHosts->begin(); it != purchasedHosts->end(); it++)
        //  {
        //      cout<<it->remainCpuA<<" "<<it->remainCpuB<<" "<<it->remainMmA<<" "<<it->remainMmB<<endl;
        //  }
         
        
    }
    else
    {
        cerr <<"file not found" << endl;
    }

    // fclose(stdin);
    finish = clock();
    cout<<count<<endl;
    printf("\nnumber of purchasedhost: %d \n", purchasedHosts->size());
    printf("\nuse time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
    printf("\nbreak point\n");
    return 0;
}

