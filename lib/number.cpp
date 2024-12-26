#include "number.h"


void ReverseString(const char* str, char* reversed) {
    int len = strlen(str);
    for (int i = 0; i < len; ++i) {
        reversed[i] = str[len - 1 - i];
    }
    reversed[len] = '\0';
}

bool operator>=(const uint239_t&lhs, const uint239_t& rhs) {
    for (int i = 0; i < uint239_t::kSize; ++i) {
        if (lhs.data[i] > rhs.data[i]) {
            return true;
        }
        if (lhs.data[i] < rhs.data[i]) { 
            return false;
        }
    }
    return true;
}

int GetPowOfTwo(uint32_t& shift) {
    int pow_of_two = 0;
    for (int i = 0; i < shift; ++i) {
        if (shift >= pow(uint239_t::kTwoBaseForTenDigit, pow_of_two + 1)) {
            ++pow_of_two;
        } else {
            break;
        }
    }
    return pow_of_two;
}

void ReverseUint239_t(uint239_t& value) {
    for (int i = 0; i < (uint239_t::kSize / 2); ++i) {
        std::swap(value.data[i], value.data[uint239_t::kSize - 1 - i]);
    }
}

uint239_t ShiftForward(const uint239_t& value, uint32_t& shift) {
    uint239_t result = value;
    for (int j = 1; j <= shift; ++j) {
        uint8_t last_bite_temp = (result.data[0] & uint239_t::kHigherSignBit) >> (uint239_t::kByte - 2);
        result.data[0] <<= 1;
        result.data[0] &= uint239_t::kSignBits;
        uint8_t temp;
        for (int i = 1; i < uint239_t::kSize; ++i) {
            temp = (result.data[i] & uint239_t::kHigherSignBit) >> (uint239_t::kByte - 2);
            result.data[i] <<= 1;
            result.data[i] &= uint239_t::kSignBits;
            result.data[i - 1] |= temp;
        }
        result.data[uint239_t::kSize - 1] |= last_bite_temp; 
    }
    int pow_of_two = GetPowOfTwo(shift);
    while (shift > 0) {
        result.data[uint239_t::kSize - 1 - pow_of_two] |= uint239_t::kServiceBit;
        shift -= pow(uint239_t::kTwoBaseForTenDigit, pow_of_two);
        pow_of_two = GetPowOfTwo(shift);
    }
    return result;
}

uint239_t ShiftBack(const uint239_t& value) {
    uint239_t result = value;
    uint32_t shift = GetShift(value);
    for (int j = 0; j < shift; ++j) {
        uint8_t first_shift_bite = (result.data[uint239_t::kSize - 1] & uint239_t::kSmallerBit) << (uint239_t::kByte - 2);
        uint8_t temp = 0;
        result.data[uint239_t::kSize - 1] &= uint239_t::kSignBits;
        result.data[uint239_t::kSize - 1] >>= 1;
        for (int i = uint239_t::kSize - 2; i >= 0; --i) {
                temp = (result.data[i] & uint239_t::kSmallerBit) << (uint239_t::kByte - 2);
                result.data[i] &= uint239_t::kSignBits;
                result.data[i] >>= 1;
                result.data[i + 1] |= temp;
            }
        result.data[0] |= first_shift_bite;
    }
    return result;
}

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t result;
    int next_value_index = uint239_t::kSize - 1;
    while (value > 0) {
        result.data[next_value_index] = value % uint239_t::kBase;
        --next_value_index;
        value /= uint239_t::kBase;
    }
    result = ShiftForward(result, shift);
    return result;
}

uint239_t FromString(const char* str, uint32_t shift) {
    uint239_t result = FromInt(0, 0);
    uint239_t temp, ten_uint239 = FromInt(10, 0);
    char string[strlen(str)];
    ReverseString(str, string);
    for (int i = 0; i < strlen(string); ++i) {
        if (!('0' <= string[i] && string[i] <= '9')) {
            throw std::runtime_error("Invalid symbol in string");
        }
        temp = FromInt((string[i] - '0'), 0);
        for (int j = 0; j < i; ++j) {
            temp = temp * ten_uint239;
        }
        result = result + temp;
    }
    result = ShiftForward(result, shift);
    return result;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_without_shift = ShiftBack(lhs);
    uint239_t rhs_without_shift = ShiftBack(rhs);
    uint16_t temp_sum = 0;
    uint239_t result;
    for (int i = uint239_t::kSize - 1; i >= 0; --i) {
        temp_sum += lhs_without_shift.data[i] + rhs_without_shift.data[i];
        result.data[i] = temp_sum & (uint239_t::kBase - 1);
        temp_sum >>= (uint239_t::kByte - 1);
    }
    result.data[0] &= uint239_t::kSmallerBit;
    uint32_t shift = GetShift(lhs) + GetShift(rhs);
    result = ShiftForward(result, shift);
    return result;
}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result;
    uint239_t lhs_without_shift = ShiftBack(lhs);
    uint239_t rhs_without_shift = ShiftBack(rhs);
    for (int i = uint239_t::kSize - 1; i >= 0; --i) {
        result.data[i] = (~rhs_without_shift.data[i]) & uint239_t::kSignBits;
    }
    result = (result + FromInt(1, 0)) + lhs_without_shift;
    long shift = GetShift(lhs) - GetShift(rhs);
    if (shift < 0) {
        shift = (pow(uint239_t::kTwoBaseForTenDigit, uint239_t::kSize) + shift);
    }
    uint32_t shift_normal = static_cast<uint32_t>(shift);
    result = ShiftForward(result, shift_normal);
    return result;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_without_shift = ShiftBack(lhs);
    uint239_t rhs_without_shift = ShiftBack(rhs);
    uint239_t result;
    uint16_t temp = 0;
    ReverseUint239_t(lhs_without_shift);
    ReverseUint239_t(rhs_without_shift);
    for (int i = 0; i < uint239_t::kSize; ++i) {
        for (int j = 0; j < uint239_t::kSize - i; ++j) {
            temp += lhs_without_shift.data[i] * rhs_without_shift.data[j];
            temp += result.data[i + j];
            result.data[i + j] = temp & (uint239_t::kBase - 1);
            temp >>= (uint239_t::kByte - 1);
        }
    }
    ReverseUint239_t(result);
    result.data[0] &= uint239_t::kSmallerBit;
    uint32_t shift = GetShift(lhs) + GetShift(rhs);
    result = ShiftForward(result, shift);
    return result;
}

void DeleteShiftBits(uint239_t& value) {
    for (int i = 0; i < uint239_t::kSize; ++i) {
        value.data[i] &= uint239_t::kSignBits;
    }
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_without_shift = ShiftBack(lhs);
    uint239_t rhs_without_shift = ShiftBack(rhs);
    if (rhs == FromInt(0, 0)) {
        throw std::runtime_error("Division by zero");
    }
    uint32_t one_shift;
    uint239_t result;
    uint239_t remainder;
    for (int i = 0; i < uint239_t::kSize; ++i) {
        for (int j = uint239_t::kByte - 2; j >= 0; --j) {
            one_shift = 1;
            remainder = ShiftForward(remainder, one_shift);
            DeleteShiftBits(remainder);
            remainder.data[uint239_t::kSize - 1] |= (lhs_without_shift.data[i] >> j) & 1;
            if (remainder >= rhs_without_shift) {
                remainder = remainder - rhs_without_shift;
                result.data[i] |= (1 << j) ;
            } 
        }
    }
    long shift = GetShift(lhs) - GetShift(rhs);
    if (shift < 0) {
        shift = pow(uint239_t::kTwoBaseForTenDigit, uint239_t::kSize) + shift;
    }
    uint32_t shift_normal = static_cast<uint32_t>(shift);
    result = ShiftForward(result, shift_normal);
    return result;
}

bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_shifted_back = ShiftBack(lhs);
    uint239_t rhs_shifted_back = ShiftBack(rhs);
    for (int i = 0; i < uint239_t::kSize; ++i) {
        if (lhs_shifted_back.data[i] != rhs_shifted_back.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_shifted_back = ShiftBack(lhs);
    uint239_t rhs_shifted_back = ShiftBack(rhs);
    return !(lhs_shifted_back == rhs_shifted_back);
}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    uint239_t result = value;
    result = ShiftBack(result);
    for (int i = 0; i < uint239_t::kSize; ++i) {
        stream << static_cast<int>(result.data[i]);
    }
    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    uint64_t shift = 0;
    for (int i = uint239_t::kSize - 1; i >= 0; --i) {
        if ((((value.data[i] & uint239_t::kServiceBit) >> (uint239_t::kByte - 1))) == 1) {
            shift += pow(uint239_t::kTwoBaseForTenDigit, uint239_t::kSize - 1 - i);
        }
    }
    return shift;
}
