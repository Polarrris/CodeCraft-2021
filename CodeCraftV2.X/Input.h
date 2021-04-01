//
//  Input.h
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/15.
//

#ifndef Input_h
#define Input_h
using namespace std;
#pragma mark INPUT_DATASTRCTURE
int remainDays = 0;

//结点枚举类型
typedef enum : short {
    A,
    B,
    Both
} Node;

//请求信息枚举类型
typedef enum : short {
    add,
    del,
    mig
} InfoType;

//主机信息
struct Host {
    string hostType;
    int cpu;
    int mm;
    int hostCost;
    int dailyCost;
};
//主机类型到信息的映射
vector<Host> hosts;
//为兼容所作无奈之举
unordered_map<string, int> hostTypeToIndex;

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
    int indexToday;
    InfoType infoType;
    string vmType;
    int vmId;
    bool isPlanned;
};
vector<RequestInfo> requestInfos;
vector<RequestInfo> requestInfosTomorrow;
unordered_map<int, vector<RequestInfo>> allRequestInfos;

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
//    _host.cmRatio = (float)_host.cpu / _host.mm;
    _host.hostCost = string2Int(hostCost);
    _host.dailyCost = string2Int(dailyCost);
    hosts.push_back(_host);
}

void generateHostTypeToIndex(){
    int index = 0;
    for (auto it = hosts.begin(); it != hosts.end(); ++it) {
        hostTypeToIndex.insert(pair<string, int>((*it).hostType, index));
        ++index;
    }
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
void generateRequest(string &vmType, string &vmId, int indexToday){
    RequestInfo _requestInfo;
    vmType.substr(0,vmType.size() - 1);
    _requestInfo.infoType = add;
    _requestInfo.vmType = vmType.substr(0,vmType.size() - 1);
    _requestInfo.vmId = string2Int(vmId);
    _requestInfo.isPlanned = false;
    _requestInfo.indexToday = indexToday;
    requestInfos.push_back(_requestInfo);
}

//解析用户删除请求
void generateRequest(string &vmId, int indexToday){
    RequestInfo _requestInfo;
    _requestInfo.infoType = del;
    _requestInfo.vmId = string2Int(vmId);
    _requestInfo.isPlanned = false;
    _requestInfo.indexToday = indexToday;
    requestInfos.push_back(_requestInfo);
}

#pragma mark getAllInput
int getAllInput(){
    //解析hosts信息
    int hostCount = 0;
    scanf("%d",&hostCount);
    string hostType,cpu,mm,hostCost,dailyCost;
    for(int i =0;i<hostCount;i++){
        cin>>hostType>>cpu>>mm>>hostCost>>dailyCost;
        generateHost(hostType,cpu,mm,hostCost,dailyCost);
    }
    generateHostTypeToIndex();
    
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
    
    //解析所有的请求
    for (int day = 0; day < dayCount; day++) {
        scanf("%d",&requestCount);
        for(int i = 0; i < requestCount; i++){
            cin>>infoType;
            if(infoType[1] == 'a'){
                cin>>vmType>>vmId;
                generateRequest(vmType,vmId, i);
            }
            else{
                cin>>vmId;
                generateRequest(vmId, i);
            }
        }
        allRequestInfos.insert(pair<int, vector<RequestInfo>>(day, requestInfos));
        requestInfos.clear();
    }
    return dayCount;
}

#endif /* Input_h */
