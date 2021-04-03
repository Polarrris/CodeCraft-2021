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

float getCostPerFormance(vector<Host>& hosts,int index, int day, int totalday){
    return (hosts[index].cpu + hosts[index].mm) / float(hosts[index].hostCost + hosts[index].dailyCost * (totalday-day));
}

#pragma mark getBestPurchasedHost
int getBestPurchasedHost(vector<Host>& hosts,unordered_map <string, VmProperties>& vms,const vector<RequestInfo>& requestInfos, int i, int day, int totalDay, bool isDuetOfVm, float cmRatioOfVm){
    // cm比差距 index 性价比
    vector<pair<float, pair<int,float>>> hostVec;
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
                            hostVec.insert(it,pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(hosts,j,day,totalDay))));
                            break;
                        }
                    }
                    if (flag != true)
                    {
                        hostVec.push_back(pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(hosts,j,day,totalDay))));
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
                            hostVec.insert(it,pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(hosts,j,day,totalDay))));
                            break;
                        }
                        
                    }
                    if (flag != true)
                    {
                        hostVec.push_back(pair<float,pair<int,float>>(thiscmRatioDiff,pair<int,float>(j,getCostPerFormance(hosts,j,day,totalDay))));
                    }
                }
            }
            
        }
        int returnIndex = hostVec[0].second.first;
        float best = hostVec[0].second.second;
        for (size_t l = 0; l < hostVec.size(); l++)
        {
            if (hostVec[l].second.second > best)
            {
                best = hostVec[l].second.second;
                returnIndex = hostVec[l].second.first;
            }
        }
        return returnIndex;
}

#pragma mark getBestExistingHost
pair<int, Node> getBestExistingHost(int vmCpu, int vmMm, bool isDuetOfVm, float cmRatioOfVm, float mincmRatioDiff,  vector<PurchasedHost>* purchasedHosts, vector<pair<int, Node>>& budgetPurchasedHosts)
{
    pair<int, Node> returnResult(-1,A);
    for(int j = 0; j < purchasedHosts->size(); ++j)
    {
//                                if (j>300&&j<purchasedHosts->size()*0.9) {
//                                    j = j + rand()%6;
//                                }
        if(isDuetOfVm)
        {
            // duet deployment
            if(purchasedHosts->at(j).isRemainResourceAvailForBoth(vmCpu, vmMm))
            {
                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioBoth > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioBoth/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioBoth;
                if(thiscmRatioDiff < mincmRatioDiff)
                {
                    mincmRatioDiff = thiscmRatioDiff;
                    //bestHost = &purchasedHosts->at(j);
                    returnResult.first = j;
                    returnResult.second = Both;
                }
            }
        }
        else
        {
            // single deployment
            if(purchasedHosts->at(j).isRemainResourceAvailForA(vmCpu, vmMm))
            {
                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioA > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioA/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioA;
                if(thiscmRatioDiff < mincmRatioDiff)
                {
                    mincmRatioDiff = thiscmRatioDiff;
                    //bestHost = &purchasedHosts->at(j);
                    returnResult.first = j;
                    returnResult.second = A;
                    budgetPurchasedHosts.push_back(pair<int, Node>(j, A));
                }
            }
            if(purchasedHosts->at(j).isRemainResourceAvailForB(vmCpu, vmMm))
            {
                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioB > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioB/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioB;
                if(thiscmRatioDiff < mincmRatioDiff)
                {
                    mincmRatioDiff = thiscmRatioDiff;
                    //bestHost = &purchasedHosts->at(j);
                    returnResult.first = j;
                    returnResult.second = B;
                    budgetPurchasedHosts.push_back(pair<int, Node>(j, B));
                }
            }
        }
    }
    return returnResult;
}

float getDiffOfAandB(vector<PurchasedHost> *purchasedHosts, int r, pair<int, Node> test, int vmCpu, int vmMm)
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

// void migrationBoth()
// {

// }

// void migrationSingle()
// {
    
// }


#pragma mark main
int main(){
    
#pragma mark INPUT_DATASTRUCTURE
    vector<Host> hosts;
    //虚拟机类型到信息的映射
    unordered_map <string, VmProperties> vms;
    int dayCount = 0, requestCount = 0, K_DayCount = 0;
    vector<RequestInfo> requestInfos;
    vector<RequestInfo> requestInfos_backup;
    
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
    clock_t start, finish;
    start = clock();
    const string filePath = "/Users/jesse/Desktop/CodeCraft/training-data/training-1.txt";
    //输入
    if(freopen(filePath.c_str(),"rb", stdin)){
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
        migrantBeforeDeployed((currentNumVms*5)/1000, *purchasedHosts, vms, migrateInfos, vmIdToDeployedVM);
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
                        mincmRatioDiff = 10000.0;
                        bestHost = nullptr;
                        //bestHostIndex = -1;
                        //bestHostNode = A;
                        vector<pair<int, Node>> budgetPurchasedHosts;     // 使A、B节点部署平衡
                        

                        // 尝试在已有的主机上部署
                        bestHostPair = getBestExistingHost(vms[requestInfos_backup[i].vmType].cpu, vms[requestInfos_backup[i].vmType].mm, isDuetOfVm, cmRatioOfVm, mincmRatioDiff, purchasedHosts, budgetPurchasedHosts);
                        bestHostIndex = bestHostPair.first;
                        bestHostNode = bestHostPair.second;
                       
                        //没找到，要买了
                        if(bestHostIndex == -1)
                        {
                            
                            int newHostIndex = getBestPurchasedHost(hosts,vms,requestInfos_backup, i, day, dayCount, isDuetOfVm, cmRatioOfVm);
                            bestHost = new PurchasedHost(newHostIndex, hosts[newHostIndex].cpu, hosts[newHostIndex].mm);
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
                                int standByHostCount = min(5, (int)budgetPurchasedHosts.size());
                                float minDiffOfAandB = 1.00;
                                //cout << "turn start" << endl;
                                //cout << "vmCpu=" << vms[requestInfos_backup[i].vmType].cpu << ",vmMm=" << vms[requestInfos_backup[i].vmType].mm << endl;
                                for(int r = budgetPurchasedHosts.size() - standByHostCount; r < budgetPurchasedHosts.size(); ++r)
                                {
                                    //cout << r << endl;
                                    float thisDiffOfAandB = getDiffOfAandB(purchasedHosts, r, budgetPurchasedHosts[r], vms[requestInfos_backup[i].vmType].cpu, vms[requestInfos_backup[i].vmType].mm);
                                    if(thisDiffOfAandB < minDiffOfAandB)
                                    {
                                        minDiffOfAandB = thisDiffOfAandB;
                                        bestHostIndex = budgetPurchasedHosts[r].first;
                                        bestHost = &purchasedHosts->at(bestHostIndex);
                                        bestHostNode = budgetPurchasedHosts[r].second;
                                    }
                                }
                                //cout << "turn finish" << endl;

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
     }
  
 #ifndef LOCAL
    }
    finish = clock();
    printf("\nnumber of purchasedhost: %lu \n", purchasedHosts->size());
    printf("\nnumber of migrate: %d \n", totalNumOfMigrate);
    printf("\nuse time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
 #endif
    return 0;
}

