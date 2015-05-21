// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: phonenumber.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "phonenumber.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)

namespace i18n {
namespace phonenumbers {

void protobuf_ShutdownFile_phonenumber_2eproto() {
  delete PhoneNumber::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_phonenumber_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_phonenumber_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  PhoneNumber::default_instance_ = new PhoneNumber();
  PhoneNumber::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_phonenumber_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_phonenumber_2eproto_once_);
void protobuf_AddDesc_phonenumber_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_phonenumber_2eproto_once_,
                 &protobuf_AddDesc_phonenumber_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_phonenumber_2eproto {
  StaticDescriptorInitializer_phonenumber_2eproto() {
    protobuf_AddDesc_phonenumber_2eproto();
  }
} static_descriptor_initializer_phonenumber_2eproto_;
#endif

// ===================================================================

bool PhoneNumber_CountryCodeSource_IsValid(int value) {
  switch(value) {
    case 1:
    case 5:
    case 10:
    case 20:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const PhoneNumber_CountryCodeSource PhoneNumber::FROM_NUMBER_WITH_PLUS_SIGN;
const PhoneNumber_CountryCodeSource PhoneNumber::FROM_NUMBER_WITH_IDD;
const PhoneNumber_CountryCodeSource PhoneNumber::FROM_NUMBER_WITHOUT_PLUS_SIGN;
const PhoneNumber_CountryCodeSource PhoneNumber::FROM_DEFAULT_COUNTRY;
const PhoneNumber_CountryCodeSource PhoneNumber::CountryCodeSource_MIN;
const PhoneNumber_CountryCodeSource PhoneNumber::CountryCodeSource_MAX;
const int PhoneNumber::CountryCodeSource_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int PhoneNumber::kCountryCodeFieldNumber;
const int PhoneNumber::kNationalNumberFieldNumber;
const int PhoneNumber::kExtensionFieldNumber;
const int PhoneNumber::kItalianLeadingZeroFieldNumber;
const int PhoneNumber::kNumberOfLeadingZerosFieldNumber;
const int PhoneNumber::kRawInputFieldNumber;
const int PhoneNumber::kCountryCodeSourceFieldNumber;
const int PhoneNumber::kPreferredDomesticCarrierCodeFieldNumber;
#endif  // !_MSC_VER

PhoneNumber::PhoneNumber()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:i18n.phonenumbers.PhoneNumber)
}

void PhoneNumber::InitAsDefaultInstance() {
}

PhoneNumber::PhoneNumber(const PhoneNumber& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:i18n.phonenumbers.PhoneNumber)
}

void PhoneNumber::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  country_code_ = 0;
  national_number_ = GOOGLE_ULONGLONG(0);
  extension_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  italian_leading_zero_ = false;
  number_of_leading_zeros_ = 1;
  raw_input_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  country_code_source_ = 1;
  preferred_domestic_carrier_code_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

PhoneNumber::~PhoneNumber() {
  // @@protoc_insertion_point(destructor:i18n.phonenumbers.PhoneNumber)
  SharedDtor();
}

void PhoneNumber::SharedDtor() {
  if (extension_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete extension_;
  }
  if (raw_input_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete raw_input_;
  }
  if (preferred_domestic_carrier_code_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete preferred_domestic_carrier_code_;
  }
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void PhoneNumber::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const PhoneNumber& PhoneNumber::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_phonenumber_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_phonenumber_2eproto();
#endif
  return *default_instance_;
}

PhoneNumber* PhoneNumber::default_instance_ = NULL;

PhoneNumber* PhoneNumber::New() const {
  return new PhoneNumber;
}

void PhoneNumber::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<PhoneNumber*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 255) {
    ZR_(national_number_, italian_leading_zero_);
    if (has_extension()) {
      if (extension_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        extension_->clear();
      }
    }
    number_of_leading_zeros_ = 1;
    if (has_raw_input()) {
      if (raw_input_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        raw_input_->clear();
      }
    }
    country_code_source_ = 1;
    if (has_preferred_domestic_carrier_code()) {
      if (preferred_domestic_carrier_code_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        preferred_domestic_carrier_code_->clear();
      }
    }
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->clear();
}

bool PhoneNumber::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  ::google::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::google::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:i18n.phonenumbers.PhoneNumber)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 country_code = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &country_code_)));
          set_has_country_code();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_national_number;
        break;
      }

      // required uint64 national_number = 2;
      case 2: {
        if (tag == 16) {
         parse_national_number:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &national_number_)));
          set_has_national_number();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_extension;
        break;
      }

      // optional string extension = 3;
      case 3: {
        if (tag == 26) {
         parse_extension:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_extension()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_italian_leading_zero;
        break;
      }

      // optional bool italian_leading_zero = 4;
      case 4: {
        if (tag == 32) {
         parse_italian_leading_zero:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &italian_leading_zero_)));
          set_has_italian_leading_zero();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(42)) goto parse_raw_input;
        break;
      }

      // optional string raw_input = 5;
      case 5: {
        if (tag == 42) {
         parse_raw_input:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_raw_input()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(48)) goto parse_country_code_source;
        break;
      }

      // optional .i18n.phonenumbers.PhoneNumber.CountryCodeSource country_code_source = 6;
      case 6: {
        if (tag == 48) {
         parse_country_code_source:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::i18n::phonenumbers::PhoneNumber_CountryCodeSource_IsValid(value)) {
            set_country_code_source(static_cast< ::i18n::phonenumbers::PhoneNumber_CountryCodeSource >(value));
          } else {
            unknown_fields_stream.WriteVarint32(tag);
            unknown_fields_stream.WriteVarint32(value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(58)) goto parse_preferred_domestic_carrier_code;
        break;
      }

      // optional string preferred_domestic_carrier_code = 7;
      case 7: {
        if (tag == 58) {
         parse_preferred_domestic_carrier_code:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_preferred_domestic_carrier_code()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(64)) goto parse_number_of_leading_zeros;
        break;
      }

      // optional int32 number_of_leading_zeros = 8 [default = 1];
      case 8: {
        if (tag == 64) {
         parse_number_of_leading_zeros:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &number_of_leading_zeros_)));
          set_has_number_of_leading_zeros();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:i18n.phonenumbers.PhoneNumber)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:i18n.phonenumbers.PhoneNumber)
  return false;
#undef DO_
}

void PhoneNumber::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:i18n.phonenumbers.PhoneNumber)
  // required int32 country_code = 1;
  if (has_country_code()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->country_code(), output);
  }

  // required uint64 national_number = 2;
  if (has_national_number()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(2, this->national_number(), output);
  }

  // optional string extension = 3;
  if (has_extension()) {
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->extension(), output);
  }

  // optional bool italian_leading_zero = 4;
  if (has_italian_leading_zero()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(4, this->italian_leading_zero(), output);
  }

  // optional string raw_input = 5;
  if (has_raw_input()) {
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      5, this->raw_input(), output);
  }

  // optional .i18n.phonenumbers.PhoneNumber.CountryCodeSource country_code_source = 6;
  if (has_country_code_source()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      6, this->country_code_source(), output);
  }

  // optional string preferred_domestic_carrier_code = 7;
  if (has_preferred_domestic_carrier_code()) {
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      7, this->preferred_domestic_carrier_code(), output);
  }

  // optional int32 number_of_leading_zeros = 8 [default = 1];
  if (has_number_of_leading_zeros()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(8, this->number_of_leading_zeros(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:i18n.phonenumbers.PhoneNumber)
}

int PhoneNumber::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 country_code = 1;
    if (has_country_code()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->country_code());
    }

    // required uint64 national_number = 2;
    if (has_national_number()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->national_number());
    }

    // optional string extension = 3;
    if (has_extension()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->extension());
    }

    // optional bool italian_leading_zero = 4;
    if (has_italian_leading_zero()) {
      total_size += 1 + 1;
    }

    // optional int32 number_of_leading_zeros = 8 [default = 1];
    if (has_number_of_leading_zeros()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->number_of_leading_zeros());
    }

    // optional string raw_input = 5;
    if (has_raw_input()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->raw_input());
    }

    // optional .i18n.phonenumbers.PhoneNumber.CountryCodeSource country_code_source = 6;
    if (has_country_code_source()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->country_code_source());
    }

    // optional string preferred_domestic_carrier_code = 7;
    if (has_preferred_domestic_carrier_code()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->preferred_domestic_carrier_code());
    }

  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void PhoneNumber::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const PhoneNumber*>(&from));
}

void PhoneNumber::MergeFrom(const PhoneNumber& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_country_code()) {
      set_country_code(from.country_code());
    }
    if (from.has_national_number()) {
      set_national_number(from.national_number());
    }
    if (from.has_extension()) {
      set_extension(from.extension());
    }
    if (from.has_italian_leading_zero()) {
      set_italian_leading_zero(from.italian_leading_zero());
    }
    if (from.has_number_of_leading_zeros()) {
      set_number_of_leading_zeros(from.number_of_leading_zeros());
    }
    if (from.has_raw_input()) {
      set_raw_input(from.raw_input());
    }
    if (from.has_country_code_source()) {
      set_country_code_source(from.country_code_source());
    }
    if (from.has_preferred_domestic_carrier_code()) {
      set_preferred_domestic_carrier_code(from.preferred_domestic_carrier_code());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void PhoneNumber::CopyFrom(const PhoneNumber& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PhoneNumber::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void PhoneNumber::Swap(PhoneNumber* other) {
  if (other != this) {
    std::swap(country_code_, other->country_code_);
    std::swap(national_number_, other->national_number_);
    std::swap(extension_, other->extension_);
    std::swap(italian_leading_zero_, other->italian_leading_zero_);
    std::swap(number_of_leading_zeros_, other->number_of_leading_zeros_);
    std::swap(raw_input_, other->raw_input_);
    std::swap(country_code_source_, other->country_code_source_);
    std::swap(preferred_domestic_carrier_code_, other->preferred_domestic_carrier_code_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.swap(other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string PhoneNumber::GetTypeName() const {
  return "i18n.phonenumbers.PhoneNumber";
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace phonenumbers
}  // namespace i18n

// @@protoc_insertion_point(global_scope)