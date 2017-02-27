// --~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~--
// Copyright 2013-2015 Qualcomm Technologies, Inc.
// All rights reserved.
// Confidential and Proprietary – Qualcomm Technologies, Inc.
// --~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~----~--~--~--~--
#pragma once

#include <array>
#include <mutex>
#include <set>
#include <string>

#include <symphony/devicetypes.hh>

#include <symphony/internal/buffer/arenaaccess.hh>
#include <symphony/internal/buffer/executordevice.hh>
#include <symphony/internal/buffer/memregion.hh>
#include <symphony/internal/compat/compilercompat.h>
#include <symphony/internal/util/debug.hh>
#include <symphony/internal/util/macros.hh>
#include <symphony/internal/util/symphonyptrs.hh>
#include <symphony/internal/util/strprintf.hh>

namespace symphony {
namespace internal {

using per_device_arena_array = std::array<arena*, static_cast<size_t>(executor_device::last)+1>;

inline
std::string to_string(per_device_arena_array const& pdaa) {
  std::string s = "[";
  for(auto a : pdaa)
    s += ::symphony::internal::strprintf("%p ", a);
  s += "]";
  return s;
}

struct buffer_acquire_info {
public:
  enum access_t {
    unspecified, read, write, readwrite
  };

  static inline
  std::string to_string(access_t access) {
    return std::string(access == unspecified ? "U" :
                       (access == read ? "R" :
                        (access == write ? "W" : "RW")));
  }

  void*                  _acquired_by;

  executor_device_bitset _edb;

  access_t               _access;

  bool                   _tentative_acquire;

  per_device_arena_array _acquired_arena_per_device;

  buffer_acquire_info() :
    _acquired_by(nullptr),
    _edb(),
    _access(unspecified),
    _tentative_acquire(false),
    _acquired_arena_per_device()
  {}

  explicit buffer_acquire_info(void const* acquired_by) :
    _acquired_by(const_cast<void*>(acquired_by)),
    _edb(),
    _access(unspecified),
    _tentative_acquire(false),
    _acquired_arena_per_device()
  {}

  explicit buffer_acquire_info(void const*            acquired_by,
                               executor_device_bitset const& edb) :
    _acquired_by(const_cast<void*>(acquired_by)),
    _edb(edb),
    _access(unspecified),
    _tentative_acquire(false),
    _acquired_arena_per_device()
  {}

  buffer_acquire_info(void const*            acquired_by,
                      executor_device_bitset edb,
                      access_t               access,
                      bool                   tentative_acquire) :
    _acquired_by(const_cast<void*>(acquired_by)),
    _edb(edb),
    _access(access),
    _tentative_acquire(tentative_acquire),
    _acquired_arena_per_device()
  {}

  std::string to_string() const {
    return ::symphony::internal::strprintf("{%p, %s, %s, %s, %s}",
                                           _acquired_by,
                                           ::symphony::internal::to_string(_edb).c_str(),
                                           this->to_string(_access).c_str(),
                                           (_tentative_acquire ? "T" : "F"),
                                           ::symphony::internal::to_string(_acquired_arena_per_device).c_str());
  }
};

bool operator<(buffer_acquire_info const& bai1, buffer_acquire_info const& bai2);

inline
bool operator<(buffer_acquire_info const& bai1, buffer_acquire_info const& bai2)
{
  return bai1._acquired_by < bai2._acquired_by;
}

struct copy_from_arena_info {
  bool   _has_copy_conflict;
  arena* _src_arena;

  bool has_copy_conflict() const {
    SYMPHONY_INTERNAL_ASSERT(!_has_copy_conflict || _src_arena == nullptr,
                             "Cannot have a source arena when a copy conflict has been detected");
    return _has_copy_conflict;
  }

  arena* src_arena() const {
    SYMPHONY_INTERNAL_ASSERT(!_has_copy_conflict || _src_arena == nullptr,
                             "Cannot have a source arena when a copy conflict has been detected");
    return _src_arena;
  }
};

SYMPHONY_GCC_IGNORE_BEGIN("-Weffc++")
class bufferstate : public ::symphony::internal::ref_counted_object<bufferstate> {
SYMPHONY_GCC_IGNORE_END("-Weffc++")

private:
  using existing_arenas_t = std::array<arena*, NUM_ARENA_TYPES>;
  using arena_set_t       = std::array<bool,   NUM_ARENA_TYPES>;

  size_t const _size_in_bytes;

  bool const _automatic_host_sync;

  std::mutex _mutex;

  device_set _device_hints;

  existing_arenas_t _existing_arenas;

  arena_set_t _valid_data_arenas;

  std::set<buffer_acquire_info> _acquire_set;

private:

  friend class buffer_ptr_base;

  bufferstate(size_t size_in_bytes, bool automatic_host_sync, device_set const& device_hints) :
    _size_in_bytes(size_in_bytes),
    _automatic_host_sync(automatic_host_sync),
    _mutex(),
    _device_hints(device_hints),
    _existing_arenas(),
    _valid_data_arenas(),
    _acquire_set()
  {
    for(auto& a : _existing_arenas)
      a = nullptr;
    for(auto& va: _valid_data_arenas)
      va = false;
  }

  SYMPHONY_DELETE_METHOD(bufferstate());

  SYMPHONY_DELETE_METHOD(bufferstate(bufferstate const&));
  SYMPHONY_DELETE_METHOD(bufferstate& operator=(bufferstate const&));

  SYMPHONY_DELETE_METHOD(bufferstate(bufferstate&&));
  SYMPHONY_DELETE_METHOD(bufferstate& operator=(bufferstate&&));

public:

  ~bufferstate() {
    for(auto& a: _existing_arenas) {
      if(a != nullptr) {
        arena_state_manip::delete_arena(a);
        a = nullptr;
      }
    }

    for(auto& va: _valid_data_arenas)
      va = false;
  }

  size_t get_size_in_bytes() const { return _size_in_bytes; }

  bool does_automatic_host_sync() const { return _automatic_host_sync; }

  std::mutex& access_mutex() {
    return _mutex;
  }

  device_set const& get_device_hints() const {
    return _device_hints;
  }

  inline
  arena* get(arena_t arena_type) const {
    SYMPHONY_INTERNAL_ASSERT(arena_type >= 0 && arena_type < NUM_ARENA_TYPES, "Invalid arena_type %zu", arena_type);
    return _existing_arenas[arena_type];
  }

  inline
  bool has(arena_t arena_type) const {
    return get(arena_type) != nullptr;
  }

  void add_device_hint(device_type d) { _device_hints.add(d); }

  void remove_device_hint(device_type d) { _device_hints.remove(d); }

  inline
  bool buffer_holds_valid_data() const {
    for(auto va : _valid_data_arenas)
      if(va == true)
        return true;
    return false;
  }

  size_t num_valid_data_arenas() const {
    size_t count = 0;
    for(auto va : _valid_data_arenas)
      if(va == true)
        count++;
    return count;
  }

  bool is_valid_data_arena(arena* a) const {
    SYMPHONY_INTERNAL_ASSERT(a != nullptr, "arena=%p has not been created", a);

    auto arena_type = arena_state_manip::get_type(a);
    SYMPHONY_INTERNAL_ASSERT(arena_type != NO_ARENA, "Unusable arena type");
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[arena_type] == a,
                             "arena=%p does not exist in bufferstate=%p",
                             a,
                             this);

    SYMPHONY_INTERNAL_ASSERT(_valid_data_arenas[arena_type] == false ||
                             arena_state_manip::get_alloc_type(a) != UNALLOCATED,
                             "arena=%p is marked as having valid data in bufstate=%p but is UNALLOCATED",
                             a,
                             this);

    return _valid_data_arenas[arena_type];
  }

  bool is_valid_data_arena(arena_t arena_type) const {

    auto a = get(arena_type);
    if(a == nullptr)
      return false;

    SYMPHONY_INTERNAL_ASSERT(_valid_data_arenas[arena_type] == false ||
                             arena_state_manip::get_alloc_type(a) != UNALLOCATED,
                             "arena=%p is marked as having valid data in bufstate=%p but is UNALLOCATED",
                             a,
                             this);

    return _valid_data_arenas[arena_type];
  }

  void add_arena(arena* a, bool has_valid_data) {
    SYMPHONY_INTERNAL_ASSERT(a != nullptr,
                             "Arena object needs to have been created a-priori by the bufferpolicy.");
    auto arena_type = arena_state_manip::get_type(a);
    SYMPHONY_INTERNAL_ASSERT(arena_type != NO_ARENA,
                             "new arena does not have a usable arena type");
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[arena_type] == nullptr,
                             "When adding arena=%p, arena=%p already present in this buffer "
                             "for arena_type=%d bufferstate=%p",
                             a,
                             _existing_arenas[arena_type],
                             static_cast<int>(arena_type),
                             this);
    _existing_arenas[arena_type] = a;

    SYMPHONY_INTERNAL_ASSERT(has_valid_data == false || num_valid_data_arenas() == 0,
                             "New arena brings valid data, but there are already arenas allocated with valid data.\n"
                             "Disable this assertion if the buffer policy can guarantee that the new arena's data is"
                             "already consistent.");
    if(has_valid_data)
      _valid_data_arenas[arena_type] = true;
  }

  void designate_as_unique_valid_data_arena(arena* unique_a) {
    SYMPHONY_INTERNAL_ASSERT(unique_a != nullptr,
                             "Arena object needs to have been created a-priori by the bufferpolicy.");
    auto arena_type = arena_state_manip::get_type(unique_a);
    SYMPHONY_INTERNAL_ASSERT(arena_type != NO_ARENA, "Unusable arena type");
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[arena_type] == unique_a,
                             "Arena to set as having valid data is not allocated in this buffer: unique_a=%p, bufferstate=%p\n",
                             unique_a,
                             this);
    if(_valid_data_arenas[arena_type] == false && buffer_holds_valid_data() == true) {

      auto one_valid_arena = pick_optimal_valid_copy_from_arena(unique_a);
      SYMPHONY_INTERNAL_ASSERT(!one_valid_arena.has_copy_conflict(),
                               "Calling context should have ensured that data can be copied to mainmem_arena: bs=%p",
                               this);
      SYMPHONY_INTERNAL_ASSERT(one_valid_arena.src_arena() != nullptr,
                               "Valid copy-from arena unexpectedly not found");
      if(one_valid_arena.src_arena() != nullptr) {
        copy_valid_data(one_valid_arena.src_arena(), unique_a);
      }
      else {
        SYMPHONY_FATAL("designate_as_unique_valid_data_arena(): Expected non-null value for one_valid_arena.src_arena().");
      }
    }
    _valid_data_arenas[arena_type] = true;

    SYMPHONY_INTERNAL_ASSERT(_existing_arenas.size() == _valid_data_arenas.size(),
                             "_existing_arenas and _valid_data_arenas unexpectedly differ in size");

    for(size_t i=0; i<_existing_arenas.size(); i++) {
      if(i != arena_type && _valid_data_arenas[i] == true) {
        invalidate_arena(_existing_arenas[i]);
      }
    }

    SYMPHONY_INTERNAL_ASSERT(_valid_data_arenas[arena_type] == true && num_valid_data_arenas() == 1,
                         "designate_as_unique_valid_data_arena() must result in exactly one valid arena");
  }

  void invalidate_arena(arena* a) {
    SYMPHONY_INTERNAL_ASSERT(a != nullptr, "Arena is null");
    auto arena_type = arena_state_manip::get_type(a);
    if(_valid_data_arenas[arena_type] == true){
      arena_state_manip::invalidate(a);
      _valid_data_arenas[arena_type] = false;
    }
  }

  void invalidate_all_except(arena* except_arena = nullptr) {
    for(auto a: _existing_arenas){
      if(a != nullptr && a != except_arena) {
        invalidate_arena(a);
      }
    }
  }

  void remove_arena(arena* a, bool delete_arena = false) {
    invalidate_arena(a);
    auto arena_type = arena_state_manip::get_type(a);
    SYMPHONY_INTERNAL_ASSERT(arena_type != NO_ARENA, "Unusable arena type");
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[arena_type] == a,
                             "Removing arena=%p which is not present in bufferstate=%p",
                             a,
                             this);
    _existing_arenas[arena_type] = nullptr;
    if(delete_arena) {
      for(auto other_arena : _existing_arenas) {
        if(other_arena != nullptr && arena_state_manip::get_bound_to_arena(other_arena) == a) {
          SYMPHONY_FATAL("Attempting to delete arena=%p, but another arena=%p is bound to it in bufstate=%s",
                         a,
                         other_arena,
                         to_string().c_str());
        }
      }
      arena_state_manip::delete_arena(a);
    }
  }

  copy_from_arena_info pick_optimal_valid_copy_from_arena(arena* to_arena) const {
    SYMPHONY_INTERNAL_ASSERT(to_arena != nullptr,
                             "to_arena has not been created.");
    auto to_arena_type = arena_state_manip::get_type(to_arena);
    SYMPHONY_INTERNAL_ASSERT(to_arena_type != NO_ARENA,
                             "Unusable arena type of to_arena=%p",
                             to_arena);
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[to_arena_type] == to_arena,
                             "to_arena=%p is not part of this bufferstate=%p",
                             to_arena,
                             this);
    if(_valid_data_arenas[to_arena_type] == true)
      return {false, to_arena};

    auto to_arena_alloc_type = arena_state_manip::get_alloc_type(to_arena);
    SYMPHONY_UNUSED(to_arena_alloc_type);

    for(size_t i=0; i<_existing_arenas.size(); i++) {
      if(_existing_arenas[i] != nullptr && _valid_data_arenas[i] == true &&
         _existing_arenas[i] != to_arena && can_copy(_existing_arenas[i], to_arena))
      {
        return {false, _existing_arenas[i]};
      }
    }

    bool all_sources_conflict = buffer_holds_valid_data();
    return {all_sources_conflict, nullptr};
  }

  void copy_data(arena* from_arena, arena* to_arena) {
    SYMPHONY_INTERNAL_ASSERT(from_arena != nullptr && to_arena != nullptr,
                         "from_arena=%p and to_arena=%p must both be created before copy",
                         from_arena,
                         to_arena);
    auto from_arena_type = arena_state_manip::get_type(from_arena);
    SYMPHONY_INTERNAL_ASSERT(from_arena_type != NO_ARENA,
                         "Unusable arena type of from_arena=%p",
                         from_arena);
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[from_arena_type] == from_arena,
                         "from_arena=%p is not part of this bufferstate=%p",
                         from_arena,
                         this);
    auto to_arena_type = arena_state_manip::get_type(to_arena);
    SYMPHONY_INTERNAL_ASSERT(to_arena_type != NO_ARENA,
                         "Unusable arena type of to_arena=%p",
                         to_arena);
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[to_arena_type] == to_arena,
                         "to_arena=%p is not part of this bufferstate=%p",
                         to_arena,
                         this);

    copy_arenas(from_arena, to_arena);
  }

  void copy_valid_data(arena* valid_from_arena, arena* to_arena) {
    SYMPHONY_INTERNAL_ASSERT(valid_from_arena != nullptr,
                         "valid_from_arena is not yet created");
    auto from_arena_type = arena_state_manip::get_type(valid_from_arena);
    SYMPHONY_INTERNAL_ASSERT(from_arena_type != NO_ARENA,
                         "Unusuable arena type of valid_from_arena=%p",
                         valid_from_arena);
    SYMPHONY_INTERNAL_ASSERT(_existing_arenas[from_arena_type] == valid_from_arena,
                         "valid_from_arena=%p is not part of this bufferstate=%p",
                         valid_from_arena,
                         this);
    SYMPHONY_INTERNAL_ASSERT(_valid_data_arenas[from_arena_type] == true,
                         "source arena=%p needs to have valid data",
                         valid_from_arena);

    copy_data(valid_from_arena, to_arena);

    auto to_arena_type = arena_state_manip::get_type(to_arena);
    SYMPHONY_INTERNAL_ASSERT(to_arena_type != NO_ARENA,
                         "Unusable arena type of to_arena=%p",
                         to_arena);
    _valid_data_arenas[to_arena_type] = true;
  }

  void sync_on_task_finish() {
    SYMPHONY_INTERNAL_ASSERT(does_automatic_host_sync(),
                             "Task finish should only sync a buffer in auto_host_sync mode");
    auto mma = get(MAINMEM_ARENA);
    SYMPHONY_INTERNAL_ASSERT(mma != nullptr && arena_state_manip::get_alloc_type(mma) != UNALLOCATED,
                             "mainmem_arena must always be allocated in conservative access mode");
    designate_as_unique_valid_data_arena(mma);
  }

  struct conflict_info {
    bool        _no_conflict_found;
    void const* _conflicting_requestor;
  };

  conflict_info add_acquire_requestor(void const*                   requestor,
                                      executor_device_bitset        edb,
                                      buffer_acquire_info::access_t access,
                                      bool                          tentative_acquire)
  {
    SYMPHONY_INTERNAL_ASSERT(edb.count() > 0, "executor device doing acquire is unspecified");
    SYMPHONY_INTERNAL_ASSERT(access != buffer_acquire_info::unspecified, "Access type is unspecified during acquire");

    buffer_acquire_info lookup_acqinfo(requestor);
    SYMPHONY_INTERNAL_ASSERT(_acquire_set.find(lookup_acqinfo) == _acquire_set.end(),
                             "requestor=%p is already present in acquire set of bufstate=%p",
                             requestor,
                             this);
    SYMPHONY_UNUSED(lookup_acqinfo);

    if(access == buffer_acquire_info::read) {
      for(auto const& acreq : _acquire_set) {
        if(acreq._access != buffer_acquire_info::read) {
          auto confirmed_conflicting_requestor = (acreq._tentative_acquire ? nullptr : acreq._acquired_by);
          return {false, confirmed_conflicting_requestor};
        }
      }
    }
    else {
      if(!_acquire_set.empty()) {
        auto const& first_acreq = *(_acquire_set.begin());
        auto confirmed_conflicting_requestor = (first_acreq._tentative_acquire ? nullptr : first_acreq._acquired_by);
        return {false, confirmed_conflicting_requestor};
      }
    }

    _acquire_set.insert( buffer_acquire_info(requestor, edb, access, tentative_acquire ) );

    edb.for_each([&](executor_device ed)
    {
      switch(ed){
      default:
      case executor_device::unspecified: SYMPHONY_UNREACHABLE("Unspecified executor_device"); break;
      case executor_device::cpu: this->_device_hints.add(device_type::cpu); break;
      case executor_device::gpucl:
      case executor_device::gpugl: this->_device_hints.add(device_type::gpu); break;
      case executor_device::gputexture: break;
      case executor_device::hexagon: this->_device_hints.add(device_type::hexagon); break;
      }
    });

    return {true, nullptr};
  }

  void confirm_tentative_acquire_requestor(void const* requestor)
  {
    buffer_acquire_info lookup_acqinfo(requestor);
    auto it = _acquire_set.find(lookup_acqinfo);
    SYMPHONY_INTERNAL_ASSERT(it != _acquire_set.end(),
                             "requestor=%p not found in bufstate=%p",
                             requestor,
                             this);
    auto entry = *it;
    SYMPHONY_INTERNAL_ASSERT(entry._tentative_acquire == true,
                             "Entry for requestor=%p was not tentative in bufstate=%p",
                             requestor,
                             this);

    entry._tentative_acquire = false;
    _acquire_set.erase(entry);
    _acquire_set.insert(entry);
  }

  void update_acquire_info_with_arena(void const* requestor,
                                      executor_device ed,
                                      arena* acquired_arena)
  {
    SYMPHONY_INTERNAL_ASSERT(acquired_arena != nullptr, "Error. acquired_arena is null");

    buffer_acquire_info lookup_acqinfo(requestor);
    auto it = _acquire_set.find(lookup_acqinfo);
    SYMPHONY_INTERNAL_ASSERT(it != _acquire_set.end(),
                             "buffer_acquire_info not found: %s",
                             lookup_acqinfo.to_string().c_str());
    auto entry = *it;

    auto ied = static_cast<size_t>(ed);
    SYMPHONY_INTERNAL_ASSERT(ied >= static_cast<size_t>(executor_device::first) &&
                             ied <= static_cast<size_t>(executor_device::last),
                             "Invalid executor device=%zu",
                             ied);
    entry._acquired_arena_per_device[ied] = acquired_arena;

    _acquire_set.erase(entry);
    _acquire_set.insert(entry);
  }

  void remove_acquire_requestor(void const* requestor)
  {
    buffer_acquire_info lookup_acqinfo(requestor);
    auto it = _acquire_set.find(lookup_acqinfo);
    SYMPHONY_INTERNAL_ASSERT(it != _acquire_set.end(),
                             "buffer_acquire_info not found: %s",
                             lookup_acqinfo.to_string().c_str());

    if(it->_tentative_acquire) {
      it->_edb.for_each([&](executor_device ed)
      {
        auto ied = static_cast<size_t>(ed);
        SYMPHONY_UNUSED(ied);
        SYMPHONY_INTERNAL_ASSERT(it->_acquired_arena_per_device[ied] == nullptr,
                                 "A tentative requestor=%p was found with non-null arena=%p for ed=%zu in bufstate=%p",
                                 requestor,
                                 it->_acquired_arena_per_device[ied],
                                 ied,
                                 this);
      });
    }
    else {
      it->_edb.for_each([&](executor_device ed)
      {
        auto ied = static_cast<size_t>(ed);
        SYMPHONY_INTERNAL_ASSERT(it->_acquired_arena_per_device[ied] != nullptr,
                                 "Acquired arena is nullptr for ed=%zu for confirmed requestor=%p in bufstate=%p",
                                 ied,
                                 requestor,
                                 this);
        arena_state_manip::unref(it->_acquired_arena_per_device[ied]);
      });
    }
    _acquire_set.erase(*it);

    SYMPHONY_INTERNAL_ASSERT(_acquire_set.find(lookup_acqinfo) == _acquire_set.end(),
                             "Multiple entries found for requestor=%p in bufstate=%p",
                             requestor,
                             this);
  }

  void const* get_any_confirmed_acquire_requestor() const
  {
    for(auto const& bai: _acquire_set) {
      if(!bai._tentative_acquire)
        return bai._acquired_by;
    }
    return nullptr;
  }

  buffer_acquire_info::access_t get_access_type() const
  {
    if(_acquire_set.empty())
      return buffer_acquire_info::unspecified;
    return _acquire_set.begin()->_access;
  }

  std::string to_string() const {
    std::string s;
    s += "[";
    for(size_t i=0; i<_existing_arenas.size(); i++) {
      auto a = _existing_arenas[i];
      auto bound_to_arena = (a == nullptr ? nullptr : arena_state_manip::get_bound_to_arena(a));

      size_t index_of_bound_to_arena = _existing_arenas.size();
      if(bound_to_arena != nullptr) {
        for(size_t j=0; j<_existing_arenas.size(); j++) {
          if(_existing_arenas[j] == bound_to_arena) {
            index_of_bound_to_arena = j;
            break;
          }
        }
      }

      s += ::symphony::internal::strprintf("%zu=(%p %s %s) ",
                                       i,
                                       _existing_arenas[i],
                                       _valid_data_arenas[i] ? "V" : "I",
                                       (bound_to_arena == nullptr ? "UB" :
                                            ::symphony::internal::strprintf("B%zu", index_of_bound_to_arena).c_str()));
    }
    s += "]";
    return s;
  }

};

};
};
