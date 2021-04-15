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

#include "Output.h"
#include "DataStructure.h"
using namespace std;

float getCostPerFormance(float weight,vector<Host>& hosts,int index, int day, int totalday){
    return (hosts[index].cpu + weight*hosts[index].mm) / float(hosts[index].hostCost + hosts[index].dailyCost * (totalday-day));
}

float getDailyCostPerFormance(vector<PurchasedHost>* purchasedHosts, int purchasedHostIndex)
{
    return (float)(purchasedHosts->at(purchasedHostIndex).dailyCost) / (purchasedHosts->at(purchasedHostIndex).cpu + purchasedHosts->at(purchasedHostIndex).mm);
}

#pragma mark getBestPurchasedHost
int getBestPurchasedHost(float weight,vector<Host>& hosts,unordered_map <string, VmProperties>& vms,const vector<RequestInfo>& requestInfos, int i, int day, int totalDay, bool isDuetOfVm, float cmRatioOfVm){
    // cm比差距 index 性价比
    list<pair<float, pair<int,float>>> hostVec;
    for (int i = 10000; i < 10022; i++) {
        hostVec.push_back(pair<float,pair<int,float>>(i,pair<int,float>(-1,0)));
    }
    for(int j = 0; j < hosts.size(); ++j)
        {
            float thiscmRatioDiff = hosts[j].cmRatio > cmRatioOfVm ? hosts[j].cmRatio/cmRatioOfVm : cmRatioOfVm/hosts[j].cmRatio;
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
                            hostVec.insert(it,pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(weight,hosts,j,day,totalDay))));
                            break;
                        }
                    }
                    if (flag != true)
                    {
                        hostVec.push_back(pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(weight,hosts,j,day,totalDay))));
                    }
                    
                }
            }
            else
            {
                if(thiscmRatioDiff < hostVec.back().first&&hosts[j].cpu>=vms[requestInfos[i].vmType].cpu&&hosts[j].mm>=vms[requestInfos[i].vmType].mm)
                {
                    hostVec.pop_back();
                    bool flag = false;
                    for (auto it = hostVec.begin(); it != hostVec.end(); it++)
                    {
                        if (it->first > thiscmRatioDiff)
                        {
                            flag = true;
                            hostVec.insert(it,pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(weight,hosts,j,day,totalDay))));
                            break;
                        }
                        
                    }
                    if (flag != true)
                    {
                        hostVec.push_back(pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(weight,hosts,j,day,totalDay))));
                    }
                }
            }
            
        }
        int returnIndex = hostVec.front().second.first;
        float best = hostVec.front().second.second;
        for (auto it1 = hostVec.begin(); it1 != hostVec.end(); it1++)
        {
             if (it1->second.second > best)
            {
                best = it1->second.second;
                returnIndex = it1->second.first;
            }
        }
        
        // for (size_t l = 0; l < hostVec.size(); l++)
        // {
        //     if (hostVec[l].second.second > best)
        //     {
        //         best = hostVec[l].second.second;
        //         returnIndex = hostVec[l].second.first;
        //     }
        // }
        return returnIndex;
}

float getDiffOfAandB(vector<PurchasedHost> *purchasedHosts, pair<int, Node> test, int vmCpu, int vmMm)
{
    float AusedCpu = purchasedHosts->at(test.first).cpu / 2 - purchasedHosts->at(test.first).remainCpuA;
    float AusedMm = purchasedHosts->at(test.first).mm / 2 - purchasedHosts->at(test.first).remainMmA;
    float BusedCpu = purchasedHosts->at(test.first).cpu / 2 - purchasedHosts->at(test.first).remainCpuB;
    float BusedMm = purchasedHosts->at(test.first).mm / 2 - purchasedHosts->at(test.first).remainMmB;
    //cout << "hostOneSideCpu="<< purchasedHosts->at(test.first).cpu / 2 <<",hostOneSideMm=" << purchasedHosts->at(test.first).mm / 2 << endl;
    if(test.second == A)
    {
        //cout << "AremainCpu=" << purchasedHosts->at(test.first).remainCpuA << ",AremainMm=" << purchasedHosts->at(test.first).remainMmA << endl;
        // if(purchasedHosts->at(test.first).remainCpuA - vmCpu < 0 || purchasedHosts->at(test.first).remainMmA - vmMm < 0)
        // {
        //     cout << r << endl;
        //     cout << "hostOneSideCpu="<< purchasedHosts->at(test.first).cpu / 2 <<",hostOneSideMm=" << purchasedHosts->at(test.first).mm / 2 << endl;
        //     cout << "AremainCpu=" << purchasedHosts->at(test.first).remainCpuA << ",AremainMm=" << purchasedHosts->at(test.first).remainMmA << endl;
        // }
        return fabs(AusedCpu + vmCpu + AusedMm + vmMm - BusedCpu - BusedMm) / (purchasedHosts->at(test.first).cpu + purchasedHosts->at(test.first).mm);
    }
        
    else    // if(test.second == B)
    {
        //cout << "BremainCpu=" << purchasedHosts->at(test.first).remainCpuB << ",BremainMm=" << purchasedHosts->at(test.first).remainMmB << endl;
        // if(purchasedHosts->at(test.first).remainCpuB - vmCpu < 0 || purchasedHosts->at(test.first).remainMmB - vmMm < 0)
        // {
        //     cout << r << endl;
        //     cout << "hostOneSideCpu="<< purchasedHosts->at(test.first).cpu / 2 <<",hostOneSideMm=" << purchasedHosts->at(test.first).mm / 2 << endl;
        //     cout << "BremainCpu=" << purchasedHosts->at(test.first).remainCpuB << ",BremainMm=" << purchasedHosts->at(test.first).remainMmB << endl;
        // }
        return fabs(BusedCpu + vmCpu + BusedMm + vmMm - AusedCpu - AusedMm) / (purchasedHosts->at(test.first).cpu + purchasedHosts->at(test.first).mm);
    }
}

#pragma mark getBestExistingHost
pair<int, Node> getBestExistingHost(int vmCpu, int vmMm, bool isDuetOfVm, float cmRatioOfVm, vector<PurchasedHost>* purchasedHosts) //, vector<pair<int, Node>>& budgetPurchasedHosts, bool& isUseEmptyHost)
{
    // int size = 100000;

    // 在该函数中
    // notEmptyBestHost == -2 表示没有可用的非空主机，这时不得不开机使用一台新主机，以尽可能避免买新主机
    // EmptyBestHost == -3 表示没有可用的空主机
    // returnResult.first == -1 表示没有可用主机，只能买新主机了

    // pair<int, Node> notEmptyBestHost(-2,A);
    // pair<int, Node> emptyBestHost(-3,A);
    // vector<pair<int, Node>> budgetNotEmptyPurchasedHosts;    // 仅用于非空主机，找其中CPU/MM最接近的
    // 主机下标，节点，剩余资源数，cpu/mm比与虚拟机的差值
    list<pair<pair<int, Node>, pair<int, float>>> budgetNotEmptyPurchasedHosts;    // 仅用于非空主机，找其中CPU/MM最接近的
    // int budgetNotEmptySize = purchasedHosts->size()/ 2000 + 1;
    int budgetNotEmptySize = 2;
    int notEmptyRemainSourceBound = 100000000;
    float notEmptycmRatioDiffBound = 1000000.0;
    // 主机下标，cpu/mm比与虚拟机的差值
    list<pair<int, float>> budgetEmptyPurchasedHosts;    // 用于空主机，用于选出当天开机性价比较高的主机
    int budgetEmptySize = 5;
    float emptycmRatioDiffBound = 1000000.0;
    // 在将主机压入容器时，就是性能从差到优的顺序排序的，性能最优的就在容器的末端
    for(int j = 0; j < purchasedHosts->size(); ++j)
    {
        if(isDuetOfVm)
        {
            // duet deployment
            if(purchasedHosts->at(j).isRemainResourceAvailForBoth(vmCpu, vmMm))
            {
                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioBoth > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioBoth/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioBoth;
                // float thiscmRatioDiffForA = purchasedHosts->at(j).remainCMRatioA > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioA / cmRatioOfVm : cmRatioOfVm / purchasedHosts->at(j).remainCMRatioA;
                // float thiscmRatioDiffForB = purchasedHosts->at(j).remainCMRatioB > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioB / cmRatioOfVm : cmRatioOfVm / purchasedHosts->at(j).remainCMRatioB;
                // float thiscmRatioDiff = (thiscmRatioDiffForA + thiscmRatioDiffForB) / 2;
                if(purchasedHosts->at(j).numOfVm > 0)
                {
                    int thisRemainResource = purchasedHosts->at(j).remainSize;
                    if(thiscmRatioDiff < notEmptycmRatioDiffBound)
                    {
                        if(budgetNotEmptyPurchasedHosts.size() >= budgetNotEmptySize)
                        {
                            budgetNotEmptyPurchasedHosts.pop_back();
                        }
                        list<pair<pair<int, Node>, pair<int, float>>>::iterator it;
                        for(it = budgetNotEmptyPurchasedHosts.begin(); it != budgetNotEmptyPurchasedHosts.end() && thiscmRatioDiff > it->second.second; ++it)
                        {
                            // 无需编写
                        }
                        budgetNotEmptyPurchasedHosts.insert(it, pair<pair<int, Node>, pair<int, float>>(pair<int, Node>(j, Both), pair<int, float>(thisRemainResource, thiscmRatioDiff)));
                        notEmptycmRatioDiffBound = budgetNotEmptyPurchasedHosts.back().second.second;
                    }
                }
                else if(purchasedHosts->at(j).numOfVm == 0)
                {
                    if(thiscmRatioDiff < emptycmRatioDiffBound)
                    {
                        if(budgetEmptyPurchasedHosts.size() >= budgetEmptySize) 
                        {
                            budgetEmptyPurchasedHosts.pop_back();
                        }
                        list<pair<int, float>>::iterator it;
                        for(it = budgetEmptyPurchasedHosts.begin(); it != budgetEmptyPurchasedHosts.end() && thiscmRatioDiff > it->second; ++it)
                        {
                            // 无需编写
                        }
                        budgetEmptyPurchasedHosts.insert(it, pair<int, float>(j, thiscmRatioDiff));
                        emptycmRatioDiffBound = budgetEmptyPurchasedHosts.back().second;
                    }
                }
            }
        }
        else
        {
            // single deployment
            if(purchasedHosts->at(j).isRemainResourceAvailForA(vmCpu, vmMm))
            {
                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioA > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioA/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioA;
                if(purchasedHosts->at(j).numOfVm > 0)
                {
                    int thisRemainResource = purchasedHosts->at(j).remainSizeA;
                    if(thiscmRatioDiff < notEmptycmRatioDiffBound)
                    {
                        if(budgetNotEmptyPurchasedHosts.size() >= budgetNotEmptySize)
                        {
                            budgetNotEmptyPurchasedHosts.pop_back();
                        }
                        list<pair<pair<int, Node>, pair<int, float>>>::iterator it;
                        for(it = budgetNotEmptyPurchasedHosts.begin(); it != budgetNotEmptyPurchasedHosts.end() && thiscmRatioDiff > it->second.second; ++it)
                        {
                            // 无需编写
                        }
                        budgetNotEmptyPurchasedHosts.insert(it, pair<pair<int, Node>, pair<int, float>>(pair<int, Node>(j, A), pair<int, float>(thisRemainResource, thiscmRatioDiff)));
                        notEmptycmRatioDiffBound = budgetNotEmptyPurchasedHosts.back().second.second;
                    }
                }
                else if(purchasedHosts->at(j).numOfVm == 0)
                {
                    if(thiscmRatioDiff < emptycmRatioDiffBound)
                    {
                        if(budgetEmptyPurchasedHosts.size() >= budgetEmptySize)
                        {
                            budgetEmptyPurchasedHosts.pop_back();
                        }
                        list<pair<int, float>>::iterator it;
                        for(it = budgetEmptyPurchasedHosts.begin(); it != budgetEmptyPurchasedHosts.end() && thiscmRatioDiff > it->second; ++it)
                        {
                            // 无需编写
                        }
                        budgetEmptyPurchasedHosts.insert(it, pair<int, float>(j, thiscmRatioDiff));
                        emptycmRatioDiffBound = budgetEmptyPurchasedHosts.back().second;
                    }
                }
            }
            if(purchasedHosts->at(j).isRemainResourceAvailForB(vmCpu, vmMm))
            {
                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioB > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioB/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioB;
                if(purchasedHosts->at(j).numOfVm > 0)
                {
                    int thisRemainResource = purchasedHosts->at(j).remainSizeB;
                    if(thiscmRatioDiff < notEmptycmRatioDiffBound)
                    {
                        if(budgetNotEmptyPurchasedHosts.size() >= budgetNotEmptySize)
                        {
                            budgetNotEmptyPurchasedHosts.pop_back();
                        }
                        list<pair<pair<int, Node>, pair<int, float>>>::iterator it;
                        for(it = budgetNotEmptyPurchasedHosts.begin(); it != budgetNotEmptyPurchasedHosts.end() && thiscmRatioDiff > it->second.second; ++it)
                        {
                            // 无需编写
                        }
                        budgetNotEmptyPurchasedHosts.insert(it, pair<pair<int, Node>, pair<int, float>>(pair<int, Node>(j, B), pair<int, float>(thisRemainResource, thiscmRatioDiff)));
                        notEmptycmRatioDiffBound = budgetNotEmptyPurchasedHosts.back().second.second;
                    }
                }
                else if(purchasedHosts->at(j).numOfVm == 0)
                {
                    if(thiscmRatioDiff < emptycmRatioDiffBound)
                    {
                        if(budgetEmptyPurchasedHosts.size() >= budgetEmptySize)
                        {
                            budgetEmptyPurchasedHosts.pop_back();
                        }
                        list<pair<int, float>>::iterator it;
                        for(it = budgetEmptyPurchasedHosts.begin(); it != budgetEmptyPurchasedHosts.end() && thiscmRatioDiff > it->second; ++it)
                        {
                            // 无需编写
                        }
                        budgetEmptyPurchasedHosts.insert(it, pair<int, float>(j, thiscmRatioDiff));
                        emptycmRatioDiffBound = budgetEmptyPurchasedHosts.back().second;
                    }
                }
            }
        }
    }

    pair<int, Node> returnResult(-1,A);

    if(budgetNotEmptyPurchasedHosts.size() > 0)
    {
        // float mincmRatioDiff = 10000.0;
        int minRemainResource = 100000000;
        for(auto it = budgetNotEmptyPurchasedHosts.begin(); it != budgetNotEmptyPurchasedHosts.end(); ++it)
        {
            if(it->second.first < minRemainResource)
            {
                minRemainResource = it->second.first;
                returnResult.first = it->first.first;
                returnResult.second = it->first.second;
            }
        }
    }
    else if(budgetEmptyPurchasedHosts.size() > 0)
    {
        returnResult.first = budgetEmptyPurchasedHosts.front().first;
        if(isDuetOfVm)
        {
            returnResult.second = Both;
        }
        else
        {
            returnResult.second = A;
        }
    }

    // bool isUseEmptyHost = false;
    // if(notEmptyBestHost.first != -2)
    // {
    //     returnResult.first = notEmptyBestHost.first;
    //     returnResult.second = notEmptyBestHost.second;
    //     isUseEmptyHost = false;
    // }
    // else if(emptyBestHost.first != -3)
    // {
    //     returnResult.first = emptyBestHost.first;
    //     returnResult.second = emptyBestHost.second;
    //     isUseEmptyHost = true;
    // }

    // if(!isDuetOfVm && !isUseEmptyHost && returnResult.first != -1)    // 使用了非空主机，要使A，B结点平衡
    // {    
    //     int standByHostCount = min(0, (int)budgetNotEmptyPurchasedHosts.size());
    //     float mincmRatioDiff = 10000.0;
    //     for(int r = budgetNotEmptyPurchasedHosts.size() - standByHostCount; r < budgetNotEmptyPurchasedHosts.size(); ++r)
    //     {
    //         float thiscmRatioDiff = budgetNotEmptyPurchasedHosts[r].second;
    //         if(thiscmRatioDiff < mincmRatioDiff)
    //         {
    //             mincmRatioDiff = thiscmRatioDiff;
    //             returnResult.first = budgetNotEmptyPurchasedHosts[r].first.first;
    //             returnResult.second = budgetNotEmptyPurchasedHosts[r].first.second;
    //         }
    //     }
    // }

    // if(isUseEmptyHost)    // 使用空主机，要考虑开机的性价比
    // {
    //     int standByHostCount = min(5, (int)budgetEmptyPurchasedHosts.size());
    //     float minDailyCostPerFormance = 100000000.0;
    //     for(int r = budgetEmptyPurchasedHosts.size() - standByHostCount; r < budgetEmptyPurchasedHosts.size(); ++r)
    //     {
    //         float thisDailyCostPerFormance = getDailyCostPerFormance(purchasedHosts, budgetEmptyPurchasedHosts[r]);
    //         if(thisDailyCostPerFormance < minDailyCostPerFormance)
    //         {
    //             minDailyCostPerFormance = thisDailyCostPerFormance;
    //             returnResult.first = budgetEmptyPurchasedHosts[r];
    //         }
    //     }
    //     if(isDuetOfVm)
    //     {
    //         returnResult.second = Both;
    //     }
    //     else
    //     {
    //         returnResult.second = A;
    //     }
    // }

    return returnResult;
}



#pragma mark main
int main(){
    
#pragma mark INPUT_DATASTRUCTURE
    vector<Host> hosts;
    //虚拟机类型到信息的映射
    unordered_map <string, VmProperties> vms;
    int dayCount = 0, requestCount = 0, K_DayCount = 0;
    vector<RequestInfo> requestInfos;
    vector<RequestInfo> requestInfos_backup;

    /////
    float cpuR = 100;
    float MmR = 100;
    
    string tmp, infoType, vmType, vmId;
    
#pragma mark RUNTIME_DATASTRUCTURE
    map<int, int> todayPurchasePlan;
    int count = 0;
    list<pair<int,pair<int,Node>>> migrateInfos;
    int totalNumOfMigrate = 0;
    int addCount=0;
    int currentNumVms=0;

    vector<PurchasedHost> a;
    //已购买的主机
    vector<PurchasedHost>* purchasedHosts = &a;
    //虚拟机id到其节点的映射
    unordered_map <int, DeployedVM*> vmIdToDeployedVM;
    
    bool isDuetOfVm;
    float cmRatioOfVm, mincmRatioDiff;
    PurchasedHost* bestHost;
    int bestHostIndex;
    Node bestHostNode;
    pair<int, Node> bestHostPair;
    
 #ifndef LOCAL
//   提交时注释掉
    // clock_t start, finish;
    // start = clock();
    // const string filePath = "E:\\project\\codecraftfile\\training-1.txt";
    // //输入
    // if(freopen(filePath.c_str(),"rb", stdin)){
 #endif
    //解析主机和虚拟机信息
    getHostsAndVms(hosts, vms);
    //解析用户请求天数
    scanf("%d",&dayCount);
    scanf("%d",&K_DayCount);
    int lastDayPurchasedHostCount = purchasedHosts->size();
    
    //开始处理每天的请求
    for(int day = 0; day < dayCount; day++){
        currentNumVms += addCount;
        addCount = 0;
        migrateInfos.clear();
        if(cpuR >0.055 || MmR > 0.055){
            migrantBeforeDeployed((currentNumVms*3)/100, *purchasedHosts, vms, migrateInfos, vmIdToDeployedVM);
        }
        //   float ratio = 1;
        //     if(cpuR+MmR<0.2){
        //         ratio = (cpuR+MmR)*50;
        //     }
        //     if (cpuR<0.04||MmR<0.04)
        //     {
        //         ratio = 0.01;
        //     }
        //     migrantBeforeDeployed((currentNumVms*3)/100*1, *purchasedHosts, vms, migrateInfos, vmIdToDeployedVM);
        scanf("%d",&requestCount);
        requestInfos.clear();
        requestInfos_backup.clear();
        
        for(int i = 0; i < requestCount; i++){
            cin>>infoType;
            if(infoType[1] == 'a'){
                addCount++;
                cin>>vmType>>vmId;
                generateRequest(requestInfos, requestInfos_backup, vmType, vmId);
                //++todayIncreaseVmCount;
            }
            else{
                addCount--;
                cin>>vmId;
                generateRequest(requestInfos, requestInfos_backup, vmId);
                //--todayIncreaseVmCount;
            }
        }
        
        

        int lastDelRequestEndIndex = -1;
        for(int k = 0; k < requestCount; ++k)
        {
            //找del
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
                sort(requestInfos_backup.begin() + lastDelRequestEndIndex + 1, requestInfos_backup.begin() + endpos, vmResCmp);

                for(int i = lastDelRequestEndIndex + 1; i < endpos; ++i)
                {
                    if (requestInfos_backup[i].infoType == add)
                    {
                        isDuetOfVm = vms[requestInfos_backup[i].vmType].isDuet;
                        cmRatioOfVm = ((float)vms[requestInfos_backup[i].vmType].cpu / vms[requestInfos_backup[i].vmType].mm);
                        // mincmRatioDiff = 10000.0;
                        bestHost = nullptr;
                        //bestHostIndex = -1;
                        //bestHostNode = A;
                        // vector<pair<int, Node>> budgetPurchasedHosts;     // 使A、B节点部署平衡
                        
                        // 尝试在已有的主机上部署
                        bestHostPair = getBestExistingHost(vms[requestInfos_backup[i].vmType].cpu, vms[requestInfos_backup[i].vmType].mm, isDuetOfVm, cmRatioOfVm, purchasedHosts); //, budgetPurchasedHosts, isUseEmptyHost);
                        bestHostIndex = bestHostPair.first;
                        bestHostNode = bestHostPair.second;
                       
                        //没找到，要买了    
                        if(bestHostIndex == -1)
                        {
                            
                            int newHostIndex = getBestPurchasedHost((1+(cpuR/MmR-1)*1.5),hosts,vms,requestInfos_backup, i, day, dayCount, isDuetOfVm, cmRatioOfVm);
                            bestHost = new PurchasedHost(newHostIndex, hosts[newHostIndex].cpu, hosts[newHostIndex].mm,hosts[newHostIndex].dailyCost);
                            purchasedHosts->push_back(*bestHost);
                            bestHost = &purchasedHosts->back();
                            bestHostIndex = purchasedHosts->size() - 1;
        
                            if(isDuetOfVm)
                            {
                                // bestHost->isDuet = true;
                                bestHost->addBoth(requestInfos_backup[i].vmType, bestHostIndex, requestInfos_backup[i].vmId, day, vmIdToDeployedVM, vms);
                                // migrationBoth();
                            }
                            else
                            {
                                // bestHost->isDuet = false;
                                bestHost->addSingleA(requestInfos_backup[i].vmType, bestHostIndex, requestInfos_backup[i].vmId, day, vmIdToDeployedVM, vms);
                                // migrationSingle();
                            }

                        }
                        // deploy
                        else
                        {
                            bestHost = &purchasedHosts->at(bestHostIndex);
                            if(isDuetOfVm)
                                bestHost->addBoth(requestInfos_backup[i].vmType, bestHostIndex, requestInfos_backup[i].vmId, day, vmIdToDeployedVM, vms);
                            else
                            {
                                
                                if(bestHostNode == A)
                                    bestHost->addSingleA(requestInfos_backup[i].vmType, bestHostIndex, requestInfos_backup[i].vmId, day, vmIdToDeployedVM, vms);
                                else if(bestHostNode == B)
                                    bestHost->addSingleB(requestInfos_backup[i].vmType, bestHostIndex, requestInfos_backup[i].vmId, day, vmIdToDeployedVM, vms);
                            }
                        }
                    }
                    // else{
                    //     purchasedHosts->at(vmIdToDeployedVM[requestInfos_backup[i].vmId]->host).delvm(*vmIdToDeployedVM[requestInfos[i].vmId]);

                    // }
                }
                if(endpos == k && requestInfos[k].infoType == del)
                {
                    count++;
                    purchasedHosts->at(vmIdToDeployedVM[requestInfos[k].vmId]->host).delvm(*vmIdToDeployedVM[requestInfos[k].vmId], vms);
                }// 为了防止最后一条请求是add
                lastDelRequestEndIndex = k;
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
            printf("(%s,%d)\n", hosts[(*it).first].hostType.c_str(), (*it).second);
        }
        totalNumOfMigrate+= migrateInfos.size();
          printf("(migration,%lu)\n",migrateInfos.size());
        for (auto it = migrateInfos.begin(); it != migrateInfos.end(); it++)
        {
            if (it->second.second == 0)
            {
                printf("(%d,%d,A)\n",it->first,it->second.first);
            }else if(it->second.second == 1){
                printf("(%d,%d,B)\n",it->first,it->second.first);
            }else{
                printf("(%d,%d)\n",it->first,it->second.first);

            }
        }
        
        outputDeploymentResult(requestInfos, vmIdToDeployedVM);

        // cout<<"day: "<<day<<endl;
        int remainCpu=0;
        int remainMm = 0;
        // int remainSource = 0; 
        // int source = 0;
        int cpu = 0;
        int mm = 0;
        int count1 =0;
        for (size_t i = 0; i < purchasedHosts->size(); i++)
        {
            if (purchasedHosts->at(i).numOfVm ==0)
            {
                count1++;
                continue;
                /* code */
            }
            
            remainCpu += purchasedHosts->at(i).remainCpuA+purchasedHosts->at(i).remainCpuB;
            cpu += purchasedHosts->at(i).cpu;
            remainMm += purchasedHosts->at(i).remainMmA+purchasedHosts->at(i).remainMmB;
            mm += purchasedHosts->at(i).mm;
            // remainSource += purchasedHosts->at(i).remainSize;
            // source += purchasedHosts->at(i).
                   // cout<<"number: "<<i<<endl;
            // cout<<((float)(purchasedHosts->at(i).remainCpuA+purchasedHosts->at(i).remainCpuB))/purchasedHosts->at(i).cpu<<endl;
            // cout<<((float)(purchasedHosts->at(i).remainMmA+purchasedHosts->at(i).remainMmB))/purchasedHosts->at(i).mm<<endl;

        }
        cpuR = (float)remainCpu/cpu;
        MmR = (float)remainMm/mm;
        // cout<<"null:"<<count1<<endl;
        // cout<<cpuR<<endl<<MmR<<endl;
        
     }
  
 #ifndef LOCAL
    // }
    // finish = clock();
    // printf("\nnumber of purchasedhost: %lu \n", purchasedHosts->size());
    // printf("\nnumber of migrate: %d \n", totalNumOfMigrate);
    // printf("\nuse time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
 #endif

    return 0;
}

