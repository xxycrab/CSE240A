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
uint8_t *gshareBHT;
uint32_t ghistory;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor() {
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
        case CUSTOM:
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

uint8_t
make_prediction(uint32_t pc) {
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
        case CUSTOM:
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
void shift_predictor(uint32_t pc, uint8_t outcome) {
    int BHTindex = (pc ^ ghistory) & ((1 << ghistoryBits) - 1);
    if (outcome == TAKEN) {
        if (gshareBHT[BHTindex] != ST)
            gshareBHT[BHTindex]++;
    } else {
        if (gshareBHT[BHTindex] != SN)
            gshareBHT[BHTindex]--;
    }
}

void
train_predictor(uint32_t pc, uint8_t outcome) {
    //
    //TODO: Implement Predictor training
    //

    // train the predictor based on the bpType
    switch (bpType) {
        case STATIC:
            return;
        case GSHARE:
            shift_predictor(pc, outcome);
            ghistory <<= 1;
            ghistory |= outcome;
            break;
        case TOURNAMENT:
        case CUSTOM:
        default:
            break;
    }
}
