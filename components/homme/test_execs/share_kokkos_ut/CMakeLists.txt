SET(SRC_DIR          ${HOMME_SOURCE_DIR}/src)
SET(SRC_SHARE_DIR    ${HOMME_SOURCE_DIR}/src/share)
SET(SRC_PREQX_DIR    ${HOMME_SOURCE_DIR}/src/preqx_kokkos)
SET(SHARE_UT_DIR     ${CMAKE_CURRENT_SOURCE_DIR})
SET(UTILS_TIMING_SRC_DIR  ${HOMME_SOURCE_DIR}/utils/cime/CIME/non_py/src/timing)
SET(UTILS_TIMING_BIN_DIR  ${HOMME_BINARY_DIR}/utils/cime/CIME/non_py/src/timing)
SET(UTILS_TIMING_DIRS ${UTILS_TIMING_SRC_DIR} ${UTILS_TIMING_BIN_DIR})

# Place common CPP definitions here.
# Note: HOMMEXX_BFB_TESTING here, since the share
#       unit tests do not include a config.h file
SET (COMMON_DEFINITIONS NP=4 NC=4)
IF (HOMMEXX_ENABLE_GPU_F90)
  SET(COMMON_DEFINITIONS ${COMMON_DEFINITIONS} HOMMEXX_ENABLE_GPU_F90)
ENDIF()
IF (HOMMEXX_BFB_TESTING)
  SET(COMMON_DEFINITIONS ${COMMON_DEFINITIONS} HOMMEXX_BFB_TESTING)
ENDIF()

### Infrastructure unit stest
if (HOMMEXX_BFB_TESTING)
SET (INFRASTRUCTURE_UT_CXX_SRCS
  ${SRC_SHARE_DIR}/cxx/Context.cpp
  ${SRC_SHARE_DIR}/cxx/ErrorDefs.cpp
  ${SRC_SHARE_DIR}/cxx/ExecSpaceDefs.cpp
  ${SRC_SHARE_DIR}/cxx/Hommexx_Session.cpp
  ${SRC_SHARE_DIR}/cxx/HybridVCoord.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Comm.cpp
  ${SHARE_UT_DIR}/infrastructure_ut.cpp
)
SET (INFRASTRUCTURE_UT_F90_SRCS
  ${SRC_SHARE_DIR}/kinds.F90
  ${SRC_SHARE_DIR}/cxx/utilities/bfb_mod.F90
  ${SHARE_UT_DIR}/infrastructure_ut_mod.F90
)

SET (CONFIG_DEFINES PIO_INTERP QSIZE_D=1 PLEV=13 ${COMMON_DEFINITIONS})
SET (INFRASTRUCTURE_UT_INCLUDE_DIRS
  ${SRC_SHARE_DIR}
  ${SRC_SHARE_DIR}/cxx
  ${SHARE_UT_DIR}
  ${UTILS_TIMING_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src/share/cxx
)

SET (NUM_CPUS 1)
cxx_unit_test (infrastructure_ut "${INFRASTRUCTURE_UT_F90_SRCS}" "${INFRASTRUCTURE_UT_CXX_SRCS}" "${INFRASTRUCTURE_UT_INCLUDE_DIRS}" "${CONFIG_DEFINES}" ${NUM_CPUS})
endif()

### Boundary exchange unit test ###
if (HOMMEXX_BFB_TESTING)
SET (BOUNDARY_EXCHANGE_UT_F90_SRCS
  ${SRC_DIR}/repro_sum_mod.F90
  ${SRC_SHARE_DIR}/bndry_mod_base.F90
  ${SRC_SHARE_DIR}/control_mod.F90
  ${SRC_SHARE_DIR}/coordinate_systems_mod.F90
  ${SRC_SHARE_DIR}/cube_mod.F90
  ${SRC_SHARE_DIR}/planar_mod.F90
  ${SRC_SHARE_DIR}/derivative_mod_base.F90
  ${SRC_SHARE_DIR}/dimensions_mod.F90
  ${SRC_SHARE_DIR}/edge_mod_base.F90
  ${SRC_SHARE_DIR}/edgetype_mod.F90
  ${SRC_SHARE_DIR}/element_mod.F90
  ${SRC_SHARE_DIR}/gridgraph_mod.F90
  ${SRC_SHARE_DIR}/hybrid_mod.F90
  ${SRC_SHARE_DIR}/kinds.F90
  ${SRC_SHARE_DIR}/ll_mod.F90
  ${SRC_SHARE_DIR}/metagraph_mod.F90
  ${SRC_SHARE_DIR}/parallel_mod.F90
  ${SRC_SHARE_DIR}/params_mod.F90
  ${SRC_SHARE_DIR}/physical_constants.F90
  ${SRC_SHARE_DIR}/quadrature_mod.F90
  ${SRC_SHARE_DIR}/schedtype_mod.F90
  ${SRC_SHARE_DIR}/schedule_mod.F90
  ${SRC_SHARE_DIR}/spacecurve_mod.F90
  ${SRC_SHARE_DIR}/thread_mod.F90
  ${SRC_SHARE_DIR}/viscosity_base.F90
  ${SRC_PREQX_DIR}/bndry_mod.F90
  ${SRC_PREQX_DIR}/derivative_mod.F90
  ${SRC_PREQX_DIR}/edge_mod.F90
  ${SRC_PREQX_DIR}/element_state.F90
  ${SHARE_UT_DIR}/boundary_exchange_ut.F90
  ${SHARE_UT_DIR}/geometry_interface.F90
)
SET (BOUNDARY_EXCHANGE_UT_CXX_SRCS
  ${SRC_SHARE_DIR}/cxx/Context.cpp
  ${SRC_SHARE_DIR}/cxx/ErrorDefs.cpp
  ${SRC_SHARE_DIR}/cxx/ExecSpaceDefs.cpp
  ${SRC_SHARE_DIR}/cxx/Hommexx_Session.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/mpi_cxx_f90_interface.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/BoundaryExchange.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Comm.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Connectivity.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/MpiBuffersManager.cpp
  ${SRC_SHARE_DIR}/cxx/utilities/BfbUtils.cpp
  ${SHARE_UT_DIR}/boundary_exchange_ut.cpp
)

SET (CONFIG_DEFINES PLEV=12 QSIZE_D=4 _MPI=1 HOMME_BE_NO_HASHER _PRIM ${COMMON_DEFINITIONS})
SET (BOUNDARY_EXCHANGE_UT_INCLUDE_DIRS
  ${SRC_SHARE_DIR}
  ${SRC_SHARE_DIR}/cxx
  ${SHARE_UT_DIR}
  ${UTILS_TIMING_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src/share/cxx
)

IF (USE_NUM_PROCS)
  SET (NUM_CPUS ${USE_NUM_PROCS})
ELSE()
  SET (NUM_CPUS 1)
ENDIF()
cxx_unit_test (boundary_exchange_ut "${BOUNDARY_EXCHANGE_UT_F90_SRCS}" "${BOUNDARY_EXCHANGE_UT_CXX_SRCS}" "${BOUNDARY_EXCHANGE_UT_INCLUDE_DIRS}" "${CONFIG_DEFINES}" ${NUM_CPUS})
endif ()

### Sphere operators unit test ###
if (HOMMEXX_BFB_TESTING)
SET (SPHERE_OP_UT_F90_SRCS
  ${SRC_SHARE_DIR}/control_mod.F90
  ${SRC_SHARE_DIR}/coordinate_systems_mod.F90
  ${SRC_SHARE_DIR}/derivative_mod_base.F90
  ${SRC_SHARE_DIR}/dimensions_mod.F90
  ${SRC_SHARE_DIR}/edgetype_mod.F90
  ${SRC_SHARE_DIR}/element_mod.F90
  ${SRC_SHARE_DIR}/gridgraph_mod.F90
  ${SRC_SHARE_DIR}/kinds.F90
  ${SRC_SHARE_DIR}/parallel_mod.F90
  ${SRC_SHARE_DIR}/params_mod.F90
  ${SRC_SHARE_DIR}/quadrature_mod.F90
  ${SRC_SHARE_DIR}/physical_constants.F90
  ${SRC_PREQX_DIR}/element_state.F90
  ${SHARE_UT_DIR}/sphere_op_interface.F90
)
SET (SPHERE_OP_UT_CXX_SRCS
  ${SRC_SHARE_DIR}/cxx/Context.cpp
  ${SRC_SHARE_DIR}/cxx/ErrorDefs.cpp
  ${SRC_SHARE_DIR}/cxx/ExecSpaceDefs.cpp
  ${SRC_SHARE_DIR}/cxx/Hommexx_Session.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Comm.cpp
  ${SHARE_UT_DIR}/sphere_op_sl.cpp
  ${SHARE_UT_DIR}/sphere_op_ml.cpp
)

SET (CONFIG_DEFINES PLEV=12 QSIZE_D=4 _MPI=1 _PRIM ${COMMON_DEFINITIONS})
SET (SPHERE_OP_UT_INCLUDE_DIRS
  ${SRC_SHARE_DIR}
  ${SRC_SHARE_DIR}/cxx
  ${SHARE_UT_DIR}
  ${UTILS_TIMING_DIRS}
  ${CMAKE_BINARY_DIR}/src/share/cxx
)

IF (USE_NUM_PROCS)
  SET (NUM_CPUS ${USE_NUM_PROCS})
ELSE()
  SET (NUM_CPUS 1)
ENDIF()
cxx_unit_test (sphere_op_ut "${SPHERE_OP_UT_F90_SRCS}" "${SPHERE_OP_UT_CXX_SRCS}" "${SPHERE_OP_UT_INCLUDE_DIRS}" "${CONFIG_DEFINES}" ${NUM_CPUS})
endif () #BFB

### Limiters unit test ###

SET (LIMITERS_UT_F90_SRCS
  ${SRC_SHARE_DIR}/control_mod.F90
  ${SRC_SHARE_DIR}/coordinate_systems_mod.F90
  ${SRC_SHARE_DIR}/derivative_mod_base.F90
  ${SRC_SHARE_DIR}/dimensions_mod.F90
  ${SRC_SHARE_DIR}/edgetype_mod.F90
  ${SRC_SHARE_DIR}/element_mod.F90
  ${SRC_SHARE_DIR}/gridgraph_mod.F90
  ${SRC_SHARE_DIR}/kinds.F90
  ${SRC_SHARE_DIR}/parallel_mod.F90
  ${SRC_SHARE_DIR}/physical_constants.F90
  ${SRC_SHARE_DIR}/params_mod.F90
  ${SRC_SHARE_DIR}/quadrature_mod.F90
  ${SRC_PREQX_DIR}/element_state.F90
  ${SHARE_UT_DIR}/limiters_interface.F90
)
SET (LIMITERS_UT_CXX_SRCS
  ${SRC_SHARE_DIR}/cxx/Context.cpp
  ${SRC_SHARE_DIR}/cxx/ErrorDefs.cpp
  ${SRC_SHARE_DIR}/cxx/EulerStepFunctorImpl.hpp
  ${SRC_SHARE_DIR}/cxx/Hommexx_Session.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Comm.cpp
  ${SRC_SHARE_DIR}/cxx/ExecSpaceDefs.cpp
  ${SHARE_UT_DIR}/limiters.cpp
)

SET (CONFIG_DEFINES PLEV=12 QSIZE_D=4 _MPI=1 _PRIM ${COMMON_DEFINITIONS})
SET (LIMITERS_UT_INCLUDE_DIRS
  ${SRC_SHARE_DIR}
  ${SRC_SHARE_DIR}/cxx
  ${SHARE_UT_DIR}
  ${UTILS_TIMING_DIRS}
  ${CMAKE_BINARY_DIR}/src/share/cxx
)

IF (USE_NUM_PROCS)
  SET (NUM_CPUS ${USE_NUM_PROCS})
ELSE()
  SET (NUM_CPUS 1)
ENDIF()
cxx_unit_test (limiters_ut "${LIMITERS_UT_F90_SRCS}" "${LIMITERS_UT_CXX_SRCS}" "${LIMITERS_UT_INCLUDE_DIRS}" "${CONFIG_DEFINES}" ${NUM_CPUS})

### ColumnOps unit tests
if (HOMMEXX_BFB_TESTING)
SET (COL_OPS_UT_CXX_SRCS
  ${SRC_SHARE_DIR}/cxx/Context.cpp
  ${SRC_SHARE_DIR}/cxx/ErrorDefs.cpp
  ${SRC_SHARE_DIR}/cxx/ExecSpaceDefs.cpp
  ${SRC_SHARE_DIR}/cxx/Hommexx_Session.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Comm.cpp
  ${SHARE_UT_DIR}/col_ops_ut.cpp
)

SET (CONFIG_DEFINES PIO_INTERP PLEV=12 QSIZE_D=4 _MPI=1 ${COMMON_DEFINITIONS})
SET (COL_OPS_UT_INCLUDE_DIRS
  ${SRC_SHARE_DIR}
  ${SRC_SHARE_DIR}/cxx
  ${SHARE_UT_DIR}
  ${UTILS_TIMING_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src/share/cxx
)

IF (USE_NUM_PROCS)
  SET (NUM_CPUS ${USE_NUM_PROCS})
ELSE()
  SET (NUM_CPUS 1)
ENDIF()
cxx_unit_test (col_ops_ut "${COL_OPS_UT_F90_SRCS}" "${COL_OPS_UT_CXX_SRCS}" "${COL_OPS_UT_INCLUDE_DIRS}" "${CONFIG_DEFINES}" ${NUM_CPUS})
endif ()

### PpmRemap unit test ###
if (HOMMEXX_BFB_TESTING)
SET (PPM_REMAP_UT_F90_SRCS
  ${SRC_SHARE_DIR}/control_mod.F90
  ${SRC_SHARE_DIR}/dimensions_mod.F90
  ${SRC_SHARE_DIR}/hybvcoord_mod.F90
  ${SRC_SHARE_DIR}/kinds.F90
  ${SRC_SHARE_DIR}/parallel_mod.F90
  ${SRC_SHARE_DIR}/physical_constants.F90
  ${SRC_SHARE_DIR}/vertremap_base.F90
  ${SHARE_UT_DIR}/ppm_remap_interface.F90
)

SET (PPM_REMAP_UT_CXX_SRCS
  ${SRC_SHARE_DIR}/cxx/Context.cpp
  ${SRC_SHARE_DIR}/cxx/ErrorDefs.cpp
  ${SRC_SHARE_DIR}/cxx/ExecSpaceDefs.cpp
  ${SRC_SHARE_DIR}/cxx/Hommexx_Session.cpp
  ${SRC_SHARE_DIR}/cxx/mpi/Comm.cpp
  ${SHARE_UT_DIR}/ppm_remap_ut.cpp
)

SET (CONFIG_DEFINES PLEV=12 QSIZE_D=4 _MPI=1 _PRIM ${COMMON_DEFINITIONS})
SET (PPM_REMAP_UT_INCLUDE_DIRS
  ${SRC_PREQX_DIR}/cxx
  ${SRC_SHARE_DIR}
  ${SRC_SHARE_DIR}/cxx
  ${UTILS_TIMING_DIRS}
  ${CMAKE_BINARY_DIR}/src/share/cxx
)

IF (USE_NUM_PROCS)
  SET (NUM_CPUS ${USE_NUM_PROCS})
ELSE()
  SET (NUM_CPUS 1)
ENDIF()
cxx_unit_test (ppm_remap_ut "${PPM_REMAP_UT_F90_SRCS}" "${PPM_REMAP_UT_CXX_SRCS}" "${PPM_REMAP_UT_INCLUDE_DIRS}" "${CONFIG_DEFINES}" ${NUM_CPUS})
endif ()
