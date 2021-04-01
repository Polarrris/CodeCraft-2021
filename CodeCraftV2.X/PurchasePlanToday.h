//
//  PurchasePlanToday.h
//  CodeCraftV2.X
//
//  Created by Jesse Chou on 2021/3/22.
//

#ifndef PurchasePlanToday_h
#define PurchasePlanToday_h

using namespace std;
#include "Input.h"

#pragma mark VM&PLANNINGVM
struct VM{
    string type;
    int vmId;
    int cpu;
    int mm;
    Node planNode;
};

class PlanningVM{
public:
    int cpuA;
    int cpuB;
    int mmA;
    int mmB;
    deque<VM> planVMs;
    Host plannedHost;
    
    PlanningVM(){
        cpuA = cpuB = mmA = mmB = 0;
        plannedHost.hostType = "";
    }
    
    PlanningVM(const PlanningVM& _origin){
        cpuA = _origin.cpuA;
        cpuB = _origin.cpuB;
        mmA = _origin.mmA;
        mmB = _origin.mmB;
        for (auto it = _origin.planVMs.begin(); it != _origin.planVMs.end(); it++) {
            planVMs.push_back(*it);
        }
        plannedHost = _origin.plannedHost;
    }
    
    void updateResource(){
        int _cpuA, _cpuB, _mmA, _mmB;
        _cpuA = _cpuB = _mmA = _mmB = 0;
        for (auto it = planVMs.begin(); it !=planVMs.end(); it++) {
            if ((*it).planNode == A) {
                _cpuA += (*it).cpu;
                _mmA += (*it).mm;
            }
            else if ((*it).planNode == B){
                _cpuB += (*it).cpu;
                _mmB += (*it).mm;
            }
            else{
                _cpuA += (*it).cpu/2;
                _mmA += (*it).mm/2;
                _cpuB += (*it).cpu/2;
                _mmB += (*it).mm/2;
            }
        }
        cpuA = _cpuA;
        cpuB = _cpuB;
        mmA = _mmA;
        mmB = _mmB;
    }
    
    Node decideSingleToAorB(int vmCpu, int vmMm){
        if (cpuA >= cpuB && mmA >= mmB) return B;
        if (cpuA < cpuB && mmA < mmB) return A;
        if (pow(cpuA + vmCpu - cpuB, 2) + pow(mmA + vmMm - mmB, 2) > pow(cpuB + vmCpu - cpuA, 2) + pow(mmB + vmMm - mmA, 2))
            return B;
        else return A;
    }
    
};

vector<PlanningVM> planningVMs;

#pragma mark vmSwap\vmMove\vmDevide\vmMerge
//基本动作：交换、移动、拆分、合并
void vmSwap(PlanningVM& planningVMFirst, PlanningVM& planningVMSecond, int vmIndexFirst, int vmIndexSecond){
    auto it1 = planningVMFirst.planVMs.begin() + vmIndexFirst;
    auto it2 = planningVMSecond.planVMs.begin() + vmIndexSecond;
    VM _tmpVM1 = *it1;
    VM _tmpVM2 = *it2;
    planningVMFirst.planVMs.erase(it1);
    planningVMSecond.planVMs.erase(it2);
    if (_tmpVM1.planNode != Both) {
        _tmpVM1.planNode = planningVMSecond.decideSingleToAorB(_tmpVM1.cpu, _tmpVM1.mm);
    }
    if (_tmpVM2.planNode != Both) {
        _tmpVM2.planNode = planningVMFirst.decideSingleToAorB(_tmpVM2.cpu, _tmpVM2.mm);
    }
    planningVMFirst.planVMs.push_back(_tmpVM2);
    planningVMSecond.planVMs.push_back(_tmpVM1);
    planningVMFirst.updateResource();
    planningVMSecond.updateResource();
}

void vmMove(PlanningVM& planningVMFirst, PlanningVM& planningVMSecond, int vmIndexFirst){
    auto it = planningVMFirst.planVMs.begin() + vmIndexFirst;
    VM _tmpVM1 = *it;
    if (_tmpVM1.planNode != Both) {
        _tmpVM1.planNode = planningVMSecond.decideSingleToAorB(_tmpVM1.cpu, _tmpVM1.mm);
    }
    planningVMSecond.planVMs.push_back(_tmpVM1);
    planningVMFirst.planVMs.erase(it);
    planningVMFirst.updateResource();
    planningVMSecond.updateResource();
}

PlanningVM vmDevide(PlanningVM& planningVMFirst){
    PlanningVM _planningVM;
    int num = planningVMFirst.planVMs.size()/2;
    int vmIndexFirst;
    for (int i = 0; i < num; i++){
        vmIndexFirst = rand()%(planningVMFirst.planVMs.size()-i);
        vmMove(planningVMFirst, _planningVM, vmIndexFirst);
    }
    return _planningVM;
}

PlanningVM vmMerge(PlanningVM& planningVMFirst, PlanningVM& planningVMSecond){
//    if (planningVMFirst.planVMs.size() > planningVMSecond.planVMs.size()) {
//        while (planningVMSecond.planVMs.size() != 0) {
//            vmMove(planningVMSecond, planningVMFirst, 0);
//        }
//        return planningVMFirst;
//    }
//    else{
//        while (planningVMFirst.planVMs.size() != 0) {
//            vmMove(planningVMFirst, planningVMSecond, 0);
//        }
//        return planningVMSecond;
//    }
    PlanningVM _planningVMtmp;
    if (planningVMFirst.planVMs.size() > planningVMSecond.planVMs.size()) {
        _planningVMtmp = planningVMFirst;
        while (planningVMSecond.planVMs.size() != 0) {
            vmMove(planningVMSecond, _planningVMtmp, 0);
        }
        return _planningVMtmp;
    }
    else{
        _planningVMtmp = planningVMSecond;
        while (planningVMFirst.planVMs.size() != 0) {
            vmMove(planningVMFirst, _planningVMtmp, 0);
        }
        return _planningVMtmp;
    }
}

#pragma mark addRequestToPlanningVMs
void addRequestToPlanningVMs(){
    int randCount = 0;
    PlanningVM _planningVM;
    VM _vm;
    
    randCount = rand()%41 + 5;//5-45
    int _count = 0;
    for (auto it = requestInfos.begin(); it != requestInfos.end(); ++it) {
        if (((*it).infoType == add && (*it).isPlanned == false) || (*it).infoType == mig) {//重要变化
            _vm.cpu = vms[(*it).vmType].cpu;
            _vm.mm = vms[(*it).vmType].mm;
            _vm.vmId = (*it).vmId;
            _vm.type = (*it).vmType;
            if (vms[(*it).vmType].isDuet == true) {
                _vm.planNode = Both;
            }
            else
                _vm.planNode = _planningVM.decideSingleToAorB(_vm.cpu, _vm.mm);
            (*it).isPlanned = true;
            _planningVM.planVMs.push_back(_vm);
            _planningVM.updateResource();
            ++_count;
        }
        if (_count == randCount || it + 1 == requestInfos.end()) {
            if (_planningVM.planVMs.size() == 0) {
                break;//防止count刷新的时候，it+1刚好等于requestInfos.end()
            }
            _planningVM.updateResource();
            planningVMs.push_back(_planningVM);
            _planningVM.planVMs.clear();
            randCount = rand()%41 + 5;
            ++_count = 0;
        }
    }
}

#pragma mark getIterationNum
int getIterationNum(int planningVMSize){
//    if (planningVMSize <= 50) {
//        return 500;
//    }
//    else if (planningVMSize <= 99) {
//        return 2000;
//    }
//    else if (planningVMSize <= 999) {
//        return 5000;
//    }
//    else return 10000;
    return 100*planningVMSize;
}

#pragma mark getTotalCost
int getTotalCost(){
    int planningVMIndex = 0;
    int _totalcost = 0;
    while (planningVMIndex != planningVMs.size()) {
        _totalcost += planningVMs[planningVMIndex].plannedHost.hostCost + planningVMs[planningVMIndex].plannedHost.dailyCost * remainDays;
        planningVMIndex++;
    }
    return _totalcost;
}

#pragma mark matchHostOfPlanningVM
//若返回type为空，撤回操作。
Host matchHostOfPlanningVM(PlanningVM& inputPlanningVM){
    Host _bestHost;
    _bestHost.hostType = "";
    _bestHost.hostCost = 1000000;
    _bestHost.dailyCost = 10000;
    int bestHostIndex = -1;
    for (int hostIndex = 0; hostIndex < hosts.size(); ++hostIndex) {
        if ((hosts[hostIndex].cpu/2 > max(inputPlanningVM.cpuA, inputPlanningVM.cpuB)) &&
            (hosts[hostIndex].mm/2 > max(inputPlanningVM.mmA, inputPlanningVM.mmB))) {
            if (hosts[hostIndex].hostCost + hosts[hostIndex].dailyCost * remainDays < _bestHost.hostCost + _bestHost.dailyCost * remainDays)
                bestHostIndex = hostIndex;
        }
    }
    if (bestHostIndex != -1) {
        _bestHost = hosts[bestHostIndex];
    }
    return _bestHost;
}

#pragma mark initPlanningVMs
//给每组分配一个vm
void initPlanningVMs(){
    Host bestHost;
    int planningVMIndex = 0;
    while(planningVMIndex < planningVMs.size()){
        bestHost = matchHostOfPlanningVM(planningVMs[planningVMIndex]);
        if (bestHost.hostType == "") {
            PlanningVM _planningVM = vmDevide(planningVMs[planningVMIndex]);
            planningVMs.push_back(_planningVM);
            --planningVMIndex;
        }
        else
            planningVMs[planningVMIndex].plannedHost = bestHost;
        ++planningVMIndex;
if(planningVMs[planningVMIndex].planVMs.size() == 0 && planningVMIndex < planningVMs.size())
    cout << "???";
    }
}


void whereIsTheFuckingBug(int num, int totalCost){
    for (auto it = planningVMs.begin(); it != planningVMs.end(); it++) {
        if ((*it).planVMs.size() == 0 || (*it).plannedHost.hostType == "") {
            cout << num << " What's your problem???" << endl;
        }
    }
    if (totalCost < 0) {
        cout << "What the fuck???";
    }
}

#pragma mark costCompare
//大于零指示操作，等于零指示不操作
int costCompare(PlanningVM& planningVMFirst, PlanningVM& planningVMSecond, PlanningVM& tmpPlanningVM1, PlanningVM& tmpPlanningVM2, Host& resHost1, Host& resHost2){
    resHost1 = matchHostOfPlanningVM(tmpPlanningVM1);
    resHost2 = matchHostOfPlanningVM(tmpPlanningVM2);
    if (resHost1.hostType == "" || resHost2.hostType == "")
        return -1;
    int doneCost = resHost1.hostCost + resHost1.dailyCost * remainDays + resHost2.hostCost + resHost2.dailyCost * remainDays;
    int undoneCost = 0;
    if (planningVMSecond.plannedHost.hostType == "")//Devide
        undoneCost = planningVMFirst.plannedHost.hostCost + planningVMFirst.plannedHost.dailyCost * remainDays;
    else
        undoneCost = planningVMFirst.plannedHost.hostCost + planningVMFirst.plannedHost.dailyCost * remainDays + planningVMSecond.plannedHost.hostCost + planningVMSecond.plannedHost.dailyCost * remainDays;
    if (doneCost < undoneCost) {
        return undoneCost - doneCost;
    }
    else
        return 0;
}

#pragma mark randomOpt
int randomOpt(){
    int _iterations = getIterationNum(planningVMs.size());
    int totalCost = getTotalCost();
//cout << "Before: " << totalCost <<endl;
    PlanningVM tmpPlanningVM1, tmpPlanningVM2;
    Host resHost1, resHost2;
    for (int iterNum = 0; iterNum < _iterations; iterNum++) {
        int ranOp = rand()%4;
        int planningVMFirstIndex, planningVMSecondIndex;
        do {
            planningVMFirstIndex = rand()%planningVMs.size();
            planningVMSecondIndex = rand()%planningVMs.size();
        } while (planningVMFirstIndex == planningVMSecondIndex);
        tmpPlanningVM1 = planningVMs[planningVMFirstIndex];
        tmpPlanningVM2 = planningVMs[planningVMSecondIndex];
        resHost1.hostType = "";
        resHost2.hostType = "";
        resHost1.hostCost = resHost2.hostCost = 1000000;
        resHost1.dailyCost = resHost2.dailyCost = 10000;
        switch (ranOp) {
            //Swap
            case 0:{
                if (tmpPlanningVM1.planVMs.size() == 1 || tmpPlanningVM2.planVMs.size() == 1) {
                    --iterNum;
                    break;
                }
                int vmIndexFirst, vmIndexSecond;
                do {
                    vmIndexFirst = rand()%tmpPlanningVM1.planVMs.size();
                    vmIndexSecond = rand()%tmpPlanningVM2.planVMs.size();
                } while (vmIndexFirst == vmIndexSecond);
                vmSwap(tmpPlanningVM1, tmpPlanningVM2, vmIndexFirst, vmIndexSecond);
                int res = costCompare(planningVMs[planningVMFirstIndex], planningVMs[planningVMSecondIndex], tmpPlanningVM1, tmpPlanningVM2, resHost1, resHost2);
                if (res > 0) {
                    vmSwap(planningVMs[planningVMFirstIndex], planningVMs[planningVMSecondIndex], vmIndexFirst, vmIndexSecond);
                    planningVMs[planningVMFirstIndex].plannedHost = resHost1;
                    planningVMs[planningVMSecondIndex].plannedHost = resHost2;
                    totalCost = totalCost - res;
                }
                break;
            }
            //Move
            case 1:{
                if (tmpPlanningVM1.planVMs.size() == 1) {
                    --iterNum;
                    break;
                }
                int vmIndexFirst = rand()%tmpPlanningVM1.planVMs.size();
                vmMove(tmpPlanningVM1, tmpPlanningVM2, vmIndexFirst);
                int res = costCompare(planningVMs[planningVMFirstIndex], planningVMs[planningVMSecondIndex], tmpPlanningVM1, tmpPlanningVM2, resHost1, resHost2);
                if (res > 0) {
                    vmMove(planningVMs[planningVMFirstIndex], planningVMs[planningVMSecondIndex], vmIndexFirst);
                    planningVMs[planningVMFirstIndex].plannedHost = resHost1;
                    planningVMs[planningVMSecondIndex].plannedHost = resHost2;
                    totalCost = totalCost - res;
                    break;
                }
                --iterNum;
                break;
            }
            //Divide
            case 2:{
                if (tmpPlanningVM1.planVMs.size() == 1) {
                    --iterNum;
                    break;
                }
                tmpPlanningVM2 = vmDevide(tmpPlanningVM1);
                PlanningVM tmpPlanningVM3;
                tmpPlanningVM3.plannedHost.hostType = "";
                int res = costCompare(planningVMs[planningVMFirstIndex], tmpPlanningVM3, tmpPlanningVM1, tmpPlanningVM2, resHost1, resHost2);
                if (res > 0) {
                    vmDevide(planningVMs[planningVMFirstIndex]);
                    planningVMs[planningVMFirstIndex].plannedHost = resHost1;
                    tmpPlanningVM2.plannedHost = resHost2;
                    planningVMs.push_back(tmpPlanningVM2);
                    totalCost = totalCost - res;
                    break;
                }
                --iterNum;
                break;
            }
            //Merge
            case 3:{
                int undoneCost = tmpPlanningVM1.plannedHost.hostCost + tmpPlanningVM1.plannedHost.dailyCost * remainDays + tmpPlanningVM2.plannedHost.hostCost + tmpPlanningVM2.plannedHost.dailyCost * remainDays;
                PlanningVM tmpPlanningVM3 = vmMerge(tmpPlanningVM1, tmpPlanningVM2);
                Host resHost1 = matchHostOfPlanningVM(tmpPlanningVM3);
                int doneCost = resHost1.hostCost + resHost1.dailyCost * remainDays;
                if ( doneCost < undoneCost && resHost1.hostType != "") {
                    if ( planningVMFirstIndex > planningVMSecondIndex) {
                        auto it1 = planningVMs.begin() + planningVMFirstIndex;
                        planningVMs.erase(it1);
                        auto it2 = planningVMs.begin() + planningVMSecondIndex;
                        planningVMs.erase(it2);
                    }
                    else{
                        auto it1 = planningVMs.begin() + planningVMSecondIndex;
                        planningVMs.erase(it1);
                        auto it2 = planningVMs.begin() + planningVMFirstIndex;
                        planningVMs.erase(it2);
                    }
                    tmpPlanningVM3.plannedHost = resHost1;
                    planningVMs.push_back(tmpPlanningVM3);
                    totalCost = totalCost - (undoneCost - doneCost);
                    break;
                }
                --iterNum;
                break;
            }
            default:
                continue;
        }
    }
    return totalCost;
};

#endif /* PurchasePlanToday_h */
