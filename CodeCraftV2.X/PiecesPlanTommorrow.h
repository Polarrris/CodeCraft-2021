//
//  PiecesPlanTommorrow.h
//  CodeCraftV2.X
//
//  Created by Jesse Chou on 2021/3/22.
//

#ifndef PiecesPlanTommorrow_h
#define PiecesPlanTommorrow_h
/*
 该过程的目标在于最小化明天部署后的碎片
 */
#define MigrationRestrict
class ResourcePiece{
public:
    int hostIndex;
    int remainCpuA;
    int remainCpuB;
    int remainMmA;
    int remainMmB;
    vector <DeployedVM> preDeployedVMs;
    deque<RequestInfo> delRequests;
    
    ResourcePiece(int _hostIndex){
        hostIndex = _hostIndex;
        remainCpuA = purchasedHosts[hostIndex].remainCpuA;
        remainCpuB = purchasedHosts[hostIndex].remainCpuB;
        remainMmA = purchasedHosts[hostIndex].remainMmA;
        remainMmB = purchasedHosts[hostIndex].remainMmB;
    }
    ResourcePiece(RequestInfo& _delRequest){
        hostIndex = vmIdToDeployedVM[_delRequest.vmId]->hostIndex;
        if (vmIdToDeployedVM[_delRequest.vmId]->deployedNode == Both) {
            remainCpuA = vms[_delRequest.vmType].cpu/2;
            remainCpuB = vms[_delRequest.vmType].cpu/2;
            remainMmA = vms[_delRequest.vmType].mm/2;
            remainMmB = vms[_delRequest.vmType].mm/2;
            delRequests.push_back(_delRequest);
        }
        else if (vmIdToDeployedVM[_delRequest.vmId]->deployedNode == A){
            remainCpuA = vms[_delRequest.vmType].cpu;
            remainCpuB = 0;
            remainMmA = vms[_delRequest.vmType].mm;
            remainMmB = 0;
            delRequests.push_back(_delRequest);
        }
        else{
            remainCpuA = 0;
            remainCpuB = vms[_delRequest.vmType].cpu;
            remainMmA = 0;
            remainMmB = vms[_delRequest.vmType].mm;
            delRequests.push_back(_delRequest);
        }
    }
    //if hostIndex = delPieceHostIndex
    void mergeDelPiece(ResourcePiece& delPiece){
        remainCpuA += delPiece.remainCpuA;
        remainCpuB += delPiece.remainCpuB;
        remainMmA += delPiece.remainMmA;
        remainMmB += delPiece.remainMmB;
        delRequests.push_back(delPiece.delRequests.back());
    }
    
    Node decideSingleToAorB(int vmCpu, int vmMm){
        if (remainCpuA >= remainCpuB && remainMmA >= remainMmB) return A;
        if (remainCpuA < remainCpuB && remainMmA < remainMmB) return B;
        //向量模型
        if (pow(remainCpuA - vmCpu - remainCpuB, 2) + pow(remainMmA - vmMm - remainMmB, 2) > pow(remainCpuB - vmCpu - remainCpuA, 2) + pow(remainMmB - vmMm - remainMmA, 2))
            return B;
        else return A;
    }
};



vector<ResourcePiece> resourcePieces;

void purchasedHostsToResourcePieces(){
    for (int hostIndex = 0; hostIndex < purchasedHosts.size(); hostIndex++) {
        resourcePieces.push_back(ResourcePiece(hostIndex));
    }
}

void delRequestToResourcePiece(RequestInfo& _delRequest){
    ResourcePiece _delPiece(_delRequest);
    resourcePieces[vmIdToDeployedVM[_delRequest.vmId]->hostIndex].mergeDelPiece(_delPiece);
}

//如何判断是因为虚拟机自身太大放不进去还是因为满了而放不进去
bool matchAddRequests(int addReqIndex){
    VmProperties vmProperty = vms[requestInfosTomorrow[addReqIndex].vmType];
    if (vmProperty.isDuet == true) {
        for (int i = 0; i < resourcePieces.size(); i++) {
            if (vmProperty.cpu/2 < min(resourcePieces[i].remainCpuA, resourcePieces[i].remainCpuB) && vmProperty.mm/2 < min(resourcePieces[i].remainMmA, resourcePieces[i].remainMmB)){
                resourcePieces[i].preDeployedVMs.push_back(DeployedVM(requestInfosTomorrow[addReqIndex].vmType, i, Both, requestInfosTomorrow[addReqIndex].vmId));
                resourcePieces[i].remainCpuA -= vmProperty.cpu/2;
                resourcePieces[i].remainCpuB -= vmProperty.cpu/2;
                resourcePieces[i].remainMmA -= vmProperty.mm/2;
                resourcePieces[i].remainMmB -= vmProperty.mm/2;
                return true;
            }
        }
    }
    else {
        for (int i = 0; i < resourcePieces.size(); i++) {
            if (vmProperty.cpu < min(resourcePieces[i].remainCpuA, resourcePieces[i].remainCpuB) && vmProperty.mm < min(resourcePieces[i].remainMmA, resourcePieces[i].remainMmB)){
                if (resourcePieces[i].decideSingleToAorB(vmProperty.cpu, vmProperty.mm) == A) {
                    resourcePieces[i].remainCpuA -= vmProperty.cpu;
                    resourcePieces[i].remainMmA -= vmProperty.mm;
                    return true;
                }
                else{
                    resourcePieces[i].preDeployedVMs.push_back(DeployedVM(requestInfosTomorrow[addReqIndex].vmType, i, B, requestInfosTomorrow[addReqIndex].vmId));
                    resourcePieces[i].remainCpuB -= vmProperty.cpu;
                    resourcePieces[i].remainMmB -= vmProperty.mm;
                    return true;
                }
            }
        }
    }
    return false;
}

void planPiecesTommorrow(){
    int lastDelIndex = requestInfosTomorrow.size();
    for (auto req = requestInfosTomorrow.rbegin(); req != requestInfosTomorrow.rend() ; ++req) {
        if ((*req).infoType == del) {
            if (vmIdToDeployedVM.find((*req).vmId) == vmIdToDeployedVM.end())
                continue;
            for (int i = (*req).indexToday + 1; i < lastDelIndex; ++i) {
                matchAddRequests(i);
                
            }
            lastDelIndex = (*req).indexToday;
        }
    }
}

#pragma mark printYesterdayMigrationPlan
void printYesterdayMigrationPlan(){
    printf("(migration, ???)\n");
}
#endif /* PiecesPlanTommorrow_h */
