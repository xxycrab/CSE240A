//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include "string.h"
#include "neural.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID = "PID";
const char *email = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint8_t *gshareBHT;  // BHT for gshare
uint32_t ghistory;  // global history

uint8_t *localBHT;  // local BHT for tournament
uint32_t *localPHT;  // PHT recording local history for tournament
uint8_t *globalBHT;  // global BHT for tournament
uint8_t *selector;  // selector fot tournament
uint8_t localOutcome;
uint8_t globalOutcome;
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_predictor() {
    //
    //TODO: Initialize Branch Predictor Data Structures
    //
    switch (bpType) {
        case STATIC:
            return;
        case GSHARE:
            ghistory = 0;
            gshareBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
            memset(gshareBHT, WN, sizeof(uint8_t) * (1 << ghistoryBits));
            break;
        case TOURNAMENT:
            ghistory = 0;
            localBHT = malloc((1 << lhistoryBits) * sizeof(uint8_t));
            localPHT = malloc((1 << pcIndexBits) * sizeof(uint32_t));
            globalBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
            selector = malloc((1 << ghistoryBits) * sizeof(uint8_t));
            memset(localBHT, WN, sizeof(uint8_t) * (1 << lhistoryBits));
            memset(localPHT, 0, sizeof(uint32_t) * (1 << pcIndexBits));
            memset(globalBHT, WN, sizeof(uint8_t) * (1 << ghistoryBits));
            memset(selector, WN, sizeof(uint8_t) * (1 << ghistoryBits));
            break;
        case CUSTOM:
            neural_path_init();
            break;
        default:
            break;
    }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//

uint8_t gshare_prediction(uint32_t pc) {
    int BHTindex = (pc ^ ghistory) & ((1 << ghistoryBits) - 1);
    uint8_t prediction = gshareBHT[BHTindex];
    uint8_t outcome = (prediction == WN || prediction == SN) ? NOTTAKEN : TAKEN;
    return outcome;
}


uint8_t tournament_prediction(uint32_t pc) {
    // local prediction
    int PHTindex = pc & ((1 << pcIndexBits) - 1);
    uint32_t localBHTindex = localPHT[PHTindex];
    uint8_t localPrediction = localBHT[localBHTindex];
    localOutcome = (localPrediction == WN || localPrediction == SN) ? NOTTAKEN : TAKEN;

    //global prediction
    int globalBHTindex = ghistory & ((1 << ghistoryBits) - 1);
    uint8_t globalPrediction = globalBHT[globalBHTindex];
    globalOutcome = (globalPrediction == WN || globalPrediction == SN) ? NOTTAKEN : TAKEN;

    // selection of predictor. If selector[globalBHTindex]==00 or 01, use global predictor; otherwise use local predictor
    uint8_t counter = selector[globalBHTindex];
    if (counter == WN || counter == SN) {
        return globalOutcome;
    } else {
        return localOutcome;
    }
}

uint8_t make_prediction(uint32_t pc) {
    //
    //TODO: Implement prediction scheme
    //

    // Make a prediction based on the bpType
    switch (bpType) {
        case STATIC:
            return TAKEN;
        case GSHARE:
            return gshare_prediction(pc);
        case TOURNAMENT:
            return tournament_prediction(pc);
        case CUSTOM:
            return get_neural_prediction(pc);
        default:
            break;
    }

    // If there is not a compatable bpType then return NOTTAKEN
    return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void gshare_shift_predictor(uint32_t pc, uint8_t outcome) {
    int BHTindex = (pc ^ ghistory) & ((1 << ghistoryBits) - 1);
    if (outcome == TAKEN) {
        if (gshareBHT[BHTindex] != ST)
            gshareBHT[BHTindex]++;
    } else {
        if (gshareBHT[BHTindex] != SN)
            gshareBHT[BHTindex]--;
    }
}

void tournament_shift_selector(uint8_t outcome) {
    int globalBHTindex = ghistory & ((1 << ghistoryBits) - 1);
    if (localOutcome == outcome) {
        if (selector[globalBHTindex] != ST)
            selector[globalBHTindex]++;
    } else {
        if (selector[globalBHTindex] != SN)
            selector[globalBHTindex]--;
    }
}

void tournament_shift_predictor(uint32_t pc, uint8_t outcome) {
    int PHTindex = pc & ((1 << pcIndexBits) - 1);
    uint32_t localBHTindex = localPHT[PHTindex];
    if (outcome == TAKEN) {
        if (localBHT[localBHTindex] != ST)
            localBHT[localBHTindex]++;
    } else {
        if (localBHT[localBHTindex] != SN)
            localBHT[localBHTindex]--;
    }

    int globalBHTindex = ghistory & ((1 << ghistoryBits) - 1);
    if (outcome == TAKEN) {
        if (globalBHT[globalBHTindex] != ST)
            globalBHT[globalBHTindex]++;
    } else {
        if (globalBHT[globalBHTindex] != SN)
            globalBHT[globalBHTindex]--;
    }
}

void tournament_update(uint32_t pc, uint8_t outcome) {
    if (localOutcome != globalOutcome) {
        tournament_shift_selector(outcome);
    }
    tournament_shift_predictor(pc, outcome); // shift local predictor
    int PHTindex = pc & ((1 << pcIndexBits) - 1);
    localPHT[PHTindex] <<= 1;
    localPHT[PHTindex] &= ((1 << lhistoryBits) - 1);
    localPHT[PHTindex] |= outcome;
    ghistory <<= 1;
    ghistory  &= ((1 << ghistoryBits) - 1);
    ghistory |= outcome;
}

void train_predictor(uint32_t pc, uint8_t outcome) {
    //
    //TODO: Implement Predictor training
    //

    // train the predictor based on the bpType
    switch (bpType) {
        case STATIC:
            return;
        case GSHARE:
            gshare_shift_predictor(pc, outcome);
            ghistory <<= 1;
            ghistory  &= ((1 << ghistoryBits) - 1);
            ghistory |= outcome;
            break;
        case TOURNAMENT:
            tournament_update(pc, outcome);
            break;
        case CUSTOM:
            neural_train(pc, outcome);
            break;
        default:
            break;
    }
}
