//
//  Output.h
//  CodeCraft
//
//  Created by Jesse Chou on 2021/3/15.
//

#ifndef Output_h
#define Output_h
#include "DataStructure.h"
/*
 规范化输出过程定义
 */

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

#endif /* Output_h */
