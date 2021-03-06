// Copyright 2015-present Facebook. All Rights Reserved.
// ** AUTOGENERATED FILE. DO NOT HAND-EDIT **

#pragma once

#include <string>

namespace proxygen { 

enum class TraceEventType {
    TotalRequest,
    RequestExchange,
    ResponseBodyRead,
    PreConnect,
    PostConnect,
    DnsResolution,
    DnsCache,
    RetryingDnsResolution,
    TcpConnect,
    TlsSetup,
    TotalConnect,
    Decompression,
    CertVerification,
    ProxyConnect,
    Push,
    Scheduling,
    NetworkChange,
    MultiConnector,
    SingleConnector,
    SessionTransactions,
    TCPInfo,
    ConnInfo,
    ZeroSetup,
    ZeroVerification,
    ZeroConnector,
    ReplaySafety,
    HTTPPerfParameters,
    RetryFilter,
    ZeroFallback,
    TLSCachedInfo,
    FBLigerProtocol,
    MQTTClient,
    MQTTMessage,
    MQTTConnect,
};

extern const std::string& getTraceEventTypeString(TraceEventType);
extern TraceEventType getTraceEventTypeFromString(const std::string&);
}

