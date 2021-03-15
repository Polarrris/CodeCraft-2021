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
} node;

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
    int hostCost;
    int dailyCost;
};

vector<Host> hosts;

//虚拟机信息
struct VmProperties {
    int cpu;
    int mm;
    bool isDuet;
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
    node deployedNode;
    
    DeployedVM(string type, int date, int host, node deployedNode){
        this->type = type;
        this->date = date;
        this->host = host;
        this->deployedNode = deployedNode;
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
    
    list <DeployedVM>* listDVMA;
    list <DeployedVM>* listDVMB;
    list <DeployedVM>* listDVMBoth;
    
    PurchasedHost(int cpu, int mm){
        remainCpuA = cpu/2;
        remainCpuB = cpu/2;
        remainMmA = mm/2;
        remainMmB = mm/2;
        listDVMA = new list<DeployedVM>();
        listDVMB = new list<DeployedVM>();
        listDVMBoth = new list<DeployedVM>();
    }
    
    DeployedVM* addSingleA(string type, int index, int date){
        remainCpuA = remainCpuA - vms[type].cpu;
        remainMmA = remainMmA - vms[type].mm;
        DeployedVM* tmpVM = new DeployedVM(type, date, index, A);
        listDVMA->push_front(*tmpVM);
        delete tmpVM;
        remainCMRatioA = (float)remainCpuA / remainMmA;
        return &listDVMA->front();
    }
    
    DeployedVM* addSingleB(string type, int index, int date){
        remainCpuB = remainCpuB - vms[type].cpu;
        remainMmB = remainMmB - vms[type].mm;
        DeployedVM* tmpVM = new DeployedVM(type, date, index, B);
        listDVMB->push_front(*tmpVM);
        delete tmpVM;
        remainCMRatioB = (float)remainCpuB / remainMmB;
        return &listDVMB->front();
    }
    
    DeployedVM* addBoth(string type, int index, int date){
        remainCpuA = remainCpuA - vms[type].cpu/2;
        remainMmA = remainMmA - vms[type].mm/2;
        remainCpuB = remainCpuB - vms[type].cpu/2;
        remainMmB = remainMmB - vms[type].mm/2;
        DeployedVM* tmpVM = new DeployedVM(type, date, index, Both);
        listDVMBoth->push_front(*tmpVM);
        delete tmpVM;
        remainCMRatioBoth = (float)(remainCpuA+remainCpuB) / (remainMmA+remainMmB);
        return &listDVMBoth->front();
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
        delete listDVMA;
        delete listDVMB;
        delete listDVMBoth;
    }
};

//已购买的主机
vector<PurchasedHost> *purchasedHosts;



#endif /* DataStructure_h */
