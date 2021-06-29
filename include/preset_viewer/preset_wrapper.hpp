#pragma once 

#include <variant>
#include <preset.pb.h>

namespace preset_helpers{
    
class preset_wrapper{
    enum class preset_state{
        error,
        undefined,
        bin,
        text,
        proto
    }
    std::variant<bool,QString,proto::Preset>
    
};
} //namespace preset_helpers