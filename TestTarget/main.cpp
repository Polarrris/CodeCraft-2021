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

//#include "Input.h"
//#include "Output.h"
//#include "DataStructure.h"
//#include "HostPlanning.h"
//#include "VMMigration.h"
//#include "VMDeployment.h"
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
        this->listDVMA = new list<DeployedVM>();
        for(auto It = purchasedHost.listDVMA->begin(); It!= purchasedHost.listDVMA->end();It++){
            this->listDVMA->push_back(*It);
        }
        this->listDVMB = new list<DeployedVM>();
        for(auto It = purchasedHost.listDVMB->begin(); It!= purchasedHost.listDVMB->end();It++){
            this->listDVMB->push_back(*It);
        }
         this->listDVMBoth = new list<DeployedVM>();
        for(auto It = purchasedHost.listDVMBoth->begin(); It!= purchasedHost.listDVMBoth->end();It++){
            this->listDVMBoth->push_back(*It);
        }
    }

    PurchasedHost()
    {
        cpu = 0;
        mm = 0;
        listDVMA = new list<DeployedVM>;
        listDVMB = new list<DeployedVM>;
        listDVMBoth = new list<DeployedVM>;
    }
    
    PurchasedHost(int cpu, int mm){
        remainCpuA = cpu/2;
        remainCpuB = cpu/2;
        remainMmA = mm/2;
        remainMmB = mm/2;
        remainCMRatioBoth = remainCMRatioA = remainCMRatioB = (float)cpu / mm;
        listDVMA = new list<DeployedVM>();
        listDVMB = new list<DeployedVM>();
        listDVMBoth = new list<DeployedVM>();
    }
    
    DeployedVM* addSingleA(string type, int index, int date, int vmId){
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
        remainCMRatioA = (float)remainCpuA / remainMmA;
        return &listDVMA->front();
    }
    
    DeployedVM* addSingleB(string type, int index, int date, int vmId){
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
        remainCMRatioB = (float)remainCpuB / remainMmB;
        return &listDVMB->front();
    }
    
    DeployedVM* addBoth(string type, int index, int date, int vmId){
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
        remainCMRatioBoth = (float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB);
        return &listDVMBoth->front();
    }
    
    void updateRemainResAfterDelete(DeployedVM& vm){
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
            // cout<<"delete "<<deletedVM.host<<endl;
            // e.g.
            updateRemainResAfterDelete(deletedVM);
            listDVMA->remove(deletedVM);     //！！！ pay attention！！！
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
    
    //    int updateRemainResource(node wantedNode){
    //        int tmp1=0,tmp2=0;
    //        if (wantedNode == A) {
    //            list <DeployedVM>::iterator p_listDVMA;
    //            for (p_listDVMA=listDVMA.begin(); p_listDVMA!=listDVMA.end(); ++p_listDVMA) {
    //                tmp1 += vms[p_listDVMA->type].cpu;
    //                tmp2 += vms[p_listDVMA->type].mm;
    //            }
    //
    //            if (!listDVMBoth.empty()) {
    //                list <DeployedVM>::iterator p_listDVMBoth;
    //                for (p_listDVMBoth=listDVMBoth.begin(); p_listDVMBoth!=listDVMBoth.end(); ++p_listDVMBoth) {
    //                    tmp1 += vms[p_listDVMBoth->type].cpu;
    //                    tmp2 += vms[p_listDVMBoth->type].mm;
    //                }
    //            }
    //
    //            remainCpuA = (cpu/2)-tmp1;
    //            remainMmA = (mm/2)-tmp2;
    //        }
    //        if (wantedNode == B) {
    //            list <DeployedVM>::iterator p_listDVMB;
    //            for (p_listDVMB=listDVMA.begin(); p_listDVMB!=listDVMA.end(); ++p_listDVMB) {
    //                tmp1 += vms[p_listDVMB->type].cpu;
    //                tmp2 += vms[p_listDVMB->type].mm;
    //            }
    //
    //            if (!listDVMBoth.empty()) {
    //                list <DeployedVM>::iterator p_listDVMBoth;
    //                for (p_listDVMBoth=listDVMBoth.begin(); p_listDVMBoth!=listDVMBoth.end(); ++p_listDVMBoth) {
    //                    tmp1 += vms[p_listDVMBoth->type].cpu;
    //                    tmp2 += vms[p_listDVMBoth->type].mm;
    //                }
    //            }
    //
    //            remainCpuA = (cpu/2)-tmp1;
    //            remainMmA = (mm/2)-tmp2;
    //        }
    //
    //
    //
    //        return 0;
    //    }
    
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
            // cout<<"vmID: "<<it->vmId<<"HostId: "<<vmIdToDeployedVM[it->vmId]->host<<endl;
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
    clock_t start, finish;
    start = clock();
//TEST
    const string filePath = "/Users/jesse/Desktop/CodeCraft/training-data/training-1.txt";
    //输入
        freopen(filePath.c_str(),"rb",stdin);
//    if(true){
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
        
//    }
    // fclose(stdin);
    finish = clock();
//    printf("\nuse time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
    return 0;
}

