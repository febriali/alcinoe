/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "pc/jseptransport.h"

#include <memory>
#include <utility>  // for std::pair

#include "api/candidate.h"
#include "p2p/base/p2pconstants.h"
#include "p2p/base/p2ptransportchannel.h"
#include "p2p/base/port.h"
#include "rtc_base/bind.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/strings/string_builder.h"

using webrtc::SdpType;

namespace cricket {

static bool VerifyIceParams(const JsepTransportDescription& jsep_description) {
  // For legacy protocols.
  // TODO(zhihuang): Remove this once the legacy protocol is no longer
  // supported.
  if (jsep_description.transport_desc.ice_ufrag.empty() &&
      jsep_description.transport_desc.ice_pwd.empty()) {
    return true;
  }

  if (jsep_description.transport_desc.ice_ufrag.length() <
          ICE_UFRAG_MIN_LENGTH ||
      jsep_description.transport_desc.ice_ufrag.length() >
          ICE_UFRAG_MAX_LENGTH) {
    return false;
  }
  if (jsep_description.transport_desc.ice_pwd.length() < ICE_PWD_MIN_LENGTH ||
      jsep_description.transport_desc.ice_pwd.length() > ICE_PWD_MAX_LENGTH) {
    return false;
  }
  return true;
}

JsepTransportDescription::JsepTransportDescription() {}

JsepTransportDescription::JsepTransportDescription(
    bool rtcp_mux_enabled,
    const std::vector<CryptoParams>& cryptos,
    const std::vector<int>& encrypted_header_extension_ids,
    int rtp_abs_sendtime_extn_id,
    const TransportDescription& transport_desc)
    : rtcp_mux_enabled(rtcp_mux_enabled),
      cryptos(cryptos),
      encrypted_header_extension_ids(encrypted_header_extension_ids),
      rtp_abs_sendtime_extn_id(rtp_abs_sendtime_extn_id),
      transport_desc(transport_desc) {}

JsepTransportDescription::JsepTransportDescription(
    const JsepTransportDescription& from)
    : rtcp_mux_enabled(from.rtcp_mux_enabled),
      cryptos(from.cryptos),
      encrypted_header_extension_ids(from.encrypted_header_extension_ids),
      rtp_abs_sendtime_extn_id(from.rtp_abs_sendtime_extn_id),
      transport_desc(from.transport_desc) {}

JsepTransportDescription::~JsepTransportDescription() = default;

JsepTransportDescription& JsepTransportDescription::operator=(
    const JsepTransportDescription& from) {
  if (this == &from) {
    return *this;
  }
  rtcp_mux_enabled = from.rtcp_mux_enabled;
  cryptos = from.cryptos;
  encrypted_header_extension_ids = from.encrypted_header_extension_ids;
  rtp_abs_sendtime_extn_id = from.rtp_abs_sendtime_extn_id;
  transport_desc = from.transport_desc;

  return *this;
}

JsepTransport::JsepTransport(
    const std::string& mid,
    const rtc::scoped_refptr<rtc::RTCCertificate>& local_certificate,
    std::unique_ptr<webrtc::RtpTransport> unencrypted_rtp_transport,
    std::unique_ptr<webrtc::SrtpTransport> sdes_transport,
    std::unique_ptr<webrtc::DtlsSrtpTransport> dtls_srtp_transport,
    std::unique_ptr<DtlsTransportInternal> rtp_dtls_transport,
    std::unique_ptr<DtlsTransportInternal> rtcp_dtls_transport,
    std::unique_ptr<webrtc::MediaTransportInterface> media_transport)
    : mid_(mid),
      local_certificate_(local_certificate),
      rtp_dtls_transport_(std::move(rtp_dtls_transport)),
      rtcp_dtls_transport_(std::move(rtcp_dtls_transport)),
      media_transport_(std::move(media_transport)) {
  RTC_DCHECK(rtp_dtls_transport_);
  if (unencrypted_rtp_transport) {
    RTC_DCHECK(!sdes_transport);
    RTC_DCHECK(!dtls_srtp_transport);
    unencrypted_rtp_transport_ = std::move(unencrypted_rtp_transport);
  } else if (sdes_transport) {
    RTC_DCHECK(!unencrypted_rtp_transport);
    RTC_DCHECK(!dtls_srtp_transport);
    sdes_transport_ = std::move(sdes_transport);
  } else {
    RTC_DCHECK(dtls_srtp_transport);
    RTC_DCHECK(!unencrypted_rtp_transport);
    RTC_DCHECK(!sdes_transport);
    dtls_srtp_transport_ = std::move(dtls_srtp_transport);
  }
}

JsepTransport::~JsepTransport() {}

webrtc::RTCError JsepTransport::SetLocalJsepTransportDescription(
    const JsepTransportDescription& jsep_description,
    SdpType type) {
  webrtc::RTCError error;

  if (!VerifyIceParams(jsep_description)) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Invalid ice-ufrag or ice-pwd length.");
  }

  if (!SetRtcpMux(jsep_description.rtcp_mux_enabled, type,
                  ContentSource::CS_LOCAL)) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Failed to setup RTCP mux.");
  }

  // If doing SDES, setup the SDES crypto parameters.
  if (sdes_transport_) {
    RTC_DCHECK(!unencrypted_rtp_transport_);
    RTC_DCHECK(!dtls_srtp_transport_);
    if (!SetSdes(jsep_description.cryptos,
                 jsep_description.encrypted_header_extension_ids, type,
                 ContentSource::CS_LOCAL)) {
      return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                              "Failed to setup SDES crypto parameters.");
    }
  } else if (dtls_srtp_transport_) {
    RTC_DCHECK(!unencrypted_rtp_transport_);
    RTC_DCHECK(!sdes_transport_);
    dtls_srtp_transport_->UpdateRecvEncryptedHeaderExtensionIds(
        jsep_description.encrypted_header_extension_ids);
  }

  bool ice_restarting =
      local_description_ != nullptr &&
      IceCredentialsChanged(local_description_->transport_desc.ice_ufrag,
                            local_description_->transport_desc.ice_pwd,
                            jsep_description.transport_desc.ice_ufrag,
                            jsep_description.transport_desc.ice_pwd);
  local_description_.reset(new JsepTransportDescription(jsep_description));

  rtc::SSLFingerprint* local_fp =
      local_description_->transport_desc.identity_fingerprint.get();

  if (!local_fp) {
    local_certificate_ = nullptr;
  } else {
    error = VerifyCertificateFingerprint(local_certificate_.get(), local_fp);
    if (!error.ok()) {
      local_description_.reset();
      return error;
    }
  }

  SetLocalIceParameters(rtp_dtls_transport_->ice_transport());

  if (rtcp_dtls_transport_) {
    SetLocalIceParameters(rtcp_dtls_transport_->ice_transport());
  }

  // If PRANSWER/ANSWER is set, we should decide transport protocol type.
  if (type == SdpType::kPrAnswer || type == SdpType::kAnswer) {
    error = NegotiateAndSetDtlsParameters(type);
  }
  if (!error.ok()) {
    local_description_.reset();
    return error;
  }

  if (needs_ice_restart_ && ice_restarting) {
    needs_ice_restart_ = false;
    RTC_LOG(LS_VERBOSE) << "needs-ice-restart flag cleared for transport "
                        << mid();
  }

  return webrtc::RTCError::OK();
}

webrtc::RTCError JsepTransport::SetRemoteJsepTransportDescription(
    const JsepTransportDescription& jsep_description,
    webrtc::SdpType type) {
  webrtc::RTCError error;

  if (!VerifyIceParams(jsep_description)) {
    remote_description_.reset();
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Invalid ice-ufrag or ice-pwd length.");
  }

  if (!SetRtcpMux(jsep_description.rtcp_mux_enabled, type,
                  ContentSource::CS_REMOTE)) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Failed to setup RTCP mux.");
  }

  // If doing SDES, setup the SDES crypto parameters.
  if (sdes_transport_) {
    RTC_DCHECK(!unencrypted_rtp_transport_);
    RTC_DCHECK(!dtls_srtp_transport_);
    if (!SetSdes(jsep_description.cryptos,
                 jsep_description.encrypted_header_extension_ids, type,
                 ContentSource::CS_REMOTE)) {
      return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                              "Failed to setup SDES crypto parameters.");
    }
    sdes_transport_->CacheRtpAbsSendTimeHeaderExtension(
        jsep_description.rtp_abs_sendtime_extn_id);
  } else if (dtls_srtp_transport_) {
    RTC_DCHECK(!unencrypted_rtp_transport_);
    RTC_DCHECK(!sdes_transport_);
    dtls_srtp_transport_->UpdateSendEncryptedHeaderExtensionIds(
        jsep_description.encrypted_header_extension_ids);
    dtls_srtp_transport_->CacheRtpAbsSendTimeHeaderExtension(
        jsep_description.rtp_abs_sendtime_extn_id);
  }

  remote_description_.reset(new JsepTransportDescription(jsep_description));
  SetRemoteIceParameters(rtp_dtls_transport_->ice_transport());

  if (rtcp_dtls_transport_) {
    SetRemoteIceParameters(rtcp_dtls_transport_->ice_transport());
  }

  // If PRANSWER/ANSWER is set, we should decide transport protocol type.
  if (type == SdpType::kPrAnswer || type == SdpType::kAnswer) {
    error = NegotiateAndSetDtlsParameters(SdpType::kOffer);
  }
  if (!error.ok()) {
    remote_description_.reset();
    return error;
  }
  return webrtc::RTCError::OK();
}

webrtc::RTCError JsepTransport::AddRemoteCandidates(
    const Candidates& candidates) {
  if (!local_description_ || !remote_description_) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_STATE,
                            mid() +
                                " is not ready to use the remote candidate "
                                "because the local or remote description is "
                                "not set.");
  }

  for (const cricket::Candidate& candidate : candidates) {
    auto transport =
        candidate.component() == cricket::ICE_CANDIDATE_COMPONENT_RTP
            ? rtp_dtls_transport_.get()
            : rtcp_dtls_transport_.get();
    if (!transport) {
      return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                              "Candidate has an unknown component: " +
                                  candidate.ToString() + " for mid " + mid());
    }
    transport->ice_transport()->AddRemoteCandidate(candidate);
  }
  return webrtc::RTCError::OK();
}

void JsepTransport::SetNeedsIceRestartFlag() {
  if (!needs_ice_restart_) {
    needs_ice_restart_ = true;
    RTC_LOG(LS_VERBOSE) << "needs-ice-restart flag set for transport " << mid();
  }
}

absl::optional<rtc::SSLRole> JsepTransport::GetDtlsRole() const {
  RTC_DCHECK(rtp_dtls_transport_);
  rtc::SSLRole dtls_role;
  if (!rtp_dtls_transport_->GetDtlsRole(&dtls_role)) {
    return absl::optional<rtc::SSLRole>();
  }

  return absl::optional<rtc::SSLRole>(dtls_role);
}

bool JsepTransport::GetStats(TransportStats* stats) {
  stats->transport_name = mid();
  stats->channel_stats.clear();
  bool ret = GetTransportStats(rtp_dtls_transport_.get(), stats);
  if (rtcp_dtls_transport_) {
    ret &= GetTransportStats(rtcp_dtls_transport_.get(), stats);
  }
  return ret;
}

webrtc::RTCError JsepTransport::VerifyCertificateFingerprint(
    const rtc::RTCCertificate* certificate,
    const rtc::SSLFingerprint* fingerprint) const {
  if (!fingerprint) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "No fingerprint");
  }
  if (!certificate) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Fingerprint provided but no identity available.");
  }
  std::unique_ptr<rtc::SSLFingerprint> fp_tmp =
      rtc::SSLFingerprint::CreateUnique(fingerprint->algorithm,
                                        *certificate->identity());
  RTC_DCHECK(fp_tmp.get() != NULL);
  if (*fp_tmp == *fingerprint) {
    return webrtc::RTCError::OK();
  }
  char ss_buf[1024];
  rtc::SimpleStringBuilder desc(ss_buf);
  desc << "Local fingerprint does not match identity. Expected: ";
  desc << fp_tmp->ToString();
  desc << " Got: " << fingerprint->ToString();
  return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                          std::string(desc.str()));
}

void JsepTransport::SetActiveResetSrtpParams(bool active_reset_srtp_params) {
  if (dtls_srtp_transport_) {
    RTC_LOG(INFO)
        << "Setting active_reset_srtp_params of DtlsSrtpTransport to: "
        << active_reset_srtp_params;
    dtls_srtp_transport_->SetActiveResetSrtpParams(active_reset_srtp_params);
  }
}

void JsepTransport::SetLocalIceParameters(IceTransportInternal* ice_transport) {
  RTC_DCHECK(ice_transport);
  RTC_DCHECK(local_description_);
  ice_transport->SetIceParameters(
      local_description_->transport_desc.GetIceParameters());
}

void JsepTransport::SetRemoteIceParameters(
    IceTransportInternal* ice_transport) {
  RTC_DCHECK(ice_transport);
  RTC_DCHECK(remote_description_);
  ice_transport->SetRemoteIceParameters(
      remote_description_->transport_desc.GetIceParameters());
  ice_transport->SetRemoteIceMode(remote_description_->transport_desc.ice_mode);
}

webrtc::RTCError JsepTransport::SetNegotiatedDtlsParameters(
    DtlsTransportInternal* dtls_transport,
    absl::optional<rtc::SSLRole> dtls_role,
    rtc::SSLFingerprint* remote_fingerprint) {
  RTC_DCHECK(dtls_transport);
  // Set SSL role. Role must be set before fingerprint is applied, which
  // initiates DTLS setup.
  if (dtls_role && !dtls_transport->SetDtlsRole(*dtls_role)) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Failed to set SSL role for the transport.");
  }
  // Apply remote fingerprint.
  if (!remote_fingerprint ||
      !dtls_transport->SetRemoteFingerprint(
          remote_fingerprint->algorithm,
          reinterpret_cast<const uint8_t*>(remote_fingerprint->digest.data()),
          remote_fingerprint->digest.size())) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_PARAMETER,
                            "Failed to apply remote fingerprint.");
  }
  return webrtc::RTCError::OK();
}

bool JsepTransport::SetRtcpMux(bool enable,
                               webrtc::SdpType type,
                               ContentSource source) {
  bool ret = false;
  switch (type) {
    case SdpType::kOffer:
      ret = rtcp_mux_negotiator_.SetOffer(enable, source);
      break;
    case SdpType::kPrAnswer:
      // This may activate RTCP muxing, but we don't yet destroy the transport
      // because the final answer may deactivate it.
      ret = rtcp_mux_negotiator_.SetProvisionalAnswer(enable, source);
      break;
    case SdpType::kAnswer:
      ret = rtcp_mux_negotiator_.SetAnswer(enable, source);
      if (ret && rtcp_mux_negotiator_.IsActive()) {
        ActivateRtcpMux();
      }
      break;
    default:
      RTC_NOTREACHED();
  }

  if (!ret) {
    return false;
  }

  auto transport = rtp_transport();
  transport->SetRtcpMuxEnabled(rtcp_mux_negotiator_.IsActive());
  return ret;
}

void JsepTransport::ActivateRtcpMux() {
  if (unencrypted_rtp_transport_) {
    RTC_DCHECK(!sdes_transport_);
    RTC_DCHECK(!dtls_srtp_transport_);
    unencrypted_rtp_transport_->SetRtcpPacketTransport(nullptr);
  } else if (sdes_transport_) {
    RTC_DCHECK(!unencrypted_rtp_transport_);
    RTC_DCHECK(!dtls_srtp_transport_);
    sdes_transport_->SetRtcpPacketTransport(nullptr);
  } else {
    RTC_DCHECK(dtls_srtp_transport_);
    RTC_DCHECK(!unencrypted_rtp_transport_);
    RTC_DCHECK(!sdes_transport_);
    dtls_srtp_transport_->SetDtlsTransports(rtp_dtls_transport(),
                                            /*rtcp_dtls_transport=*/nullptr);
  }
  rtcp_dtls_transport_.reset();
  // Notify the JsepTransportController to update the aggregate states.
  SignalRtcpMuxActive();
}

bool JsepTransport::SetSdes(const std::vector<CryptoParams>& cryptos,
                            const std::vector<int>& encrypted_extension_ids,
                            webrtc::SdpType type,
                            ContentSource source) {
  bool ret = false;
  ret = sdes_negotiator_.Process(cryptos, type, source);
  if (!ret) {
    return ret;
  }

  if (source == ContentSource::CS_LOCAL) {
    recv_extension_ids_ = std::move(encrypted_extension_ids);
  } else {
    send_extension_ids_ = std::move(encrypted_extension_ids);
  }

  // If setting an SDES answer succeeded, apply the negotiated parameters
  // to the SRTP transport.
  if ((type == SdpType::kPrAnswer || type == SdpType::kAnswer) && ret) {
    if (sdes_negotiator_.send_cipher_suite() &&
        sdes_negotiator_.recv_cipher_suite()) {
      RTC_DCHECK(send_extension_ids_);
      RTC_DCHECK(recv_extension_ids_);
      ret = sdes_transport_->SetRtpParams(
          *(sdes_negotiator_.send_cipher_suite()),
          sdes_negotiator_.send_key().data(),
          static_cast<int>(sdes_negotiator_.send_key().size()),
          *(send_extension_ids_), *(sdes_negotiator_.recv_cipher_suite()),
          sdes_negotiator_.recv_key().data(),
          static_cast<int>(sdes_negotiator_.recv_key().size()),
          *(recv_extension_ids_));
    } else {
      RTC_LOG(LS_INFO) << "No crypto keys are provided for SDES.";
      if (type == SdpType::kAnswer) {
        // Explicitly reset the |sdes_transport_| if no crypto param is
        // provided in the answer. No need to call |ResetParams()| for
        // |sdes_negotiator_| because it resets the params inside |SetAnswer|.
        sdes_transport_->ResetParams();
      }
    }
  }
  return ret;
}

webrtc::RTCError JsepTransport::NegotiateAndSetDtlsParameters(
    SdpType local_description_type) {
  if (!local_description_ || !remote_description_) {
    return webrtc::RTCError(webrtc::RTCErrorType::INVALID_STATE,
                            "Applying an answer transport description "
                            "without applying any offer.");
  }
  std::unique_ptr<rtc::SSLFingerprint> remote_fingerprint;
  absl::optional<rtc::SSLRole> negotiated_dtls_role;

  rtc::SSLFingerprint* local_fp =
      local_description_->transport_desc.identity_fingerprint.get();
  rtc::SSLFingerprint* remote_fp =
      remote_description_->transport_desc.identity_fingerprint.get();
  if (remote_fp && local_fp) {
    remote_fingerprint = absl::make_unique<rtc::SSLFingerprint>(*remote_fp);
    webrtc::RTCError error =
        NegotiateDtlsRole(local_description_type,
                          local_description_->transport_desc.connection_role,
                          remote_description_->transport_desc.connection_role,
                          &negotiated_dtls_role);
    if (!error.ok()) {
      return error;
    }
  } else if (local_fp && (local_description_type == SdpType::kAnswer)) {
    return webrtc::RTCError(
        webrtc::RTCErrorType::INVALID_PARAMETER,
        "Local fingerprint supplied when caller didn't offer DTLS.");
  } else {
    // We are not doing DTLS
    remote_fingerprint = absl::make_unique<rtc::SSLFingerprint>(
        "", rtc::ArrayView<const uint8_t>());
  }
  // Now that we have negotiated everything, push it downward.
  // Note that we cache the result so that if we have race conditions
  // between future SetRemote/SetLocal invocations and new transport
  // creation, we have the negotiation state saved until a new
  // negotiation happens.
  webrtc::RTCError error = SetNegotiatedDtlsParameters(
      rtp_dtls_transport_.get(), negotiated_dtls_role,
      remote_fingerprint.get());
  if (!error.ok()) {
    return error;
  }

  if (rtcp_dtls_transport_) {
    error = SetNegotiatedDtlsParameters(rtcp_dtls_transport_.get(),
                                        negotiated_dtls_role,
                                        remote_fingerprint.get());
  }
  return error;
}

webrtc::RTCError JsepTransport::NegotiateDtlsRole(
    SdpType local_description_type,
    ConnectionRole local_connection_role,
    ConnectionRole remote_connection_role,
    absl::optional<rtc::SSLRole>* negotiated_dtls_role) {
  // From RFC 4145, section-4.1, The following are the values that the
  // 'setup' attribute can take in an offer/answer exchange:
  //       Offer      Answer
  //      ________________
  //      active     passive / holdconn
  //      passive    active / holdconn
  //      actpass    active / passive / holdconn
  //      holdconn   holdconn
  //
  // Set the role that is most conformant with RFC 5763, Section 5, bullet 1
  // The endpoint MUST use the setup attribute defined in [RFC4145].
  // The endpoint that is the offerer MUST use the setup attribute
  // value of setup:actpass and be prepared to receive a client_hello
  // before it receives the answer.  The answerer MUST use either a
  // setup attribute value of setup:active or setup:passive.  Note that
  // if the answerer uses setup:passive, then the DTLS handshake will
  // not begin until the answerer is received, which adds additional
  // latency. setup:active allows the answer and the DTLS handshake to
  // occur in parallel.  Thus, setup:active is RECOMMENDED.  Whichever
  // party is active MUST initiate a DTLS handshake by sending a
  // ClientHello over each flow (host/port quartet).
  // IOW - actpass and passive modes should be treated as server and
  // active as client.
  bool is_remote_server = false;
  if (local_description_type == SdpType::kOffer) {
    if (local_connection_role != CONNECTIONROLE_ACTPASS) {
      return webrtc::RTCError(
          webrtc::RTCErrorType::INVALID_PARAMETER,
          "Offerer must use actpass value for setup attribute.");
    }

    if (remote_connection_role == CONNECTIONROLE_ACTIVE ||
        remote_connection_role == CONNECTIONROLE_PASSIVE ||
        remote_connection_role == CONNECTIONROLE_NONE) {
      is_remote_server = (remote_connection_role == CONNECTIONROLE_PASSIVE);
    } else {
      return webrtc::RTCError(
          webrtc::RTCErrorType::INVALID_PARAMETER,
          "Answerer must use either active or passive value "
          "for setup attribute.");
    }
    // If remote is NONE or ACTIVE it will act as client.
  } else {
    if (remote_connection_role != CONNECTIONROLE_ACTPASS &&
        remote_connection_role != CONNECTIONROLE_NONE) {
      // Accept a remote role attribute that's not "actpass", but matches the
      // current negotiated role. This is allowed by dtls-sdp, though our
      // implementation will never generate such an offer as it's not
      // recommended.
      //
      // See https://datatracker.ietf.org/doc/html/draft-ietf-mmusic-dtls-sdp,
      // section 5.5.
      auto current_dtls_role = GetDtlsRole();
      if (!current_dtls_role ||
          (*current_dtls_role == rtc::SSL_CLIENT &&
           remote_connection_role == CONNECTIONROLE_ACTIVE) ||
          (*current_dtls_role == rtc::SSL_SERVER &&
           remote_connection_role == CONNECTIONROLE_PASSIVE)) {
        return webrtc::RTCError(
            webrtc::RTCErrorType::INVALID_PARAMETER,
            "Offerer must use actpass value or current negotiated role for "
            "setup attribute.");
      }
    }

    if (local_connection_role == CONNECTIONROLE_ACTIVE ||
        local_connection_role == CONNECTIONROLE_PASSIVE) {
      is_remote_server = (local_connection_role == CONNECTIONROLE_ACTIVE);
    } else {
      return webrtc::RTCError(
          webrtc::RTCErrorType::INVALID_PARAMETER,
          "Answerer must use either active or passive value "
          "for setup attribute.");
    }

    // If local is passive, local will act as server.
  }

  *negotiated_dtls_role = (is_remote_server ? std::move(rtc::SSL_CLIENT)
                                            : std::move(rtc::SSL_SERVER));
  return webrtc::RTCError::OK();
}

bool JsepTransport::GetTransportStats(DtlsTransportInternal* dtls_transport,
                                      TransportStats* stats) {
  RTC_DCHECK(dtls_transport);
  TransportChannelStats substats;
  substats.component = dtls_transport == rtcp_dtls_transport_.get()
                           ? ICE_CANDIDATE_COMPONENT_RTCP
                           : ICE_CANDIDATE_COMPONENT_RTP;
  dtls_transport->GetSrtpCryptoSuite(&substats.srtp_crypto_suite);
  dtls_transport->GetSslCipherSuite(&substats.ssl_cipher_suite);
  substats.dtls_state = dtls_transport->dtls_state();
  if (!dtls_transport->ice_transport()->GetStats(
          &substats.connection_infos, &substats.candidate_stats_list)) {
    return false;
  }
  stats->channel_stats.push_back(substats);
  return true;
}

}  // namespace cricket
