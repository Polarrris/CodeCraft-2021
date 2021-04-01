//
//  Input.h
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/15.
//

#ifndef Input_h
#define Input_h
#include "DataStructure.h"
using namespace std;

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

void getHostsAndVms(){
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
}

#endif /* Input_h */