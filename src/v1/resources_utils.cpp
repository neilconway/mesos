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

#include <stout/check.hpp>

#include "v1/resources_utils.hpp"

namespace mesos {
namespace v1 {

void transformToPreReservationRefinementResource(Resource* resource)
{
  switch (resource->reservations_size()) {
    // unreserved resource.
    case 0: {
      CHECK(!resource->has_reservation());
      resource->set_role("*");
      break;
    }
    // resource with a single reservation.
    case 1: {
      const Resource::ReservationInfo& reservation = resource->reservations(0);
      if (reservation.type() == Resource::ReservationInfo::DYNAMIC) {
        resource->mutable_reservation()->set_principal(reservation.principal());
      }

      resource->set_role(reservation.role());
      resource->clear_reservations();
      break;
    }
    // resource with refined reservations.
    default: { /* do nothing */ };
  }
}


void transformToPostReservationRefinementResource(Resource* resource)
{
  if (resource->reservations_size() > 0) {
    return;
  }

  // unreserved resources.
  if (resource->role() == "*" && !resource->has_reservation()) {
    resource->clear_role();
    return;
  }

  Resource::ReservationInfo reservation;

  if (!resource->has_reservation()) {
    reservation.set_type(Resource::ReservationInfo::STATIC);
  } else {
    reservation = resource->reservation();
    resource->clear_reservation();
    reservation.set_type(Resource::ReservationInfo::DYNAMIC);
  }

  reservation.set_role(resource->role());
  resource->clear_role();
  CHECK_EQ(0, resource->reservations_size());
  resource->add_reservations()->CopyFrom(reservation);
}

} // namespace v1 {
} // namespace mesos {
