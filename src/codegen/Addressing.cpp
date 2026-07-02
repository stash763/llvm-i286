// Addressing Implementation

#include "codegen/Addressing.h"

namespace llvm_i286 {
namespace codegen {

struct Addressing::Impl {
    std::map<std::string, Segment> globalSegments;
    std::map<std::string, Segment> functionSegments;
    std::map<std::string, std::map<std::string, int>> localStackOffsets; // func -> var -> offset
};

Addressing::Addressing() : impl(std::make_unique<Impl>()) {}

Addressing::~Addressing() = default;

Segment Addressing::getSegmentForGlobal(const std::string& name) const {
    auto it = impl->globalSegments.find(name);
    if (it != impl->globalSegments.end()) {
        return it->second;
    }
    return Segment::Data; // Default to data segment
}

Segment Addressing::getSegmentForFunction(const std::string& name) const {
    auto it = impl->functionSegments.find(name);
    if (it != impl->functionSegments.end()) {
        return it->second;
    }
    return Segment::Code; // Default to code segment
}

void Addressing::setSegmentForGlobal(const std::string& name, Segment seg) {
    impl->globalSegments[name] = seg;
}

std::string Addressing::segmentToNasm(Segment seg) {
    switch (seg) {
        case Segment::Code: return "_TEXT";
        case Segment::Data: return "_DATA";
        case Segment::BSS: return "_BSS";
        case Segment::Stack: return "STACK";
        case Segment::Extra: return "_EXTRA";
    }
    return "_DATA";
}

std::string Addressing::segmentOverride(Segment seg) {
    switch (seg) {
        case Segment::Code: return "cs:";
        case Segment::Data: return "ds:";
        case Segment::BSS: return "ds:";
        case Segment::Stack: return "ss:";
        case Segment::Extra: return "es:";
    }
    return "";
}

int Addressing::getStackOffset(const std::string& funcName, const std::string& varName) const {
    auto funcIt = impl->localStackOffsets.find(funcName);
    if (funcIt != impl->localStackOffsets.end()) {
        auto varIt = funcIt->second.find(varName);
        if (varIt != funcIt->second.end()) {
            return varIt->second;
        }
    }
    return 0;
}

void Addressing::setStackOffset(const std::string& funcName, const std::string& varName, int offset) {
    impl->localStackOffsets[funcName][varName] = offset;
}

std::string toNasmReg(const std::string& reg) {
    // Convert LLVM register names to NASM format
    // LLVM uses % prefix, NASM doesn't
    if (reg.empty()) return "ax";
    
    // Remove % prefix if present
    std::string nasmReg = reg;
    if (nasmReg[0] == '%') {
        nasmReg = nasmReg.substr(1);
    }
    
    return nasmReg;
}

} // namespace codegen
} // namespace llvm_i286
