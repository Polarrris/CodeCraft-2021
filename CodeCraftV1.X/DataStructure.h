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

    }
    
    PurchasedHost(int _hostId, int cpu, int mm){
        hostId = _hostId;
        this->cpu = cpu;
        this->mm = mm;
        remainCpuA = cpu/2;
        remainCpuB = cpu/2;
        remainMmA = mm/2;
        remainMmB = mm/2;
        remainCMRatioBoth = remainCMRatioA = remainCMRatioB = ((float)cpu / mm);
        listDVMA = new list<DeployedVM>();
        listDVMB = new list<DeployedVM>();
        listDVMBoth = new list<DeployedVM>();
    }
    
    void addSingleA(string type, int index, int vmId,int day){
        remainCpuA = remainCpuA - vms[type].cpu;
        remainMmA = remainMmA - vms[type].mm;
        // if(remainCpuA < 0 || remainMmA < 0)
        //     cout << "addSingleA error!!!"<<endl;
        DeployedVM tmpVM(type, index, A, vmId,day);
        listDVMA->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMA->front();
        remainCMRatioA = ((float)remainCpuA / remainMmA);
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
    }
    
    void addSingleB(string type, int index, int vmId,int day){
        remainCpuB = remainCpuB - vms[type].cpu;
        remainMmB = remainMmB - vms[type].mm;
        // if(remainCpuA < 0 || remainMmA < 0)
        //     cout << "addSingleB error!!!"<<endl;
        DeployedVM tmpVM(type, index, B, vmId,day);
        listDVMB->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMB->front();
        remainCMRatioB = ((float)remainCpuB / remainMmB);
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
    }
    
    void addBoth(string type, int index, int vmId,int day){
        remainCpuA = remainCpuA - vms[type].cpu/2;
        remainMmA = remainMmA - vms[type].mm/2;
        remainCpuB = remainCpuB - vms[type].cpu/2;
        remainMmB = remainMmB - vms[type].mm/2;
        // if(remainCpuA < 0 || remainMmA < 0 || remainCpuB < 0 || remainMmB < 0)
        //     cout << "addBoth error!!!"<<endl;
        DeployedVM tmpVM(type, index, Both, vmId,day);
        listDVMBoth->push_front(tmpVM);
        vmIdToDeployedVM[vmId] = &listDVMBoth->front();
        remainCMRatioA = ((float)remainCpuA / remainMmA);
        remainCMRatioB = ((float)remainCpuB / remainMmB);
        remainCMRatioBoth = ((float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB));
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

bool static vmResCmp(const RequestInfo& first, const RequestInfo& second)
{
    if(vms[first.vmType].isDuet != vms[second.vmType].isDuet)
        return vms[first.vmType].isDuet > vms[second.vmType].isDuet;
    else
        return vms[first.vmType].cpu + vms[first.vmType].mm > vms[second.vmType].cpu + vms[second.vmType].mm;
}



void migratePhysicalToA(vector<PurchasedHost>& purchasedHost,int vmId,int toHostIndex){
    int formerHostId = vmIdToDeployedVM[vmId]->host;
    string vmType = vmIdToDeployedVM[vmId]->type;
    int day = vmIdToDeployedVM[vmId]->day;
    purchasedHost[formerHostId].delvm(*vmIdToDeployedVM[vmId]);
    // if(!purchasedHost[toHostIndex].isRemainResourceAvailForA(vms[vmType].cpu,vms[vmType].mm))
    // {
    //     cout << "迁移中A节点资源不够" << endl; 
    // }
    purchasedHost[toHostIndex].addSingleA(vmType,toHostIndex,vmId,day);
}

void migratePhysicalToB(vector<PurchasedHost>& purchasedHost,int vmId,int toHostIndex){
    int formerHostId = vmIdToDeployedVM[vmId]->host;
    string vmType = vmIdToDeployedVM[vmId]->type;
    int day = vmIdToDeployedVM[vmId]->day;
    purchasedHost[formerHostId].delvm(*vmIdToDeployedVM[vmId]);
    // if(!purchasedHost[toHostIndex].isRemainResourceAvailForB(vms[vmType].cpu,vms[vmType].mm))
    // {
    //     cout << "迁移中B节点资源不够" << endl; 
    // }
    purchasedHost[toHostIndex].addSingleB(vmType,toHostIndex,vmId,day);

}

void migratePhysicalToBoth(vector<PurchasedHost>& purchasedHost,int vmId,int toHostIndex){
    int formerHostId = vmIdToDeployedVM[vmId]->host;
    string vmType = vmIdToDeployedVM[vmId]->type;
    int day = vmIdToDeployedVM[vmId]->day;
    purchasedHost[formerHostId].delvm(*vmIdToDeployedVM[vmId]);
    // if(!purchasedHost[toHostIndex].isRemainResourceAvailForBoth(vms[vmType].cpu,vms[vmType].mm))
    // {
    //     cout << "迁移中双节点资源不够" << endl; 
    // }
    purchasedHost[toHostIndex].addBoth(vmType,toHostIndex,vmId,day);

}

//找到最小能放下的服务器下标
int findBestFitHost(DeployedVM* p,vector<PurchasedHost>& purchasedHost,vector<int>& disabledHost,int& upperLimit){
    int index = -1;
    //score 评分越高越好，现在的score为剩下的空间的倒数
    // int score = 0;
    int size = 100000;
    int up = 0;

    int length = purchasedHost.size();
    if (p->deployedNode == Both)
    {
        for (size_t i = 0; i < purchasedHost.size(); i++)
        {
            int temp =purchasedHost[i].remainCpuA+purchasedHost[i].remainMmA+
                    purchasedHost[i].remainCpuB+purchasedHost[i].remainMmB;
            ///////////////////
             if ( temp> up)
             {
                 up = temp;
             }
            if (purchasedHost[i].isRemainResourceAvailForBoth(vms[p->type].cpu,vms[p->type].mm)&&
           temp<size)
            {
                size = temp;
                index = i;
            }

             
             if (rand()%2==0)
             {
                 i = i+ (length/1800)*(length/1800);
             }
             
            
            
            
            
        }
        
    }else{
        for (size_t i = 0; i < purchasedHost.size(); i++)
        {
            int temp = purchasedHost[i].remainCpuA+purchasedHost[i].remainCpuB+purchasedHost[i].remainMmA+purchasedHost[i].remainMmB;
            if ( temp> up)
             {
                 up = temp;
             }
            if ((purchasedHost[i].isRemainResourceAvailForA(vms[p->type].cpu,vms[p->type].mm)||
            purchasedHost[i].isRemainResourceAvailForB(vms[p->type].cpu,vms[p->type].mm))&&temp<size)
            {
                size = temp;
                index = i;
            }
           if (rand()%2==0)
             {
                 i = i+ (length/1800)*(length/1800);
             }
            
        }
    }

    if (upperLimit > up )
    {
        upperLimit = up;
    }
    
    if (index == p->host)
    {
        return -1;
    }
    
   return index;

}

DeployedVM* findSmallerVmFromHost(const vector<PurchasedHost>& purchasedHost,int index,int& lastSize){
    DeployedVM* p=nullptr;
    if (purchasedHost[index].listDVMA->size() == 0 && purchasedHost[index].listDVMB->size() == 0 
    && purchasedHost[index].listDVMBoth->size() == 0)
    {
        
        return p;
    }
    int size = -1;
     for (auto  it = purchasedHost[index].listDVMA->begin(); it !=purchasedHost[index].listDVMA->end(); it++)
    {
        if (vms[it->type].cpu+vms[it->type].mm > size&&vms[it->type].cpu+vms[it->type].mm<lastSize)
        {
            size = vms[it->type].cpu+vms[it->type].mm;
            p = &(*it);
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMB->begin(); it !=purchasedHost[index].listDVMB->end(); it++)
    {
        if (vms[it->type].cpu+vms[it->type].mm > size&&vms[it->type].cpu+vms[it->type].mm<lastSize)
        {
            size = vms[it->type].cpu+vms[it->type].mm;
            p = &(*it);
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMBoth->begin(); it !=purchasedHost[index].listDVMBoth->end(); it++)
    {
        // 超参数 单双虚拟机比较大小
        if (vms[it->type].cpu/2+vms[it->type].mm/2 > size&&vms[it->type].cpu/2+vms[it->type].mm/2<lastSize)
        {
            size = vms[it->type].cpu/2+vms[it->type].mm/2;
            p = &(*it);
        }
    }
    lastSize = size;
    return p;

}
DeployedVM* findSmallestVmFromHost(const vector<PurchasedHost>& purchasedHost,int index){
    DeployedVM* p=nullptr;
    
    // 服务器上无虚拟机
    if (purchasedHost[index].listDVMA->size() == 0 && purchasedHost[index].listDVMB->size() == 0 
    && purchasedHost[index].listDVMBoth->size() == 0)
    {
        
        return p;
    }
    int size = 100000;
    // 
    for (auto  it = purchasedHost[index].listDVMA->begin(); it !=purchasedHost[index].listDVMA->end(); it++)
    {
        if (vms[it->type].cpu+vms[it->type].mm < size)
        {
            size = vms[it->type].cpu+vms[it->type].mm;
            p = &(*it);
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMB->begin(); it !=purchasedHost[index].listDVMB->end(); it++)
    {
        if (vms[it->type].cpu+vms[it->type].mm < size)
        {
            size = vms[it->type].cpu+vms[it->type].mm;
            p = &(*it);
        }
        
    }
    for (auto  it = purchasedHost[index].listDVMBoth->begin(); it !=purchasedHost[index].listDVMBoth->end(); it++)
    {
        // 超参数 单双虚拟机比较大小
        if (vms[it->type].cpu/2+vms[it->type].mm/2 < size)
        {
            size = vms[it->type].cpu/2+vms[it->type].mm/2;
            p = &(*it);
        }
    }
    return p;
}


int findBiggestHost(const vector<PurchasedHost>& purchasedHost,vector<int> disabledHost){
    int index = -1;
    int totalSize = -1;
    for (auto it = purchasedHost.begin(); it != purchasedHost.end(); it++)
    {        
        if(it->remainCpuA+it->remainCpuB+it->remainMmA+it->remainMmB>totalSize&& disabledHost[it-purchasedHost.begin()]==0){
            index = it - purchasedHost.begin();
            totalSize = it->remainCpuA+it->remainCpuB+it->remainMmA+it->remainMmB;
        }
    }
    return index;
}


void migrantBeforeDeployed(vector<PurchasedHost>& purchasedHost,list<pair<int,pair<int,Node>>>& migrateInfos,int allowedCount){
 
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
            int upperLimit = purchasedHost[i].remainCpuA+purchasedHost[i].remainMmA+
                    purchasedHost[i].remainCpuB+purchasedHost[i].remainMmB;
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
            DeployedVM* p = findSmallerVmFromHost(purchasedHost,index,lastSize);
            if (p == nullptr)
            {
                // cout<<"warning host is empty!"<<endl;
                disabledHost[index] = 1;
                break;
            }
            // cout<<lastSize;
            int upperLimit = 1000000;

            int targetIndex = findBestFitHost(p,purchasedHost,disabledHost,upperLimit);
            if (lastSize > upperLimit)
            {
                lastSize = (upperLimit*5)/7;
            }
            
            // int upperLimit = purchasedHost[targetIndex].remainCpuA+purchasedHost[targetIndex].remainMmA+
            //         purchasedHost[targetIndex].remainCpuB+purchasedHost[targetIndex].remainMmB;
            // if (lastSize>(upperLimit*2/3))
            // {
            //     lastSize = upperLimit*2/3;
            // }
            
            //如果找不到就找下一个
            if (targetIndex == -1)
            {
                continue;
            }
            if(disabledHost[targetIndex] == 1){
                if(lastSize%5==0){
                    allowedCount -= (int)migrateInfos.size()/25;
                }
                continue;
            }
            // cout<<"  "<<index<<" moveto! "<<targetIndex<<endl;
            Node targetNode= Both;
            if (p->deployedNode == A || p->deployedNode == B)
            {
                if (purchasedHost[targetIndex].isRemainResourceAvailForA(vms[p->type].cpu,vms[p->type].mm)&&
                purchasedHost[targetIndex].isRemainResourceAvailForB(vms[p->type].cpu,vms[p->type].mm))
                {
                    // 超参数
                    if (purchasedHost[targetIndex].remainCpuA+purchasedHost[targetIndex].remainMmA<
                    purchasedHost[targetIndex].remainCpuB+purchasedHost[targetIndex].remainMmB)
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
                migratePhysicalToA(purchasedHost,p->vmId,targetIndex);
            }else if (targetNode == B)
            {
                migratePhysicalToB(purchasedHost,p->vmId,targetIndex);
            }else{
                migratePhysicalToBoth(purchasedHost,p->vmId,targetIndex);
            }
        }
        ///////////
      
    }
    
}

#endif /* DataStructure_h */
