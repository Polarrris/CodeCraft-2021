//
//  DataStructure.h
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/15.
//

#ifndef DataStructure_h
#define DataStructure_h
using namespace std;
#include "Input.h"

#pragma mark DeployedVM

class DeployedVM {
    
public:
    string type;
    int host;
    Node deployedNode;
    int vmId;
    int day;
    
    DeployedVM(const DeployedVM& deployedVM){
        this->type = deployedVM.type;
        this->host = deployedVM.host;
        this->deployedNode = deployedVM.deployedNode;
        this->vmId = deployedVM.vmId;
        this->day = deployedVM.day;
    }

    DeployedVM(string type, int host, Node deployedNode, int vmId,int day){
        this->type = type;
        this->host = host;
        this->deployedNode = deployedNode;
        this->vmId = vmId;
        this->day = day;
    }
    friend bool operator == (const DeployedVM& first, const DeployedVM& second)
        {
            return first.vmId == second.vmId;
        }
};


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
    int numOfVm;
    int dailyCost;
    int remainSize;
    int remainSizeA;
    int remainSizeB;
    
    bool isDuet;    //购买时决定
    
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
        this->isDuet = purchasedHost.isDuet;
        this->listDVMA = purchasedHost.listDVMA;
        this->listDVMB = purchasedHost.listDVMB;
        this->listDVMBoth = purchasedHost.listDVMBoth;
        this->numOfVm = purchasedHost.numOfVm;
        this->dailyCost = purchasedHost.dailyCost;
        this->remainSize = purchasedHost.remainSize;
        this->remainSizeA = purchasedHost.remainSizeA;
        this->remainSizeB = purchasedHost.remainSizeB;
    }
    
    PurchasedHost(int _hostId, int cpu, int mm, int dailyCost){
        hostId = _hostId;
        this->cpu = cpu;
        this->mm = mm;
        remainCpuA = cpu/2;
        remainCpuB = cpu/2;
        remainMmA = mm/2;
        remainMmB = mm/2;
        this->remainSizeA = remainCpuA+remainMmA;
        this->remainSizeB = remainCpuB+remainMmB;
        remainCMRatioBoth = remainCMRatioA = remainCMRatioB = ((float)cpu / mm);
        listDVMA = new list<DeployedVM>();
        listDVMB = new list<DeployedVM>();
        listDVMBoth = new list<DeployedVM>();
        this->numOfVm = 0;
        this->dailyCost = dailyCost;
        this->remainSize = cpu+mm;
    }
    
    void addSingleA(string type, int index, int vmId, int day, unordered_map <int, DeployedVM*>& vmIdToDeployedVM, unordered_map<string, VmProperties>& vms){
        remainCpuA = remainCpuA - vms[type].cpu;
        remainMmA = remainMmA - vms[type].mm;
        this->remainSizeA = this->remainCpuA+this->remainMmA;
        this->remainSize = this->remainSize - vms[type].cpu- vms[type].mm;
        // if(remainCpuA < 0 || remainMmA < 0)
        //     cout << "addSingleA error!!!"<<endl;
        DeployedVM tmpVM(type, index, A, vmId,day);
        listDVMA->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMA->front();
        remainCMRatioA = ((float)remainCpuA / remainMmA);
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
        ++this->numOfVm;
    }
    
    void addSingleB(string type, int index, int vmId, int day, unordered_map <int, DeployedVM*>& vmIdToDeployedVM, unordered_map<string, VmProperties>& vms){
        remainCpuB = remainCpuB - vms[type].cpu;
        remainMmB = remainMmB - vms[type].mm;
        this->remainSizeB = this->remainCpuB+this->remainMmB;
        this->remainSize = this->remainSize - vms[type].cpu- vms[type].mm;
        // if(remainCpuA < 0 || remainMmA < 0)
        //     cout << "addSingleB error!!!"<<endl;
        DeployedVM tmpVM(type, index, B, vmId,day);
        listDVMB->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMB->front();
        remainCMRatioB = ((float)remainCpuB / remainMmB);
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
        ++this->numOfVm;
    }
    
    void addBoth(string type, int index, int vmId,int day, unordered_map <int, DeployedVM*>& vmIdToDeployedVM, unordered_map<string, VmProperties>& vms){
        remainCpuA = remainCpuA - vms[type].cpu/2;
        remainMmA = remainMmA - vms[type].mm/2;
        remainCpuB = remainCpuB - vms[type].cpu/2;
        remainMmB = remainMmB - vms[type].mm/2;
        this->remainSizeA = this->remainCpuA+this->remainMmA;
        this->remainSizeB = this->remainCpuB+this->remainMmB;
        this->remainSize = this->remainSize - vms[type].cpu- vms[type].mm;
        // if(remainCpuA < 0 || remainMmA < 0 || remainCpuB < 0 || remainMmB < 0)
        //     cout << "addBoth error!!!"<<endl;
        DeployedVM tmpVM(type, index, Both, vmId,day);
        listDVMBoth->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMBoth->front();
        remainCMRatioA = ((float)remainCpuA / remainMmA);
        remainCMRatioB = ((float)remainCpuB / remainMmB);
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
        ++this->numOfVm;
    }
    
    void updateRemainResBeforeDelete(DeployedVM& vm, unordered_map<string, VmProperties>& vms){
            string type = vm.type;
            Node _node = vm.deployedNode;
            int cpuNum = vms[type].cpu;
            int mmNum = vms[type].mm;
            this->remainSize = this->remainSize+cpuNum + mmNum;
            if (_node == A)
            {
                remainCpuA += cpuNum;
                remainMmA += mmNum;
                remainCMRatioA = ((float)remainCpuA / remainMmA);
                remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
            }else if(_node == B){
                remainCpuB += cpuNum;
                remainMmB += mmNum;
                remainCMRatioB = ((float)remainCpuB / remainMmB);
                remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
            }else{
                remainCpuA += cpuNum/2;
                remainMmA += mmNum/2;
                remainCpuB += cpuNum/2;
                remainMmB += mmNum/2;
                remainCMRatioA = ((float)remainCpuA / remainMmA);
                remainCMRatioB = ((float)remainCpuB / remainMmB);
                remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
            }
        }

        void delvm(DeployedVM& deletedVM, unordered_map<string, VmProperties>& vms)
        {
        --this->numOfVm;
        updateRemainResBeforeDelete(deletedVM, vms);
        this->remainSizeA = this->remainCpuA+this->remainMmA;
        this->remainSizeB = this->remainCpuB+this->remainMmB;
        if(deletedVM.deployedNode == A)
        listDVMA->remove(deletedVM);//！！！ pay attention！！！
        else if(deletedVM.deployedNode == B)
        listDVMB->remove(deletedVM);
        else
        listDVMBoth->remove(deletedVM);
        }

        bool isRemainResourceAvailForA(int _cpu, int _mm)
        {
            if((_cpu <= remainCpuA) && (_mm <= remainMmA))
                return true;
            else
                return false;
        }

        bool isRemainResourceAvailForB(int _cpu, int _mm)
        {
            if((_cpu <= remainCpuB) && (_mm <= remainMmB))
                return true;
            else
                return false;
        }

        bool isRemainResourceAvailForBoth(int _cpu, int _mm)
        {
            if((_cpu / 2 <= remainCpuA) && (_mm / 2 <= remainMmA) && (_cpu / 2 <= remainCpuB) && (_mm / 2 <= remainMmB))
                return true;
            else
                return false;
        }

        void updateListsHostIndex(int afterHostId)
        {
            for(auto It = listDVMA->begin(); It!= listDVMA->end();It++){
                It->host = afterHostId;
            }
            for(auto It = listDVMB->begin(); It!= listDVMB->end();It++){
                It->host = afterHostId;
            }
            for(auto It = listDVMBoth->begin(); It!= listDVMBoth->end();It++){
                It->host = afterHostId;
            }
        }
};

// //已购买的主机
// vector<PurchasedHost> *purchasedHosts = new vector<PurchasedHost>;

//purchasedHost比较函数
bool static purchasedHostCmp(const PurchasedHost& first, const PurchasedHost& second)
{
    return first.hostId < second.hostId;
}

bool static vmResCmp(const RequestInfo& first, const RequestInfo& second)
{
    if(vms_backup[first.vmType].isDuet != vms_backup[second.vmType].isDuet)
        return vms_backup[first.vmType].isDuet > vms_backup[second.vmType].isDuet;
    else
        return vms_backup[first.vmType].cpu + vms_backup[first.vmType].mm > vms_backup[second.vmType].cpu + vms_backup[second.vmType].mm;
    // return first.vmId > second.vmId;
}



void migratePhysicalToA(int vmId, int toHostIndex, vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms,  unordered_map <int, DeployedVM*>& vmIdToDeployedVM){
    int formerHostId = vmIdToDeployedVM[vmId]->host;
    string vmType = vmIdToDeployedVM[vmId]->type;
    int day = vmIdToDeployedVM[vmId]->day;
    purchasedHost[formerHostId].delvm(*vmIdToDeployedVM[vmId], vms);
    purchasedHost[toHostIndex].addSingleA(vmType, toHostIndex, vmId, day, vmIdToDeployedVM, vms);
}

void migratePhysicalToB(int vmId,int toHostIndex, vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms,  unordered_map <int, DeployedVM*>& vmIdToDeployedVM){
    int formerHostId = vmIdToDeployedVM[vmId]->host;
    string vmType = vmIdToDeployedVM[vmId]->type;
    int day = vmIdToDeployedVM[vmId]->day;
    purchasedHost[formerHostId].delvm(*vmIdToDeployedVM[vmId], vms);
    // if(!purchasedHost[toHostIndex].isRemainResourceAvailForB(vms[vmType].cpu,vms[vmType].mm))
    // {
    //     cout << "迁移中B节点资源不够" << endl;
    // }
    purchasedHost[toHostIndex].addSingleB(vmType, toHostIndex, vmId, day, vmIdToDeployedVM, vms);

}

void migratePhysicalToBoth(int vmId,int toHostIndex, vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms,  unordered_map <int, DeployedVM*>& vmIdToDeployedVM){
    int formerHostId = vmIdToDeployedVM[vmId]->host;
    string vmType = vmIdToDeployedVM[vmId]->type;
    int day = vmIdToDeployedVM[vmId]->day;
    purchasedHost[formerHostId].delvm(*vmIdToDeployedVM[vmId], vms);
    // if(!purchasedHost[toHostIndex].isRemainResourceAvailForBoth(vms[vmType].cpu,vms[vmType].mm))
    // {
    //     cout << "迁移中双节点资源不够" << endl;
    // }
    purchasedHost[toHostIndex].addBoth(vmType, toHostIndex, vmId, day, vmIdToDeployedVM, vms);

}

//找到最小能放下的服务器下标(dailyCost越低越好)
int findBestFitHost(DeployedVM* p, vector<int>& disabledHost, int& upperLimit, vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms){
    int index = -1;
    int resSize = 2;
    float thiscmRatioDiff;
    list<pair<int,pair<int,float>>> res;
    int target = 100000;
    float cmRatioOfVm = (float)vms[p->type].cpu/vms[p->type].mm;    
    // cout<<"cout<<VMRation:"<<cmRatioOfVm<<endl;
    //  for (size_t i = 0; i < 2; i++)
    // {
    //     res.push_back(pair<int,pair<int,float>>(-1,pair<int,float>(i+100000,-1)));
    // }
    int up = 0;
    int length = purchasedHost.size();
    // cout<<"findfit: "<<p->deployedNode<<p->type<<endl;
    if (p->deployedNode == Both)
    {
        for (size_t i = 0; i < purchasedHost.size(); i++)
        {
            //remainSize
            int temp = purchasedHost[i].remainSize;
             if ( temp> up)
            {
                up = temp;
            }
            if (purchasedHost[i].isRemainResourceAvailForBoth(vms[p->type].cpu,vms[p->type].mm)&&temp<target&&disabledHost[i]==0)
            {
                thiscmRatioDiff = purchasedHost[i].remainCMRatioBoth > cmRatioOfVm ? purchasedHost[i].remainCMRatioBoth/cmRatioOfVm : cmRatioOfVm/purchasedHost[i].remainCMRatioBoth;
                // int costPerSource = (float)(purchasedHost[i].dailyCost)/(purchasedHost[i].cpu+purchasedHost[i].mm);
                if(res.size()>resSize)
                {
                    res.pop_back();
                }
                bool flag = false;
                for(auto it1 = res.begin();it1 != res.end();it1++){
                    if(it1->second.first > temp){
                        flag = true;
                        res.insert(it1,pair<int,pair<int,float>>(i,pair<int,float>(temp,thiscmRatioDiff)));
                        break;
                    }
                }
                if (flag == false)
                {
                    res.insert(res.end(),pair<int,pair<int,float>>(i,pair<int,float>(temp,thiscmRatioDiff)));
                }
                if(res.size()>=resSize){
                    target = res.back().second.first;
                }
            }
             if (rand()%2==0)
             {
                 i = i+ (length/1500)*(length/1500);
             }
        }
        
    }else{
        for (size_t i = 0; i < purchasedHost.size(); i++)
        {
            //remainSize
            int temp = purchasedHost[i].remainSize;
            if ( temp> up)
             {
                 up = temp;
             }
            if (purchasedHost[i].isRemainResourceAvailForA(vms[p->type].cpu,vms[p->type].mm)&&purchasedHost[i].remainSizeA<target&&disabledHost[i]==0)
            {
                // float costPerSource = (float)(purchasedHost[i].dailyCost)/(purchasedHost[i].cpu+purchasedHost[i].mm);
                if(res.size()>resSize)
                {
                    res.pop_back();
                }
                bool flag = false;
                thiscmRatioDiff = purchasedHost[i].remainCMRatioA > cmRatioOfVm ? purchasedHost[i].remainCMRatioA/cmRatioOfVm : cmRatioOfVm/purchasedHost[i].remainCMRatioA;
                for(auto it1 = res.begin();it1 != res.end();it1++){
                    if(it1->second.first > purchasedHost[i].remainSizeA){
                        flag = true;
                        res.insert(it1,pair<int,pair<int,float>>(i,pair<int,float>(purchasedHost[i].remainSizeA,thiscmRatioDiff)));
                        break;
                    }
                }
                if (flag == false)
                {
                    res.insert(res.end(),pair<int,pair<int,float>>(i,pair<int,float>(purchasedHost[i].remainSizeA,thiscmRatioDiff)));
                }
                if(res.size()>=resSize){
                target = res.back().second.first;
            }
                
            }
            if (purchasedHost[i].isRemainResourceAvailForB(vms[p->type].cpu,vms[p->type].mm)&&purchasedHost[i].remainSizeB<target&&disabledHost[i]==0)
            {
                
                thiscmRatioDiff = purchasedHost[i].remainCMRatioB > cmRatioOfVm ? purchasedHost[i].remainCMRatioB/cmRatioOfVm : cmRatioOfVm/purchasedHost[i].remainCMRatioB;
                // cout<<"Hostremain:"<<purchasedHost[i].remainCMRatioB<<endl;
                // cout<<"VmRatioOfVm:"<<cmRatioOfVm<<endl;
                // cout<<"Diff:          "<<thiscmRatioDiff<<endl;
                // float costPerSource = (float)(purchasedHost[i].dailyCost)/(purchasedHost[i].cpu+purchasedHost[i].mm);
                if(res.size()>resSize)
            {
                res.pop_back();
            }
                bool flag = false;
                for(auto it1 = res.begin();it1 != res.end();it1++){
                    if(it1->second.first > purchasedHost[i].remainSizeB){
                        flag = true;
                        res.insert(it1,pair<int,pair<int,float>>(i,pair<int,float>(purchasedHost[i].remainSizeB,thiscmRatioDiff)));
                        break;
                    }
                }
                if (flag == false)
                {
                    res.insert(res.end(),pair<int,pair<int,float>>(i,pair<int,float>(purchasedHost[i].remainSizeB,thiscmRatioDiff)));
                }
            if(res.size()>=resSize){
                target = res.back().second.first;
            }
            }
           if (rand()%2==0)
             {
                 i = i+ (length/1500)*(length/1500);
             }
            
        }
    }
    float cost = 10000;
    // cout<<"size: "<<res.size()<<endl;
    for (auto it = res.begin(); it != res.end(); it++)
    {
        // cout<<it->first<<" "<<it->second.first<<" "<<it->second.second<<endl;
        if (it->first!=-1 && it->second.second<cost)
        {
            // cout<<it->second.first<<" "<<it->second.second<<" "<<it->first<<endl;
            cost = it->second.second;
            index = it->first;
        }
    }
    // cout<<index<<endl;
    if (upperLimit > up )
    {
        upperLimit = up;
    }
    
    if (index == p->host)
    {
        return -1;
    }
    // cout<<"return index:"<<index<<endl;
   return index;

}

DeployedVM* findSmallerVmFromHost(int index, int& lastSize, const vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms){
    DeployedVM* p=nullptr;
    if (purchasedHost[index].numOfVm==0)
    {
        return p;
    }
    int size = -1;
        for (auto  it = purchasedHost[index].listDVMA->begin(); it !=purchasedHost[index].listDVMA->end(); it++)
    {
        if (vms[it->type].size > size&&vms[it->type].size<lastSize)
        {
            size = vms[it->type].size;
            p = &(*it);
        }
    }
    for (auto  it = purchasedHost[index].listDVMB->begin(); it !=purchasedHost[index].listDVMB->end(); it++)
    {
        if (vms[it->type].size > size&&vms[it->type].size<lastSize)
        {
            size = vms[it->type].size;
            p = &(*it);
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMBoth->begin(); it !=purchasedHost[index].listDVMBoth->end(); it++)
    {
        // 超参数 单双虚拟机比较大小
        if (vms[it->type].size > size&&vms[it->type].size<lastSize)
        {
            size = vms[it->type].size;
            p = &(*it);
        }
    }
    lastSize = size;
    return p;

}

// int findBiggestHost(const vector<PurchasedHost>& purchasedHost,vector<int> disabledHost){
//     int index = -1;
//     float price = -1;
//     for (auto it = purchasedHost.begin(); it != purchasedHost.end(); it++)
//     {
//         if((float)(it->dailyCost)/(it->cpu+it->mm)>price&& disabledHost[it-purchasedHost.begin()]==0){
//             index = it - purchasedHost.begin();
//             price = (float)(it->dailyCost)/(it->cpu+it->mm);
//         }
//     }
//     return index;
// }

// int findBiggestHost(const vector<PurchasedHost>& purchasedHost,vector<int> disabledHost){
//     //index size dailycost
//     list<pair<int,pair<int,float>>> res;
//     int account =0;
//     for (size_t i = 0; i < 5; i++)
//     {
//         res.push_back(pair<int,pair<int,float>>(-1,pair<int,float>(i+100000,-1)));
//     }
//     float costPerSource = 0;
//     for (auto it = purchasedHost.begin(); it != purchasedHost.end(); it++)
//     {
//         //remainSize
//         if(it->cpu+it->mm-it->remainSize < res.back().second.first && disabledHost[it-purchasedHost.begin()]==0){
//             costPerSource = (float)(it->dailyCost)/(it->cpu+it->mm);
//             res.pop_back();
//             bool flag = false;
//                for (auto it2 = res.begin(); it2 != res.end(); it2++)
//                     {
//                         if (it2->second.first > it->cpu+it->mm-it->remainSize)
//                         {
//                             flag = true;
//                             account++;
//                             res.insert(it2,pair<int,pair<int,float>>(it-purchasedHost.begin(),pair<int,float>(it->cpu+it->mm-it->remainSize,costPerSource)));
//                             break;
//                         }
//                     }
//             if (flag == false)
//             {
//                 account++;
//                 res.insert(res.end(),pair<int,pair<int,float>>(it-purchasedHost.begin(),pair<int,float>(it->cpu+it->mm-it->remainSize,costPerSource)));
//             }
//         }
//     }
//     int index = -1;
//     float cost = 0;
//     for (auto it = res.begin(); it != res.end(); it++)
//     {
//         cout<<it->second.first<<"  "<< it->first<<endl;
//         if (it->first!=-1 && it->second.second>cost)
//         {
//             cost = it->second.second;
//             index = it->first;
//         }
//     }
//     cout<<account<<endl;
//     cout<<"index"<<index<<endl;
//     return index;
// }



/////////////////找到剩余最大的中dailycost最值得
int findBiggestHost(const vector<PurchasedHost>& purchasedHost,vector<int> disabledHost){
    //index size dailycost
    list<pair<int,pair<int,float>>> res;
    int resSize = 5;
    int target = 0;
    float costPerSource = 0;
    for (auto it = purchasedHost.begin(); it != purchasedHost.end(); it++)
    {
        //remainSize
        if(it->remainSize>target && disabledHost[it-purchasedHost.begin()]==0){
            costPerSource = (float)(it->dailyCost)/(it->cpu+it->mm);
            if(res.size()>resSize)
            {
                res.pop_back();
            }
            bool flag = false;
               for (auto it2 = res.begin(); it2 != res.end(); it2++)
                    {
                        if (it2->second.first < it->remainSize)
                        {
                            flag = true;
                            res.insert(it2,pair<int,pair<int,float>>(it-purchasedHost.begin(),pair<int,float>(it->remainSize,costPerSource)));
                            break;
                        }
                    }
            if (flag == false)
            {
                res.insert(res.end(),pair<int,pair<int,float>>(it-purchasedHost.begin(),pair<int,float>(it->remainSize,costPerSource)));
            }
            if(res.size()>=resSize)
            {
                target = res.back().second.first;
            }
        }
    }
    int index = -1;
    float cost = 0;
    // cout<<"hostSize: "<<res.size()<<endl;
    for (auto it = res.begin(); it != res.end(); it++)
    {
        if (it->first!=-1 && it->second.second>cost)
        {
            cost = it->second.second;
            index = it->first;
        }
    }
    // cout<<"return: "<<index<<endl;
    return index;
}

int findRank(DeployedVM* p ,int index,vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms)
{
    int rank = 0;
    int size = vms[p->type].size;
      for (auto  it = purchasedHost[index].listDVMA->begin(); it !=purchasedHost[index].listDVMA->end(); it++)
    {
        if (vms[it->type].size > size)
        {
            ++rank;
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMB->begin(); it !=purchasedHost[index].listDVMB->end(); it++)
    {
        if (vms[it->type].size > size)
        {
            ++rank;
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMBoth->begin(); it !=purchasedHost[index].listDVMBoth->end(); it++)
    {
        // 超参数 单双虚拟机比较大小
        if (vms[it->type].size > size)
        {
            ++rank;
        }
    }
    return rank;

}

void migrantBeforeDeployed(int allowedCount, vector<PurchasedHost>& purchasedHost, unordered_map<string, VmProperties>& vms, list<pair<int,pair<int,Node>>>& migrateInfos, unordered_map <int, DeployedVM*>& vmIdToDeployedVM){

    if (purchasedHost.size() == 0)
    {
        // cout<<"return because number of host is 0!"<<endl;
        return;
    }
       //1被禁用
    vector<int> disabledHost(purchasedHost.size(),0);
    while (true)
    {
        int xxx = migrateInfos.size();
        if (xxx>=allowedCount)
        {
        // cout<<"return because break Count"<<endl;
            break;
        }
        int index = findBiggestHost(purchasedHost,disabledHost);
         if (index == -1)
        {
        // cout<<"return because can not find Biggest Host!"<<endl;
            break;
        }
        int lastSize = 100;
        for (size_t i = 0; i < purchasedHost.size(); i++)
        {
            if (disabledHost[i]==1)
            {
                continue;
            }
            int upperLimit = purchasedHost[i].remainSize;
            if (upperLimit>lastSize)
            {
                lastSize = upperLimit;
            }
        }
        lastSize = (lastSize*5)/7;
        // cout<<"num: "<<purchasedHost[index].listDVMA->size()+purchasedHost[index].listDVMB->size()+purchasedHost[index].listDVMBoth->size()<<endl;
        //返回-1说明已经找不到Host了
        while (migrateInfos.size()<allowedCount)
        {
            DeployedVM* p = findSmallerVmFromHost(index, lastSize, purchasedHost, vms);
            if (p == nullptr)
            {
                // cout<<"warning host is empty!"<<endl;
                disabledHost[index] = 1;
                break;
            }
            // cout<<lastSize;
            int upperLimit = 1000000;

            int targetIndex = findBestFitHost(p, disabledHost, upperLimit, purchasedHost, vms);
            if (lastSize > upperLimit)
            {
                lastSize = (upperLimit*5)/7;
            }
           
            
            //如果找不到就找下一个
            // if ()
            // {
            //     continue;
            // }
             if(targetIndex == -1||disabledHost[targetIndex] == 1){

                if(lastSize%2==0){
                    int rank = findRank(p,index,purchasedHost,vms);
	                if(rank*1.3>purchasedHost[index].numOfVm){
                        allowedCount -= allowedCount/20;
		                allowedCount -= (int)(rank*20/purchasedHost[index].numOfVm);
	                }
                }
                continue;
            }
            // if(disabledHost[targetIndex] == 1){

            //     if(lastSize%2==0){
            //         int rank = findRank(p,index,purchasedHost,vms);
            //         allowedCount -= (int)(rank*1.4/purchasedHost[index].numOfVm);
            //     }
            //     continue;
            // }
            // cout<<"  "<<index<<" moveto! "<<targetIndex<<endl;
            Node targetNode= Both;
            if (p->deployedNode == A || p->deployedNode == B)
            {
                if (purchasedHost[targetIndex].isRemainResourceAvailForA(vms[p->type].cpu,vms[p->type].mm)&&
                purchasedHost[targetIndex].isRemainResourceAvailForB(vms[p->type].cpu,vms[p->type].mm))
                {
                    // 超参数
                    if (purchasedHost[targetIndex].remainSizeA<
                    purchasedHost[targetIndex].remainSizeB)
                    {
                        /* code */
                        targetNode = A;
                    }else{
                        targetNode = B;
                    }
                }else if (purchasedHost[targetIndex].isRemainResourceAvailForA(vms[p->type].cpu,vms[p->type].mm))
                {
                    targetNode = A;
                }else{
                    targetNode = B;
                }
                
            }
            // cout<<"migrate add one!!!!!!!!!!!"<<endl;
            migrateInfos.push_back(pair<int,pair<int,Node>>(p->vmId,pair<int,Node>(targetIndex,targetNode)));
            if (targetNode == A)
            {
                migratePhysicalToA(p->vmId, targetIndex, purchasedHost, vms, vmIdToDeployedVM);
            }else if (targetNode == B)
            {
                migratePhysicalToB(p->vmId, targetIndex, purchasedHost, vms, vmIdToDeployedVM);
            }else{
                migratePhysicalToBoth(p->vmId,targetIndex, purchasedHost, vms, vmIdToDeployedVM);
            }
        }
        ///////////
      
    }
    
}

#endif /* DataStructure_h */
