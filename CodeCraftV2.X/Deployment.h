//
//  Deployment.h
//  CodeCraftV2.X
//
//  Created by Jesse Chou on 2021/3/22.
//

#ifndef Deployment_h
#define Deployment_h

using namespace std;
#include "Input.h"
#include "PurchasePlanToday.h"
#pragma mark DeployedVM
class DeployedVM {
    
public:
    string type;
//    int date;
    int hostIndex;
    Node deployedNode;
    int vmId;
    
    DeployedVM(const DeployedVM& deployedVM){
        this->type = deployedVM.type;
//        this->date = deployedVM.date;
        this->hostIndex = deployedVM.hostIndex;
        this->deployedNode = deployedVM.deployedNode;
        this->vmId = deployedVM.vmId;
    }

    DeployedVM(string type, int host, Node deployedNode, int vmId){
        this->type = type;
//        this->date = date;
        this->hostIndex = host;
        this->deployedNode = deployedNode;
        this->vmId = vmId;
    }
    friend bool operator == (const DeployedVM& first, const DeployedVM& second)
        {
            return first.vmId == second.vmId;
        }
};

//虚拟机id到其节点的映射
unordered_map <int, DeployedVM*> vmIdToDeployedVM;


#pragma mark PurchasedHost
class PurchasedHost {
public:
    int hostId;     // 在所给小于等于100台可选物理机中的下标
    int cpu;
    int mm;
    int remainCpuA;
    int remainMmA;
    int remainCpuB;
    int remainMmB;
    float remainCMRatioA;
    float remainCMRatioB;
    float remainCMRatioBoth;
    
    
    list<DeployedVM>* listDVMA;
    list<DeployedVM>* listDVMB;
    list<DeployedVM>* listDVMBoth;

    PurchasedHost(const PurchasedHost& purchasedHost){
        this->hostId = purchasedHost.hostId;
        this->remainCpuA = purchasedHost.remainCpuA;
        this->remainCpuB = purchasedHost.remainCpuB;
        this->remainMmA = purchasedHost.remainMmA;
        this->remainMmB = purchasedHost.remainMmB;
        this->cpu = purchasedHost.cpu;
        this->mm = purchasedHost.mm;
        this->remainCMRatioA = purchasedHost.remainCMRatioA;
        this->remainCMRatioB = purchasedHost.remainCMRatioB;
        this->remainCMRatioBoth = purchasedHost.remainCMRatioBoth;
        this->listDVMA = purchasedHost.listDVMA;
        this->listDVMB = purchasedHost.listDVMB;
        this->listDVMBoth = purchasedHost.listDVMBoth;
    }
    
    PurchasedHost(int _hostId, int _cpu, int _mm){
        hostId = _hostId;
        cpu = _cpu;
        mm = _mm;
        remainCpuA = cpu/2;
        remainCpuB = cpu/2;
        remainMmA = mm/2;
        remainMmB = mm/2;
        remainCMRatioBoth = remainCMRatioA = remainCMRatioB = (float)cpu / mm;
        listDVMA = new list<DeployedVM>();
        listDVMB = new list<DeployedVM>();
        listDVMBoth = new list<DeployedVM>();
    }
    
    bool isFull(){
        if (remainCpuA == 0 && remainCpuB == 0 && remainMmA == 0 && remainMmB == 0) {
            return true;
        }
        return false;
    }
    
    void addSingleA(DeployedVM& dvm){
        listDVMA->push_back(dvm);
        vmIdToDeployedVM[dvm.vmId] = &listDVMA->back();
    }
    void addSingleB(DeployedVM& dvm){
        listDVMB->push_back(dvm);
        vmIdToDeployedVM[dvm.vmId] = &listDVMB->back();
    }
    void addBoth(DeployedVM& dvm){
        listDVMBoth->push_back(dvm);
        vmIdToDeployedVM[dvm.vmId] = &listDVMBoth->back();
    }
    void generateDeployedVMIndex(){
        for (auto it = listDVMA->begin(); it != listDVMA->end(); it++) {
            vmIdToDeployedVM[(*it).vmId] = &*it;
        }
        for (auto it = listDVMB->begin(); it != listDVMB->end(); it++) {
            vmIdToDeployedVM[(*it).vmId] = &*it;
        }
        for (auto it = listDVMBoth->begin(); it != listDVMBoth->end(); it++) {
            vmIdToDeployedVM[(*it).vmId] = &*it;
        }
    }
    void updateRemainResAfterPurchase(PlanningVM& _planningVM){
        remainCpuA = remainCpuA - _planningVM.cpuA;
        remainCpuB = remainCpuB - _planningVM.cpuB;
        remainMmA = remainMmA - _planningVM.mmA;
        remainMmB = remainMmB - _planningVM.mmB;
        //未更新CMRatio
    }
    
    void updateRemainResBeforeDelete(DeployedVM& vm){
            string type = vm.type;
            Node _node = vm.deployedNode;
            int cpuNum = vms[type].cpu;
            int mmNum = vms[type].mm;
            if (_node == A)
            {
                remainCpuA += cpuNum;
                remainMmA += mmNum;
                remainCMRatioA = (float)remainCpuA / remainMmA;
            }else if(_node == B){
                remainCpuB += cpuNum;
                remainMmB += mmNum;
                remainCMRatioB = (float)remainCpuB / remainMmB;
            }else{
                remainCpuA += cpuNum/2;
                remainMmA += mmNum/2;
                remainCpuB += cpuNum/2;
                remainMmB += mmNum/2;
                remainCMRatioBoth = (float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB);
            }
        }

    void delvm(DeployedVM& deletedVM)
    {
    updateRemainResBeforeDelete(deletedVM);
    if(deletedVM.deployedNode == A)
    listDVMA->remove(deletedVM);
    else if(deletedVM.deployedNode == B)
    listDVMB->remove(deletedVM);
    else
    listDVMBoth->remove(deletedVM);
    }

//    bool isRemainResourceAvailForA(int _cpu, int _mm)
//    {
//        if((_cpu <= remainCpuA) && (_mm <= remainMmA))
//            return true;
//        else
//            return false;
//    }
//
//    bool isRemainResourceAvailForB(int _cpu, int _mm)
//    {
//        if((_cpu <= remainCpuB) && (_mm <= remainMmB))
//            return true;
//        else
//            return false;
//    }
//
//    bool isRemainResourceAvailForBoth(int _cpu, int _mm)
//    {
//        if((_cpu / 2 <= remainCpuA) && (_mm / 2 <= remainMmA) && (_cpu / 2 <= remainCpuB) && (_mm / 2 <= remainMmB))
//            return true;
//        else
//            return false;
//    }
    
    void updateListsHostIndex(int afterHostId)
    {
        for(auto It = listDVMA->begin(); It!= listDVMA->end();It++){
            It->hostIndex = afterHostId;
        }
        for(auto It = listDVMB->begin(); It!= listDVMB->end();It++){
            It->hostIndex = afterHostId;
        }
        for(auto It = listDVMBoth->begin(); It!= listDVMBoth->end();It++){
            It->hostIndex = afterHostId;
        }
    }
};

//已购买的主机
vector<PurchasedHost> purchasedHosts;

//purchasedHost比较函数
bool static purchasedHostCmp(const PurchasedHost& first, const PurchasedHost& second)
{
    return first.hostId < second.hostId;
}



#pragma mark vmToDeployedVM
DeployedVM vmToDeployedVM(VM& _vm, int hostId){
    if (_vm.planNode == A)
        return DeployedVM(_vm.type, hostId, A, _vm.vmId);
    else if (_vm.planNode == B)
        return DeployedVM(_vm.type, hostId, B, _vm.vmId);
    else
        return DeployedVM(_vm.type, hostId, Both, _vm.vmId);
}

#pragma mark printPurchaseToday
void printPurchaseToday(int yesterdayPurchasedHostNext){
    map<int, int> todayPurchasePlan;
    sort(purchasedHosts.begin() + yesterdayPurchasedHostNext, purchasedHosts.end(), purchasedHostCmp);
    for(int i = yesterdayPurchasedHostNext; i < purchasedHosts.size(); ++i)
    {
        purchasedHosts.at(i).updateListsHostIndex(i);
        if(todayPurchasePlan.find(purchasedHosts.at(i).hostId) != todayPurchasePlan.end())
            ++todayPurchasePlan[purchasedHosts.at(i).hostId];
        else
            todayPurchasePlan[purchasedHosts.at(i).hostId] = 1;
    }
    printf("(purchase,%lu)\n", todayPurchasePlan.size());

    for (auto it = todayPurchasePlan.begin(); it != todayPurchasePlan.end(); it++)
    {
        printf("(%s, %d)\n", hosts[(*it).first].hostType.c_str(), (*it).second);
    }
}

#pragma mark purchaseAndDeployByPlan
int purchaseAndDeployByPlan(int yesterdayPurchasedHostNext){
    int todayCount = yesterdayPurchasedHostNext;
    for (auto it = planningVMs.begin(); it != planningVMs.end(); ++it) {
        PurchasedHost _purchasedHost(hostTypeToIndex[(*it).plannedHost.hostType], (*it).plannedHost.cpu, (*it).plannedHost.mm);
        for (auto it2 = (*it).planVMs.begin(); it2 != (*it).planVMs.end(); ++it2) {
            if ((*it2).planNode == A){
                _purchasedHost.listDVMA->push_back(vmToDeployedVM(*it2, todayCount));}
            else if((*it2).planNode == B)
                _purchasedHost.listDVMB->push_back(vmToDeployedVM(*it2, todayCount));
            else
                _purchasedHost.listDVMBoth->push_back(vmToDeployedVM(*it2, todayCount));
        }
        _purchasedHost.updateRemainResAfterPurchase((*it));
printf("%4.d %4.d %4.d %4.d\n", _purchasedHost.remainCpuA, _purchasedHost.remainCpuB, _purchasedHost.remainMmA, _purchasedHost.remainMmB);
        purchasedHosts.push_back(_purchasedHost);
        purchasedHosts.back().generateDeployedVMIndex();
        todayCount++;
    }
    printPurchaseToday(yesterdayPurchasedHostNext);
    return planningVMs.size();
}

#pragma mark handelDelRequest
void handelDelRequest(){
    
}

#pragma mark printDeploymentResult
void printDeploymentResult(){
    for(auto it = requestInfos.begin(); it != requestInfos.end(); it++){
        if (it->infoType == add)
        {
            if(vmIdToDeployedVM[it->vmId]->deployedNode == A){
                printf("(%d,A)\n",vmIdToDeployedVM[it->vmId]->hostIndex);

            }else if (vmIdToDeployedVM[it->vmId]->deployedNode == B)
            {
                printf("(%d,B)\n",vmIdToDeployedVM[it->vmId]->hostIndex);
            }else{
                printf("(%d)\n",vmIdToDeployedVM[it->vmId]->hostIndex);
            }
        }
    }
}


#endif /* Deployment_h */
