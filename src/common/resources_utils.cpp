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

#include <stout/error.hpp>
#include <stout/foreach.hpp>
#include <stout/stringify.hpp>

#include "common/resources_utils.hpp"

namespace mesos {

bool needCheckpointing(const Resource& resource)
{
  return Resources::isDynamicallyReserved(resource) ||
         Resources::isPersistentVolume(resource);
}


// NOTE: We effectively duplicate the logic in 'Resources::apply'
// which is less than ideal. But we cannot simply create
// 'Offer::Operation' and invoke 'Resources::apply' here.
// 'RESERVE' operation requires that the specified resources are
// dynamically reserved only, and 'CREATE' requires that the
// specified resources are already dynamically reserved.
// These requirements are violated when we try to infer dynamically
// reserved persistent volumes.
// TODO(mpark): Consider introducing an atomic 'RESERVE_AND_CREATE'
// operation to solve this problem.
Try<Resources> applyCheckpointedResources(
    const Resources& resources,
    const Resources& checkpointedResources)
{
  Resources totalResources = resources;

  foreach (const Resource& resource, checkpointedResources) {
    if (!needCheckpointing(resource)) {
      return Error("Unexpected checkpointed resources " + stringify(resource));
    }

    Resource stripped = resource;

    if (Resources::isDynamicallyReserved(resource)) {
      Resource::ReservationInfo reservation = stripped.reservations(0);
      stripped.clear_reservations();
      if (reservation.type() == Resource::ReservationInfo::STATIC) {
        stripped.add_reservations()->CopyFrom(reservation);
      }
    }

    // Strip persistence and volume from the disk info so that we can
    // check whether it is contained in the `totalResources`.
    if (Resources::isPersistentVolume(resource)) {
      if (stripped.disk().has_source()) {
        stripped.mutable_disk()->clear_persistence();
        stripped.mutable_disk()->clear_volume();
      } else {
        stripped.clear_disk();
      }
    }

    stripped.clear_shared();

    if (!totalResources.contains(stripped)) {
      return Error(
          "Incompatible agent resources: " + stringify(totalResources) +
          " does not contain " + stringify(stripped));
    }

    totalResources -= stripped;
    totalResources += resource;
  }

  return totalResources;
}


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
    default: { /* do nothing */
    };
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


void transformToPreReservationRefinementResources(
    google::protobuf::RepeatedPtrField<Resource>* resources)
{
  foreach (Resource& resource, *resources) {
    transformToPreReservationRefinementResource(&resource);
  }
}


void transformToPreReservationRefinementResources(
    std::vector<Resource>* resources)
{
  foreach (Resource& resource, *resources) {
    transformToPreReservationRefinementResource(&resource);
  }
}


void transformToPostReservationRefinementResources(
    google::protobuf::RepeatedPtrField<Resource>* resources)
{
  foreach (Resource& resource, *resources) {
    transformToPostReservationRefinementResource(&resource);
  }
}


void transformToPostReservationRefinementResources(
    std::vector<Resource>* resources)
{
  foreach (Resource& resource, *resources) {
    transformToPostReservationRefinementResource(&resource);
  }
}


void transformToPreReservationRefinementResources(Offer::Operation* operation)
{
  switch (operation->type()) {
    case Offer::Operation::LAUNCH: {
      Offer::Operation::Launch* launch = operation->mutable_launch();

      foreach (TaskInfo& task, *launch->mutable_task_infos()) {
        transformToPreReservationRefinementResources(task.mutable_resources());

        if (task.has_executor()) {
          transformToPreReservationRefinementResources(
              task.mutable_executor()->mutable_resources());
        }
      }
      break;
    }

    case Offer::Operation::LAUNCH_GROUP: {
      Offer::Operation::LaunchGroup* launchGroup =
        operation->mutable_launch_group();

      if (launchGroup->has_executor()) {
        transformToPreReservationRefinementResources(
            launchGroup->mutable_executor()->mutable_resources());
      }

      TaskGroupInfo* taskGroup = launchGroup->mutable_task_group();

      foreach (TaskInfo& task, *taskGroup->mutable_tasks()) {
        transformToPreReservationRefinementResources(task.mutable_resources());

        if (task.has_executor()) {
          transformToPreReservationRefinementResources(
              task.mutable_executor()->mutable_resources());
        }
      }
      break;
    }

    case Offer::Operation::RESERVE: {
      transformToPreReservationRefinementResources(
          operation->mutable_reserve()->mutable_resources());

      break;
    }

    case Offer::Operation::UNRESERVE: {
      transformToPreReservationRefinementResources(
          operation->mutable_unreserve()->mutable_resources());

      break;
    }

    case Offer::Operation::CREATE: {
      transformToPreReservationRefinementResources(
          operation->mutable_create()->mutable_volumes());

      break;
    }

    case Offer::Operation::DESTROY: {
      transformToPreReservationRefinementResources(
          operation->mutable_destroy()->mutable_volumes());

      break;
    }

    case Offer::Operation::UNKNOWN:
      break; // No-op.
  }
}


void transformToPostReservationRefinementResources(Offer::Operation* operation)
{
  switch (operation->type()) {
    case Offer::Operation::LAUNCH: {
      Offer::Operation::Launch* launch = operation->mutable_launch();

      foreach (TaskInfo& task, *launch->mutable_task_infos()) {
        transformToPostReservationRefinementResources(task.mutable_resources());

        if (task.has_executor()) {
          transformToPostReservationRefinementResources(
              task.mutable_executor()->mutable_resources());
        }
      }
      break;
    }

    case Offer::Operation::LAUNCH_GROUP: {
      Offer::Operation::LaunchGroup* launchGroup =
        operation->mutable_launch_group();

      if (launchGroup->has_executor()) {
        transformToPostReservationRefinementResources(
            launchGroup->mutable_executor()->mutable_resources());
      }

      TaskGroupInfo* taskGroup = launchGroup->mutable_task_group();

      foreach (TaskInfo& task, *taskGroup->mutable_tasks()) {
        transformToPostReservationRefinementResources(task.mutable_resources());

        if (task.has_executor()) {
          transformToPostReservationRefinementResources(
              task.mutable_executor()->mutable_resources());
        }
      }
      break;
    }

    case Offer::Operation::RESERVE: {
      transformToPostReservationRefinementResources(
          operation->mutable_reserve()->mutable_resources());

      break;
    }

    case Offer::Operation::UNRESERVE: {
      transformToPostReservationRefinementResources(
          operation->mutable_unreserve()->mutable_resources());

      break;
    }

    case Offer::Operation::CREATE: {
      transformToPostReservationRefinementResources(
          operation->mutable_create()->mutable_volumes());

      break;
    }

    case Offer::Operation::DESTROY: {
      transformToPostReservationRefinementResources(
          operation->mutable_destroy()->mutable_volumes());

      break;
    }

    case Offer::Operation::UNKNOWN:
      break; // No-op.
  }
}

} // namespace mesos {
