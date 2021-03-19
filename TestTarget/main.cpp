//
//  main.cpp
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/12.
//

#pragma mark Header
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

using namespace std;

#pragma mark DATA_STRUCTURE
using namespace std;

//结点枚举类型
typedef enum : short {
    A,
    B,
    Both
} Node;

//请求信息枚举类型
typedef enum : short {
    add,
    del
} InfoType;

//主机信息
struct Host {
    string hostType;
    int cpu;
    int mm;
    float cmRatio;
    int hostCost;
    int dailyCost;
};

vector<Host> hosts;

//虚拟机信息
struct VmProperties {
    int cpu;
    int mm;
    bool isDuet;  // true为双节点部署
};

//虚拟机类型到信息的映射
unordered_map <string, VmProperties> vms;

//请求信息
struct RequestInfo {
    InfoType infoType;
    string vmType;
    int vmId;
};
vector<RequestInfo> requestInfos;

#pragma mark DeployedVM

class DeployedVM {
    
public:
    string type;
    int date;
    int host;
    Node deployedNode;
    int vmId;
    
    DeployedVM(const DeployedVM& deployedVM){
        this->type = deployedVM.type;
        this->date = deployedVM.date;
        this->host = deployedVM.host;
        this->deployedNode = deployedVM.deployedNode;
        this->vmId = deployedVM.vmId;
    }

    DeployedVM(string type, int date, int host, Node deployedNode,int vmId){
        this->type = type;
        this->date = date;
        this->host = host;
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
//        for(auto It = purchasedHost.listDVMA->begin(); It!= purchasedHost.listDVMA->end();It++){
//            this->listDVMA->push_back(*It);
//        }
//        this->listDVMB = new list<DeployedVM>();
//        for(auto It = purchasedHost.listDVMB->begin(); It!= purchasedHost.listDVMB->end();It++){
//            this->listDVMB->push_back(*It);
//        }
//         this->listDVMBoth = new list<DeployedVM>();
//        for(auto It = purchasedHost.listDVMBoth->begin(); It!= purchasedHost.listDVMBoth->end();It++){
//            this->listDVMBoth->push_back(*It);
//        }
    }

    PurchasedHost()
    {
        cpu = 0;
        mm = 0;
        listDVMA = new list<DeployedVM>;
        listDVMB = new list<DeployedVM>;
        listDVMBoth = new list<DeployedVM>;
    }
    
    PurchasedHost(int _hostId, int cpu, int mm){
        hostId = _hostId;
        remainCpuA = cpu/2;
        remainCpuB = cpu/2;
        remainMmA = mm/2;
        remainMmB = mm/2;
        remainCMRatioBoth = remainCMRatioA = remainCMRatioB = (float)cpu / mm;
        listDVMA = new list<DeployedVM>();
        listDVMB = new list<DeployedVM>();
        listDVMBoth = new list<DeployedVM>();
    }
    
    void addSingleA(string type, int index, int date, int vmId){
        remainCpuA = remainCpuA - vms[type].cpu;
        remainMmA = remainMmA - vms[type].mm;
        DeployedVM tmpVM(type, date, index, A, vmId);
        listDVMA->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMA->front();
        remainCMRatioA = (float)remainCpuA / remainMmA;
    }
    
    void addSingleB(string type, int index, int date, int vmId){
        remainCpuB = remainCpuB - vms[type].cpu;
        remainMmB = remainMmB - vms[type].mm;
        DeployedVM tmpVM(type, date, index, B, vmId);
        listDVMB->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMB->front();
        remainCMRatioB = (float)remainCpuB / remainMmB;
    }
    
    void addBoth(string type, int index, int date, int vmId){
        remainCpuA = remainCpuA - vms[type].cpu/2;
        remainMmA = remainMmA - vms[type].mm/2;
        remainCpuB = remainCpuB - vms[type].cpu/2;
        remainMmB = remainMmB - vms[type].mm/2;
        DeployedVM tmpVM(type, date, index, Both, vmId);
        listDVMBoth->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMBoth->front();
        remainCMRatioBoth = (float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB);
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

//已购买的主机
vector<PurchasedHost> *purchasedHosts = new vector<PurchasedHost>;

//purchasedHost比较函数
bool static purchasedHostCmp(const PurchasedHost& first, const PurchasedHost& second)
{
    return first.hostId < second.hostId;
}

#pragma mark INPUT
/*
 
 解析输入过程定义
 
 */

int string2Int(string s){
    int _tmp = 0;
    for(int i = 0; i < s.size() - 1; i++){
        _tmp = 10 * _tmp + s[i] - '0';
    }
    return _tmp;
}

//解析主机信息
void generateHost(string &hostType,string &cpu,string &mm,string &hostCost,string &dailyCost){
    string _hostType="";
    for(int i = 1; i < hostType.size() - 1; i++){
        _hostType += hostType[i];
    }
    Host _host;
    _host.hostType = _hostType;
    _host.cpu = string2Int(cpu);
    _host.mm = string2Int(mm);
    _host.cmRatio = (float)_host.cpu / _host.mm;
    _host.hostCost = string2Int(hostCost);
    _host.dailyCost = string2Int(dailyCost);
    hosts.push_back(_host);
    
}

//解析虚拟机信息
void generateVm(string &vmType,string &cpu,string &mm,string &isDuet){
    string _vmType;
    VmProperties _vmProperties;
    
    _vmProperties.cpu = string2Int(cpu);
    _vmProperties.mm = string2Int(mm);
    for(int i = 1; i < vmType.size() - 1; i++){
        _vmType += vmType[i];
    }
    if(isDuet[0] == '1')
        _vmProperties.isDuet = true;
    else
        _vmProperties.isDuet = false;
    
    vms.insert(pair<string,VmProperties>(_vmType, _vmProperties));
}

//解析用户添加请求
void generateRequest(string &vmType,string &vmId){
    RequestInfo _requestInfo;
    vmType.substr(0,vmType.size() - 1);
    _requestInfo.infoType = add;
    _requestInfo.vmType = vmType.substr(0,vmType.size() - 1);
    _requestInfo.vmId = string2Int(vmId);
    requestInfos.push_back(_requestInfo);
}

//解析用户删除请求
void generateRequest(string &vmId){
    RequestInfo _requestInfo;
    _requestInfo.infoType = del;
    _requestInfo.vmId = string2Int(vmId);
    requestInfos.push_back(_requestInfo);
}

#pragma mark OUTPUT
//使用全局变量
//vector<RequestInfo>& requestInfos
//unordered_map <int, DeployedVM*> vmIdToDeployedVM;
void outputDeploymentResult(){
    for(auto it = requestInfos.begin(); it != requestInfos.end(); it++){
        if (it->infoType == add)
        {
            if(vmIdToDeployedVM[it->vmId]->deployedNode == A){
                printf("(%d,A)\n",vmIdToDeployedVM[it->vmId]->host);

            }else if (vmIdToDeployedVM[it->vmId]->deployedNode == B)
            {
                printf("(%d,B)\n",vmIdToDeployedVM[it->vmId]->host);
            }else{
                printf("(%d)\n",vmIdToDeployedVM[it->vmId]->host);
            }
        }
    }
}

int main(){
    list<string> buyMsg;
    map<int, int> todayPurchasePlan;
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

                // 尝试在已有的主机上部署
                for(int j = 0; j < purchasedHosts->size(); ++j)
                {
                    if (j>300&&j<purchasedHosts->size()*0.9) {
                        j = j + rand()%2;
                    }
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
                    // TODO
                    // purchase new host and deploy vm on that
                    float mincmRatioDiff = 10000.0;
                    int purchasedBestHostIndex = -1;
                    string purchasedBestHostType;
                    for(int j = 0; j < hosts.size(); ++j)
                    {
                        float thiscmRatioDiff = fabs(hosts[j].cmRatio / (float)cmRatioOfVm - 1);
                        if(!isDuetOfVm)
                        {
                                if(thiscmRatioDiff < mincmRatioDiff&&hosts[j].cpu>=vms[requestInfos[i].vmType].cpu*2&&hosts[j].mm>=vms[requestInfos[i].vmType].mm*2)
                            {
                                mincmRatioDiff = thiscmRatioDiff;
                                purchasedBestHostIndex = j;
                            }
                        }else{
                            if(thiscmRatioDiff < mincmRatioDiff&&hosts[j].cpu>=vms[requestInfos[i].vmType].cpu&&hosts[j].mm>=vms[requestInfos[i].vmType].mm)
                            {
                                mincmRatioDiff = thiscmRatioDiff;
                                purchasedBestHostIndex = j;
                            }
                        }
                    }
                    bestHost = new PurchasedHost(purchasedBestHostIndex, hosts[purchasedBestHostIndex].cpu, hosts[purchasedBestHostIndex].mm);
                    purchasedHosts->push_back(*bestHost);

                    bestHost = &purchasedHosts->back();
                    bestHostIndex = purchasedHosts->size() - 1;
                    //buyMsg.push_back("("+hosts[purchasedBestHostIndex].hostType+",1)\n");

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
        
    return 0;
}

