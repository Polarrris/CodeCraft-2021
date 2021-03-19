//
//  DataStructure.h
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/15.
//

#ifndef DataStructure_h
#define DataStructure_h

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
        //  cout<<vmId<<" host:"<<vmIdToDeployedVM[vmId]->host<<"Node "<<vmIdToDeployedVM[vmId]->deployedNode;
        //  cout<<"show!\n";
        //  for(auto It = vmIdToDeployedVM.begin();It!=vmIdToDeployedVM.end();It++){
        //         cout<<It->first<<"  hostId:"<<It->second->host<<"  node:"<<It->second->deployedNode<<endl;
        //     }
        remainCMRatioA = (float)remainCpuA / remainMmA;
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
        remainCMRatioB = (float)remainCpuB / remainMmB;
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
        remainCMRatioBoth = (float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB);
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

#endif /* DataStructure_h */
