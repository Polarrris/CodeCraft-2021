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
        //  cout<<"new a VM Id:  "<<vmId<<" on Host:"<<host<<endl;
    }

    DeployedVM(string type, int date, int host, Node deployedNode,int vmId){
        this->type = type;
        this->date = date;
        this->host = host;
        this->deployedNode = deployedNode;
        this->vmId = vmId;
        //  cout<<"new a VM Id:  "<<vmId<<" on Host:"<<host<<endl;
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
        remainCMRatioBoth = remainCMRatioA = remainCMRatioB = ((float)cpu / mm);
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
        //  cout<<vmId<<" host:"<<vmIdToDeployedVM[vmId]->host<<"Node "<<vmIdToDeployedVM[vmId]->deployedNode;
        //  cout<<"show!\n";
        //  for(auto It = vmIdToDeployedVM.begin();It!=vmIdToDeployedVM.end();It++){
        //         cout<<It->first<<"  hostId:"<<It->second->host<<"  node:"<<It->second->deployedNode<<endl;
        //     }
        remainCMRatioA = ((float)remainCpuA / remainMmA);
//        return &listDVMA->front();
    }
    
    void addSingleB(string type, int index, int date, int vmId){
        remainCpuB = remainCpuB - vms[type].cpu;
        remainMmB = remainMmB - vms[type].mm;
        DeployedVM tmpVM(type, date, index, B, vmId);
        listDVMB->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMB->front();
        //  cout<<vmId<<" host:"<<vmIdToDeployedVM[vmId]->host<<"Node "<<vmIdToDeployedVM[vmId]->deployedNode;
        //  cout<<"show!\n";
        //  for(auto It = vmIdToDeployedVM.begin();It!=vmIdToDeployedVM.end();It++){
        //         cout<<It->first<<"  hostId:"<<It->second->host<<"  node:"<<It->second->deployedNode<<endl;
        //     }
        remainCMRatioB = ((float)remainCpuB / remainMmB);
//        return &listDVMB->front();
    }
    
    void addBoth(string type, int index, int date, int vmId){
        remainCpuA = remainCpuA - vms[type].cpu/2;
        remainMmA = remainMmA - vms[type].mm/2;
        remainCpuB = remainCpuB - vms[type].cpu/2;
        remainMmB = remainMmB - vms[type].mm/2;
        DeployedVM tmpVM(type, date, index, Both, vmId);
        listDVMBoth->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMBoth->front();
        // cout<<vmId<<" host:"<<vmIdToDeployedVM[vmId]->host<<"Node "<<vmIdToDeployedVM[vmId]->deployedNode;
        // cout<<"show!\n";
        //  for(auto It = vmIdToDeployedVM.begin();It!=vmIdToDeployedVM.end();It++){
        //         cout<<It->first<<"  hostId:"<<It->second->host<<"  node:"<<It->second->deployedNode<<endl;
        //     }
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
//        return &listDVMBoth->front();
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
                remainCMRatioA = ((float)remainCpuA / remainMmA);
            }else if(_node == B){
                remainCpuB += cpuNum;
                remainMmB += mmNum;
                remainCMRatioB = ((float)remainCpuB / remainMmB);
            }else{
                remainCpuA += cpuNum/2;
                remainMmA += mmNum/2;
                remainCpuB += cpuNum/2;
                remainMmB += mmNum/2;
                remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
            }
        }

        void delvm(DeployedVM& deletedVM)
        {
        //  for(auto It = listDVMA->begin(); It!= listDVMA->end();It++){
        //     cout<<It->vmId<<" ";
        // }
        // cout<<endl;
        // for(auto It = listDVMB->begin(); It!= listDVMB->end();It++){
        //     cout<<It->vmId<<" ";

        // }
        // cout<<endl;
        // for(auto It = listDVMBoth->begin(); It!= listDVMBoth->end();It++){
        //     cout<<It->vmId<<" ";

        // }
        // cout<<endl;
        // cout<<"delete "<<deletedVM.host<<endl;
        // e.g.
        updateRemainResBeforeDelete(deletedVM);
        // cout<<"before remove"<<endl<<this->listDVMA->size()<<endl<<this->listDVMB->size()<<endl<<this->listDVMBoth->size()<<endl;;
        if(deletedVM.deployedNode == A)
        listDVMA->remove(deletedVM);//！！！ pay attention！！！
        else if(deletedVM.deployedNode == B)
        listDVMB->remove(deletedVM);
        else
        listDVMBoth->remove(deletedVM);
        // cout<<"After remove"<<endl<<this->listDVMA->size()<<endl<<this->listDVMB->size()<<endl<<this->listDVMBoth->size()<<endl;

        //TODO
        // update remain resource
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

    
    ~PurchasedHost(){
        // cout<<"~~function"<<endl;
        // cout<<this->listDVMA->size()<<endl;
        // delete listDVMA;
        // delete listDVMB;
        // delete listDVMBoth;

        // cout<<"delHost\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        // listDVMA->clear();
        // listDVMBoth->clear();
        // listDVMB->clear();
        // cout<<this->listDVMA->size()<<endl;

    }
};

//已购买的主机
vector<PurchasedHost> *purchasedHosts = new vector<PurchasedHost>;

//purchasedHost比较函数
bool static purchasedHostCmp(const PurchasedHost& first, const PurchasedHost& second)
{
    return first.hostId < second.hostId;
}

bool static vmResCmp(const RequestInfo& first, const RequestInfo& second)
{
    return vms[first.vmType].cpu + vms[first.vmType].mm > vms[second.vmType].cpu + vms[second.vmType].mm;
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
#ifndef DEMO
    //VmProperties vmp = vms[vmType.substr(0,vmType.size() - 1)];
    
        
#endif
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

float getCostPerFormance(int index,int day,int totalday){
    return (hosts[index].cpu+hosts[index].mm)/float(hosts[index].hostCost+hosts[index].dailyCost*(totalday-day));
}

int getBestHost(const vector<RequestInfo>& requestInfos,int i,int day,int totalDay){
    bool isDuetOfVm = vms[requestInfos[i].vmType].isDuet;
    
    float cmRatioOfVm = ((double)vms[requestInfos[i].vmType].cpu / vms[requestInfos[i].vmType].mm);
    // cm比差距 index 性价比
    vector<pair<float,pair<int,float>>> hostVec;
    hostVec.push_back(pair<float,pair<int,float>>(10000,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10001,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10002,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10003,pair<int,float>(-1,0)));
    hostVec.push_back(pair<float,pair<int,float>>(10004,pair<int,float>(-1,0)));
    for(int j = 0; j < hosts.size(); ++j)
        {
            // float thiscmRatioDiff = fabs(hosts[j].cmRatio - (float)cmRatioOfVm);
            float thiscmRatioDiff = hosts[j].cmRatio > cmRatioOfVm ? hosts[j].cmRatio/cmRatioOfVm : cmRatioOfVm/hosts[j].cmRatio;
            // if(thiscmRatioDiff< 0 )
            // {
            //     cout<<"error"<<endl;
            //     cout<<hosts[j].cmRatio <<" "<< cmRatioOfVm<<endl;
            // }
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


#pragma mark MAIN
int main(){
    map<int, int> todayPurchasePlan;
    int count = 0;
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
                    float cmRatioOfVm = ((float)vms[requestInfos_backup[i].vmType].cpu / vms[requestInfos_backup[i].vmType].mm);
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
                                // float thiscmRatioDiff = fabs(purchasedHosts->at(j).remainCMRatioBoth - (double)cmRatioOfVm );
                                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioBoth > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioBoth/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioBoth;
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
                                
                                // float thiscmRatioDiff = fabs(purchasedHosts->at(j).remainCMRatioA - (double)cmRatioOfVm );
                                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioA > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioA/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioA;
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
                                // float thiscmRatioDiff = fabs(purchasedHosts->at(j).remainCMRatioB - (double)cmRatioOfVm );
                                float thiscmRatioDiff = purchasedHosts->at(j).remainCMRatioB > cmRatioOfVm ? purchasedHosts->at(j).remainCMRatioB/cmRatioOfVm : cmRatioOfVm/purchasedHosts->at(j).remainCMRatioB;
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
        
    return 0;
}

