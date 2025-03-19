/*
* Copyright (c) 2018 The Pigeon Core developers
* Distributed under the MIT software license, see the accompanying
* file COPYING or http://www.opensource.org/licenses/mit-license.php.
* 
* FounderPayment.cpp
*
*  Created on: Jun 24, 2018
*      Author: Tri Nguyen
*/

#include "founder_payment.h"

#include "util.h"
#include "chainparams.h"
#include <boost/foreach.hpp>
#include "base58.h"

CAmount FounderPayment::getFounderPaymentAmount(int blockHeight, CAmount blockReward) {
    if (blockHeight < startBlock){
        return 0;
    }
    for(int i = 0; i < rewardStructures.size(); i++) {
        FounderRewardStructure rewardStructure = rewardStructures[i];
        if(rewardStructure.blockHeight == INT_MAX || blockHeight <= rewardStructure.blockHeight) {
            return blockReward * rewardStructure.rewardPercentage / 100;
        }
    }
    return 0;
}

void FounderPayment::FillFounderPayment(CMutableTransaction& txNew, int nBlockHeight, CAmount blockReward, CTxOut& txoutFounderRet) {
    // make sure it's not filled yet
    CAmount founderPayment = getFounderPaymentAmount(nBlockHeight, blockReward);

    txoutFounderRet = CTxOut();
    CScript payee;

//     std::string DEFAULT_FOUNDER_ADDRESS = "B5JsYYsjnbFYHDrpibAStTEGpxyZrPBVr3";
//     std::string NEW_FOUNDER_ADDRESS = "AHwprhBpA8nVngVf1YcgMTwx9RzDThnhR3";
//     std::string founderAddress;

//     if (nBlockHeight < 180000) {
//         founderAddress = DEFAULT_FOUNDER_ADDRESS;
//     } else {
//         founderAddress = NEW_FOUNDER_ADDRESS;
//     }

    std::string founderAddress = "B5JsYYsjnbFYHDrpibAStTEGpxyZrPBVr3";

    // fill payee with the foundFounderRewardStrcutureFounderRewardStrcutureer address
    CBitcoinAddress cbAddress(founderAddress);
    payee = GetScriptForDestination(cbAddress.Get());
    // GET FOUNDER PAYMENT VARIABLES SETUP

    // split reward between miner ...
    txNew.vout[0].nValue -= founderPayment;
    txoutFounderRet = CTxOut(founderPayment, payee);
    txNew.vout.push_back(txoutFounderRet);

    LogPrintf("FounderPayment::FillFounderPayment -- Founder payment %lld to %s\n", founderPayment, founderAddress.c_str());
}

bool FounderPayment::IsBlockPayeeValid(const CTransaction& txNew, const int height, const CAmount blockReward) {
    CScript payee;
    // fill payee with the founder address
    payee = GetScriptForDestination(CBitcoinAddress(founderAddress).Get());
    const CAmount founderReward = getFounderPaymentAmount(height, blockReward);
    BOOST_FOREACH(const CTxOut& out, txNew.vout) {
        if(out.scriptPubKey == payee && out.nValue >= founderReward) {
            return true;
        }
    }

    return false;
}


