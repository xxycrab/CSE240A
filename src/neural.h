#include <stdio.h>
#include <string.h>

// Data for path based neural predictor
#define n_PCSIZE 427
#define n_HISTORYLEN 28
#define n_WEIGHTLEN 8

#define n_MASK_PC(x) ((x * 137) % n_PCSIZE)

//Items usage:
// fp_1: 36
// fp_2: 42
// mm_1: 357
// mm_2: 1135
// int_1: 292
// int_2: 161


int16_t n_W[n_PCSIZE][n_HISTORYLEN + 1];
int8_t n_gHistory[n_HISTORYLEN];          // 1 * n_HISTORYLEN
int32_t n_shiftWeight[n_HISTORYLEN + 1];  // (log(n_HISTORYLEN + 1) + n_WEIGHTLEN) * (n_HISTORYLEN + 1)
uint32_t n_branches[n_HISTORYLEN + 1];    // log(PCSIZE) * (n_HISTORYLEN + 1)

uint8_t n_recentPrediction = NOTTAKEN;
uint8_t n_needTrain = 0;
int32_t n_trainTheta = 0, n_trainTheta0 = 0;

// Tuning
int32_t n_usageCount[n_PCSIZE];
int32_t n_usedSlots[n_PCSIZE];
int32_t n_conflictCount[n_PCSIZE];

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//


//====================== Neural Path-based Predictor ==============================

void neural_shift(int16_t *satuate, uint8_t same) {
    if (same) {
        if (*satuate != ((1 << (n_WEIGHTLEN - 1)) - 1)) {
            (*satuate)++;
        }
    } else {
        if (*satuate != -(1 << (n_WEIGHTLEN - 1))) {
            (*satuate)--;
        }
    }
}


void neural_path_init() {
    //printf("neural: historylen: %d, PCSIZE: %d, saturate=%d\n", n_HISTORYLEN, n_PCSIZE, n_WEIGHTLEN);
    n_trainTheta = (int32_t)(2.14 * (n_HISTORYLEN + 1) + 20.58);
    n_trainTheta0 = n_trainTheta;
    memset(n_W, 0, sizeof(int16_t) * n_PCSIZE * (n_HISTORYLEN + 1));

    memset(n_shiftWeight, 0, sizeof(int32_t) * (n_HISTORYLEN + 1));
    memset(n_branches, 0, sizeof(uint32_t) * (n_HISTORYLEN + 1));
    memset(n_gHistory, 0, sizeof(uint8_t) * n_HISTORYLEN);

    // Tuning
    memset(n_usageCount, 0, sizeof(int32_t) * n_PCSIZE);
    memset(n_usedSlots, 0, sizeof(int32_t) * n_PCSIZE);
    memset(n_conflictCount, 0, sizeof(int32_t) * n_PCSIZE);
}


uint8_t get_neural_prediction(uint32_t pc) {
    int32_t out = (int32_t)(n_W[n_MASK_PC(pc)][0]) + n_shiftWeight[n_HISTORYLEN];
    n_recentPrediction = (out >= 0) ? TAKEN : NOTTAKEN;
    n_needTrain = (out < n_trainTheta && out > -n_trainTheta) ? 1 : 0;

    return n_recentPrediction;
}


uint32_t n_additionalErr = 50;

void neural_train(uint32_t pc, uint8_t outcome) {
    // Add this branch pc to branch history list
    for (int i = n_HISTORYLEN; i >= 1; i--) {
        n_branches[i] = n_branches[i - 1];
    }
    n_branches[0] = n_MASK_PC(pc);


    // Update cumulative array
    for (int i = n_HISTORYLEN; i >= 1; i--) {
        n_shiftWeight[i] = n_shiftWeight[i - 1] +
                           ((outcome == TAKEN) ? n_W[n_MASK_PC(pc)][n_HISTORYLEN - i + 1] : -n_W[n_MASK_PC(pc)][
                                   n_HISTORYLEN - i + 1]);
    }
    n_shiftWeight[0] = 0;
    // perceptron learning rule
    if ((outcome != n_recentPrediction) || n_needTrain) {
        neural_shift(&(n_W[n_MASK_PC(pc)][0]), outcome);
        for (int i = 1; i <= n_HISTORYLEN; i++) {
            uint32_t k = n_branches[i];
            uint8_t predict = n_gHistory[i - 1];
            neural_shift(&(n_W[k][i]), (outcome == predict));
        }
    }


    // Update global history register
    for (int i = n_HISTORYLEN - 1; i > 0; i--) {
        n_gHistory[i] = n_gHistory[i - 1];
    }
    n_gHistory[0] = outcome;

}
