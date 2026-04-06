#pragma once

#include <memory>

#include "BestFit.h"
#include "FirstFit.h"
#include "IAllocationStrategy.h"
#include "NextFit.h"
#include "WorstFit.h"

enum class AlgorithmType {
    FirstFit,
    NextFit,
    BestFit,
    WorstFit
};

class AlgorithmFactory {
public:
    static std::unique_ptr<IAllocationStrategy> create(AlgorithmType type)
    {
        switch (type) {
        case AlgorithmType::FirstFit:
            return std::make_unique<FirstFit>();
        case AlgorithmType::NextFit:
            return std::make_unique<NextFit>();
        case AlgorithmType::BestFit:
            return std::make_unique<BestFit>();
        case AlgorithmType::WorstFit:
            return std::make_unique<WorstFit>();
        }
        return std::make_unique<FirstFit>();
    }
};
