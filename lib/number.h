#pragma once
#include <cinttypes>
#include <iostream>


struct uint239_t {
    static const uint8_t kSize = 35;
    static const uint8_t kBase = 128;
    static const uint8_t kLength = 245;
    static const uint8_t kServiceBit = 0x80;
    static const uint8_t kSignBits = 0x7F;
    static const uint8_t kHigherSignBit = 0x40;
    static const uint8_t kSmallerBit = 0x01;
    static const uint16_t kByte = 8;
    static const uint8_t kTwoBaseForTenDigit = 2;
    uint8_t data[kSize] {};
};

static_assert(sizeof(uint239_t) == 35, "Size of uint239_t must be no higher than 35 bytes");

uint239_t FromInt(uint32_t value, uint32_t shift);

uint239_t FromString(const char* str, uint32_t shift);

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs);

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs);

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs);

bool operator>=(const uint239_t& lhs, const uint239_t& rhs);

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs);

bool operator==(const uint239_t& lhs, const uint239_t& rhs);

bool operator!=(const uint239_t& lhs, const uint239_t& rhs);

std::ostream& operator<<(std::ostream& stream, const uint239_t& value);

uint64_t GetShift(const uint239_t& value);