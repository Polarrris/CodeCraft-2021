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
#include <map>
#include <unordered_map>
#include <deque>
#include <ctime>
#include <utility>
#include <cstdio>
#include <algorithm>
#include <math.h>
#include <cmath>

#include "Input.h"
#include "Deployment.h"
#include "PurchasePlanToday.h"
#include "PiecesPlanTommorrow.h"

using namespace std;


int main(){
    srand((unsigned)time(NULL));
    
unsigned long costcostcost = 0;
#ifndef LOCAL
    clock_t start, finish;
    start = clock();
    const string filePath = "/Users/jesse/Desktop/CodeCraft/training-data/training-1.txt";
    //输入
    if(freopen(filePath.c_str(),"rb",stdin)){
#endif
    
    int yesterdayPurchasedHostNext = 0;
    int dayCount = getAllInput();
    //开始每天的流程
    for(int day = 0; day < dayCount; day++){
#pragma mark 状态0：读入当日和明天请求
        remainDays = dayCount - day;
        requestInfos.clear();
        requestInfos = allRequestInfos[day];

#pragma mark 状态1：根据昨天的碎片规划在已购买机器上部署新虚拟机（第一天跳过）
    if (day != 0) {
        
    }
        
#pragma mark 状态2：制定购买计划（昨天被迁出的虚拟机+今天未被处理的add请求）
    planningVMs.clear();
    addRequestToPlanningVMs();
    initPlanningVMs();
    int _after = randomOpt();
    costcostcost += _after;
    cout << "After: " << _after << endl;
        
#pragma mark 状态3：根据购买计划购买新机器并在上面执行今天的add请求，输出今天的部署方案
    yesterdayPurchasedHostNext += purchaseAndDeployByPlan(yesterdayPurchasedHostNext);
    if (day != 0)
        printYesterdayMigrationPlan();
    else
        printf("(migration, 0)\n");
    printDeploymentResult();
#pragma mark 状态4：处理今天的del请求
    for (int i = 0; i < requestInfos.size(); i++) {
        if (requestInfos[i].infoType == del) {
            purchasedHosts.at(vmIdToDeployedVM[requestInfos[i].vmId]->hostIndex).delvm(*vmIdToDeployedVM[requestInfos[i].vmId]);
        }
    }
#pragma mark 状态5：整理今天del后的碎片并规划给明天使用，决定哪些机器被迁移
    if (day + 1 != dayCount)
        requestInfosTomorrow = allRequestInfos[day+1];
    purchasedHostsToResourcePieces();
        
    }
        
#ifndef LOCAL
    }
    else
    {
        cerr <<"file not found" << endl;
    }
    finish = clock();
    cout << "total:" << costcostcost << endl;
    printf("\nuse time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
#endif
    return 0;
}

