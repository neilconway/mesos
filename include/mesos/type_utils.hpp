// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __MESOS_TYPE_UTILS_H__
#define __MESOS_TYPE_UTILS_H__

#include <iosfwd>
#include <string>
#include <vector>

#include <boost/functional/hash.hpp>

#include <google/protobuf/repeated_field.h>

#include <mesos/mesos.hpp>

#include <stout/hashmap.hpp>
#include <stout/stringify.hpp>
#include <stout/strings.hpp>
#include <stout/uuid.hpp>

// This file includes definitions for operators on public protobuf
// classes (defined in mesos.proto, module.proto, etc.) that don't
// have these operators generated by the protobuf compiler. The
// corresponding definitions are in src/common/type_utils.cpp.
//
// Mesos modules need some of the protobuf classes defined in
// mesos.proto, module.proto, etc., and require some of these
// operators declared in type_utils.hpp. Exposing type_utils.hpp
// allows us to build modules without having a dependency on mesos
// source tree (src/*).

namespace mesos {

bool operator==(const CheckStatusInfo& left, const CheckStatusInfo& right);
bool operator==(const CommandInfo& left, const CommandInfo& right);
bool operator==(const CommandInfo::URI& left, const CommandInfo::URI& right);
bool operator==(const ContainerID& left, const ContainerID& right);
bool operator==(const Credential& left, const Credential& right);
bool operator==(const DiscoveryInfo& left, const DiscoveryInfo& right);
bool operator==(const Environment& left, const Environment& right);
bool operator==(const ExecutorInfo& left, const ExecutorInfo& right);
bool operator==(const Label& left, const Label& right);
bool operator==(const Labels& left, const Labels& right);
bool operator==(const MasterInfo& left, const MasterInfo& right);

bool operator==(
    const ResourceProviderInfo& left,
    const ResourceProviderInfo& right);

bool operator==(
    const ResourceStatistics& left,
    const ResourceStatistics& right);

bool operator==(const SlaveInfo& left, const SlaveInfo& right);
bool operator==(const Task& left, const Task& right);
bool operator==(const TaskGroupInfo& left, const TaskGroupInfo& right);
bool operator==(const TaskInfo& left, const TaskInfo& right);
bool operator==(const TaskStatus& left, const TaskStatus& right);
bool operator==(const URL& left, const URL& right);
bool operator==(const Volume& left, const Volume& right);

bool operator!=(const CheckStatusInfo& left, const CheckStatusInfo& right);
bool operator!=(const ExecutorInfo& left, const ExecutorInfo& right);
bool operator!=(const Labels& left, const Labels& right);
bool operator!=(const TaskStatus& left, const TaskStatus& right);


bool operator!=(
    const ResourceProviderInfo& left,
    const ResourceProviderInfo& right);

inline bool operator==(const ExecutorID& left, const ExecutorID& right)
{
  return left.value() == right.value();
}


inline bool operator==(const FrameworkID& left, const FrameworkID& right)
{
  return left.value() == right.value();
}


inline bool operator==(const FrameworkInfo& left, const FrameworkInfo& right)
{
  return (left.name() == right.name()) && (left.user() == right.user());
}


inline bool operator==(const OfferID& left, const OfferID& right)
{
  return left.value() == right.value();
}


inline bool operator==(
    const ResourceProviderID& left,
    const ResourceProviderID& right)
{
  return left.value() == right.value();
}


inline bool operator==(const SlaveID& left, const SlaveID& right)
{
  return left.value() == right.value();
}


inline bool operator==(const TaskID& left, const TaskID& right)
{
  return left.value() == right.value();
}


inline bool operator==(const TimeInfo& left, const TimeInfo& right)
{
  return left.nanoseconds() == right.nanoseconds();
}


inline bool operator==(const DurationInfo& left, const DurationInfo& right)
{
  return left.nanoseconds() == right.nanoseconds();
}


inline bool operator==(const ContainerID& left, const std::string& right)
{
  return left.value() == right;
}


inline bool operator==(const ExecutorID& left, const std::string& right)
{
  return left.value() == right;
}


inline bool operator==(const FrameworkID& left, const std::string& right)
{
  return left.value() == right;
}


inline bool operator==(const OfferID& left, const std::string& right)
{
  return left.value() == right;
}


inline bool operator==(const SlaveID& left, const std::string& right)
{
  return left.value() == right;
}


inline bool operator==(const TaskID& left, const std::string& right)
{
  return left.value() == right;
}


inline bool operator==(
    const DomainInfo::FaultDomain::RegionInfo& left,
    const DomainInfo::FaultDomain::RegionInfo& right)
{
  return left.name() == right.name();
}


inline bool operator==(
    const DomainInfo::FaultDomain::ZoneInfo& left,
    const DomainInfo::FaultDomain::ZoneInfo& right)
{
  return left.name() == right.name();
}


inline bool operator==(
    const DomainInfo::FaultDomain& left,
    const DomainInfo::FaultDomain& right)
{
  return left.region() == right.region() && left.zone() == right.zone();
}


inline bool operator==(const DomainInfo& left, const DomainInfo& right)
{
  return left.fault_domain() == right.fault_domain();
}


/**
 * For machines to match, both the `hostname` and `ip` must be equivalent.
 * Hostname is not case sensitive, so it is lowercased before comparison.
 */
inline bool operator==(const MachineID& left, const MachineID& right)
{
  // NOTE: Both fields default to the empty string if they are not specified,
  // so the string comparisons are safe.
  return left.has_hostname() == right.has_hostname() &&
    strings::lower(left.hostname()) == strings::lower(right.hostname()) &&
    left.has_ip() == right.has_ip() &&
    left.ip() == right.ip();
}


inline bool operator!=(const ContainerID& left, const ContainerID& right)
{
  return !(left == right);
}


inline bool operator!=(const ExecutorID& left, const ExecutorID& right)
{
  return left.value() != right.value();
}


inline bool operator!=(const FrameworkID& left, const FrameworkID& right)
{
  return left.value() != right.value();
}


inline bool operator!=(
    const ResourceProviderID& left,
    const ResourceProviderID& right)
{
  return left.value() != right.value();
}


inline bool operator!=(const SlaveID& left, const SlaveID& right)
{
  return left.value() != right.value();
}


inline bool operator!=(const TimeInfo& left, const TimeInfo& right)
{
  return !(left == right);
}


inline bool operator!=(const DurationInfo& left, const DurationInfo& right)
{
  return !(left == right);
}


inline bool operator<(const ContainerID& left, const ContainerID& right)
{
  return left.value() < right.value();
}


inline bool operator<(const ExecutorID& left, const ExecutorID& right)
{
  return left.value() < right.value();
}


inline bool operator<(const FrameworkID& left, const FrameworkID& right)
{
  return left.value() < right.value();
}


inline bool operator<(const OfferID& left, const OfferID& right)
{
  return left.value() < right.value();
}


inline bool operator<(const SlaveID& left, const SlaveID& right)
{
  return left.value() < right.value();
}


inline bool operator<(const TaskID& left, const TaskID& right)
{
  return left.value() < right.value();
}


std::ostream& operator<<(
    std::ostream& stream,
    const CapabilityInfo& capabilityInfo);


std::ostream& operator<<(
    std::ostream& stream,
    const DeviceWhitelist& deviceWhitelist);


std::ostream& operator<<(
    std::ostream& stream,
    const CheckStatusInfo& checkStatusInfo);


std::ostream& operator<<(std::ostream& stream, const CommandInfo& commandInfo);


std::ostream& operator<<(std::ostream& stream, const ContainerID& containerId);


std::ostream& operator<<(
    std::ostream& stream,
    const ContainerInfo& containerInfo);


std::ostream& operator<<(std::ostream& stream, const DomainInfo& domainInfo);


std::ostream& operator<<(std::ostream& stream, const Environment& environment);


std::ostream& operator<<(std::ostream& stream, const ExecutorID& executorId);


std::ostream& operator<<(std::ostream& stream, const ExecutorInfo& executor);


std::ostream& operator<<(std::ostream& stream, const FrameworkID& frameworkId);


std::ostream& operator<<(std::ostream& stream, const MasterInfo& master);


std::ostream& operator<<(std::ostream& stream, const OfferID& offerId);


std::ostream& operator<<(std::ostream& stream, const RateLimits& limits);


std::ostream& operator<<(
    std::ostream& stream,
    const ResourceProviderID& resourceProviderId);


std::ostream& operator<<(std::ostream& stream, const RLimitInfo& rlimitInfo);


std::ostream& operator<<(std::ostream& stream, const SlaveID& slaveId);


std::ostream& operator<<(std::ostream& stream, const SlaveInfo& slave);


std::ostream& operator<<(std::ostream& stream, const TaskID& taskId);


std::ostream& operator<<(std::ostream& stream, const MachineID& machineId);


std::ostream& operator<<(std::ostream& stream, const TaskInfo& task);


std::ostream& operator<<(std::ostream& stream, const TaskState& state);


std::ostream& operator<<(
    std::ostream& stream,
    const std::vector<TaskID>& taskIds);


std::ostream& operator<<(std::ostream& stream, const CheckInfo::Type& type);


std::ostream& operator<<(
    std::ostream& stream,
    const FrameworkInfo::Capability& capability);


std::ostream& operator<<(std::ostream& stream, const Image::Type& imageType);


std::ostream& operator<<(std::ostream& stream, const Secret::Type& secretType);


template <typename T>
inline std::ostream& operator<<(
    std::ostream& stream,
    const google::protobuf::RepeatedPtrField<T>& messages)
{
  stream << "[ ";
  for (auto it = messages.begin(); it != messages.end(); ++it) {
    if (it != messages.begin()) {
      stream << ", ";
    }
    stream << *it;
  }
  stream << " ]";
  return stream;
}


std::ostream& operator<<(
    std::ostream& stream,
    const hashmap<std::string, std::string>& map);

std::ostream& operator<<(
    std::ostream& stream,
    const ::google::protobuf::Message& map);

} // namespace mesos {

namespace std {

template <>
struct hash<mesos::CommandInfo_URI>
{
  typedef size_t result_type;

  typedef mesos::CommandInfo_URI argument_type;

  result_type operator()(const argument_type& uri) const
  {
    size_t seed = 0;

    if (uri.extract()) {
      seed += 11;
    }

    if (uri.executable()) {
      seed += 2003;
    }

    boost::hash_combine(seed, uri.value());
    return seed;
  }
};


template <>
struct hash<mesos::ContainerID>
{
  typedef size_t result_type;

  typedef mesos::ContainerID argument_type;

  result_type operator()(const argument_type& containerId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, containerId.value());

    if (containerId.has_parent()) {
      boost::hash_combine(
          seed,
          std::hash<mesos::ContainerID>()(containerId.parent()));
    }

    return seed;
  }
};


template <>
struct hash<mesos::ExecutorID>
{
  typedef size_t result_type;

  typedef mesos::ExecutorID argument_type;

  result_type operator()(const argument_type& executorId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, executorId.value());
    return seed;
  }
};


template <>
struct hash<mesos::FrameworkID>
{
  typedef size_t result_type;

  typedef mesos::FrameworkID argument_type;

  result_type operator()(const argument_type& frameworkId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, frameworkId.value());
    return seed;
  }
};


template <>
struct hash<mesos::OfferID>
{
  typedef size_t result_type;

  typedef mesos::OfferID argument_type;

  result_type operator()(const argument_type& offerId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, offerId.value());
    return seed;
  }
};


template <>
struct hash<mesos::SlaveID>
{
  typedef size_t result_type;

  typedef mesos::SlaveID argument_type;

  result_type operator()(const argument_type& slaveId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, slaveId.value());
    return seed;
  }
};


template <>
struct hash<mesos::TaskID>
{
  typedef size_t result_type;

  typedef mesos::TaskID argument_type;

  result_type operator()(const argument_type& taskId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, taskId.value());
    return seed;
  }
};


template <>
struct hash<mesos::TaskState>
{
  typedef size_t result_type;

  typedef mesos::TaskState argument_type;

  result_type operator()(const argument_type& taskState) const
  {
    // Use the underlying type of the enum as hash value.
    return static_cast<size_t>(taskState);
  }
};


template <>
struct hash<mesos::TaskStatus_Source>
{
  typedef size_t result_type;

  typedef mesos::TaskStatus_Source argument_type;

  result_type operator()(const argument_type& source) const
  {
    // Use the underlying type of the enum as hash value.
    return static_cast<size_t>(source);
  }
};


template <>
struct hash<mesos::TaskStatus_Reason>
{
  typedef size_t result_type;

  typedef mesos::TaskStatus_Reason argument_type;

  result_type operator()(const argument_type& reason) const
  {
    // Use the underlying type of the enum as hash value.
    return static_cast<size_t>(reason);
  }
};


template <>
struct hash<mesos::Image::Type>
{
  typedef size_t result_type;

  typedef mesos::Image::Type argument_type;

  result_type operator()(const argument_type& imageType) const
  {
    // Use the underlying type of the enum as hash value.
    return static_cast<size_t>(imageType);
  }
};


template <>
struct hash<std::pair<mesos::FrameworkID, mesos::ExecutorID>>
{
  typedef size_t result_type;

  typedef std::pair<
      mesos::FrameworkID, mesos::ExecutorID> argument_type;

  result_type operator()(const argument_type& pair) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, std::hash<mesos::FrameworkID>()(pair.first));
    boost::hash_combine(seed, std::hash<mesos::ExecutorID>()(pair.second));
    return seed;
  }
};


template <>
struct hash<mesos::MachineID>
{
  typedef size_t result_type;

  typedef mesos::MachineID argument_type;

  result_type operator()(const argument_type& machineId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, strings::lower(machineId.hostname()));
    boost::hash_combine(seed, machineId.ip());
    return seed;
  }
};


template <>
struct hash<mesos::ResourceProviderID>
{
  typedef size_t result_type;

  typedef mesos::ResourceProviderID argument_type;

  result_type operator()(const argument_type& resourceProviderId) const
  {
    size_t seed = 0;
    boost::hash_combine(seed, resourceProviderId.value());
    return seed;
  }
};

} // namespace std {

#endif // __MESOS_TYPE_UTILS_H__
