#include <catch2/catch.hpp>

#include <iostream>
#include <random>

#include "Context.hpp"
#include "ElementsGeometry.hpp"
#include "ElementsState.hpp"
#include "ElementsForcing.hpp"
#include "HybridVCoord.hpp"
#include "ForcingFunctor.hpp"
#include "SimulationParams.hpp"
#include "Tracers.hpp"
#include "Types.hpp"

#include "utilities/MathUtils.hpp"
#include "utilities/TestUtils.hpp"
#include "utilities/SubviewUtils.hpp"
#include "utilities/SyncUtils.hpp"

using namespace Homme;

template<typename T>
using HVM = HostViewManaged<T>;


// ============= THETA MODEL FORCING ================ //

extern "C" {
void init_f90 (const int& num_elems,
               const Real* hyai_ptr, const Real* hybi_ptr,
               const Real* hyam_ptr, const Real* hybm_ptr,
               const Real* gradphis,
               const Real& ps0, const int& qsize);
void set_forcing_pointers_f90 (Real*& q_ptr, Real*& fq_ptr, Real*& qdp_ptr,
                               Real*& v_ptr, Real*& w_ptr, Real*& vtheta_ptr, 
                               Real*& dp_ptr, Real*& phinh_ptr, Real*& ps_ptr,
                               Real*& fm_ptr, Real*& ft_ptr,
                               Real*& fvtheta_ptr, Real*& fphi_ptr);
void tracers_forcing_f90 (const Real& dt, const int& np1, const int& np1_qdp, const bool& hydrostatic, const bool& moist);
} // extern "C"

TEST_CASE("forcing", "forcing") {
  using rngAlg = std::mt19937_64;
  using ipdf = std::uniform_int_distribution<int>;
  using dpdf = std::uniform_real_distribution<double>;

  constexpr int num_elems = 1;
  const int seed = 1984;// rd();
  std::random_device rd;
  rngAlg engine(seed);

  // Init everything through singleton, which is what happens in normal runs
  auto& c = Context::singleton();
  auto& p = c.create<SimulationParams>();

  auto& hv = c.create<HybridVCoord>();
  hv.random_init(seed);

  auto& geo     = c.create<ElementsGeometry>();
  geo.init(num_elems,true);
  geo.randomize(seed);

  auto& state   = c.create<ElementsState>();
  state.init(num_elems);

  auto& forcing = c.create<ElementsForcing>();
  forcing.init(num_elems);

  auto& tracers = c.create<Tracers>();
  p.qsize = ipdf(1,QSIZE_D)(engine);
  tracers.init(num_elems,p.qsize);
  // Kokkos::deep_copy(tracers.fq,0.0);

  const Real dt = dpdf(0.1,10.0)(engine);
  const int np1 = ipdf(0,2)(engine);
  const int np1_qdp = ipdf(0,1)(engine);

  // Init the f90 side
  geo.m_gradphis = decltype(geo.m_gradphis)("",num_elems);
  genRandArray(geo.m_gradphis,engine,dpdf(0.1,1.0));
  auto h_hyai = Kokkos::create_mirror_view(hv.hybrid_ai);
  auto h_hybi = Kokkos::create_mirror_view(hv.hybrid_bi);
  auto h_hyam = Kokkos::create_mirror_view(hv.hybrid_am);
  auto h_hybm = Kokkos::create_mirror_view(hv.hybrid_bm);
  auto h_gradphis = Kokkos::create_mirror_view(geo.m_gradphis);
  Kokkos::deep_copy(h_hyai,hv.hybrid_ai);
  Kokkos::deep_copy(h_hybi,hv.hybrid_bi);
  Kokkos::deep_copy(h_hyam,hv.hybrid_am);
  Kokkos::deep_copy(h_hybm,hv.hybrid_bm);
  Kokkos::deep_copy(h_gradphis,geo.m_gradphis);
  init_f90(num_elems,
           h_hyai.data(), h_hybi.data(),
           reinterpret_cast<Real*>(h_hyam.data()),
           reinterpret_cast<Real*>(h_hybm.data()),
           h_gradphis.data(), hv.ps0, p.qsize);

  // Create f90-layout views
  HVM<Real*[QSIZE_D][NUM_PHYSICAL_LEV][NP][NP]>                    q_f90 ("",num_elems);
  HVM<Real*[QSIZE_D][NUM_PHYSICAL_LEV][NP][NP]>                    fq_f90 ("",num_elems);
  HVM<Real*[Q_NUM_TIME_LEVELS][QSIZE_D][NUM_PHYSICAL_LEV][NP][NP]> qdp_f90("",num_elems);

  HVM<Real*[NUM_TIME_LEVELS][NUM_PHYSICAL_LEV][2][NP][NP]> v_f90("",num_elems);
  HVM<Real*[NUM_TIME_LEVELS][NUM_INTERFACE_LEV][NP][NP]>   w_f90("",num_elems);
  HVM<Real*[NUM_TIME_LEVELS][NUM_PHYSICAL_LEV][NP][NP]>    dp_f90("",num_elems);
  HVM<Real*[NUM_TIME_LEVELS][NUM_PHYSICAL_LEV][NP][NP]>    vtheta_f90("",num_elems);
  HVM<Real*[NUM_TIME_LEVELS][NUM_INTERFACE_LEV][NP][NP]>   phinh_f90("",num_elems);
  HVM<Real*[NUM_TIME_LEVELS][NP][NP]>                      ps_f90("",num_elems);

  HVM<Real*[NUM_PHYSICAL_LEV][3][NP][NP]> fm_f90("",num_elems);
  HVM<Real*[NUM_PHYSICAL_LEV][NP][NP]>    ft_f90("",num_elems);
  HVM<Real*[NUM_PHYSICAL_LEV][NP][NP]>    fvtheta_f90("",num_elems);
  HVM<Real*[NUM_INTERFACE_LEV][NP][NP]>   fphi_f90("",num_elems);

  // Get pointers, and init f90
  Real* q_ptr      = q_f90.data();
  Real* fq_ptr     = fq_f90.data();
  Real* qdp_ptr    = qdp_f90.data();

  Real* v_ptr      = v_f90.data();
  Real* w_ptr      = w_f90.data();
  Real* vtheta_ptr = vtheta_f90.data();
  Real* dp_ptr     = dp_f90.data();
  Real* phinh_ptr  = phinh_f90.data();
  Real* ps_ptr     = ps_f90.data();

  Real* fm_ptr      = fm_f90.data();
  Real* ft_ptr      = ft_f90.data();
  Real* fvtheta_ptr = fvtheta_f90.data();
  Real* fphi_ptr    = fphi_f90.data();

  set_forcing_pointers_f90 (q_ptr, fq_ptr, qdp_ptr,
                            v_ptr, w_ptr, vtheta_ptr, dp_ptr, phinh_ptr, ps_ptr,
                            fm_ptr, ft_ptr, fvtheta_ptr, fphi_ptr);

  // Host mirrors of cxx views (for results checking)
  auto h_q       = Kokkos::create_mirror_view(tracers.Q);
  auto h_fq      = Kokkos::create_mirror_view(tracers.fq);
  auto h_qdp     = Kokkos::create_mirror_view(tracers.qdp);

  auto h_dp      = Kokkos::create_mirror_view(state.m_dp3d);
  auto h_v       = Kokkos::create_mirror_view(state.m_v);
  auto h_w       = Kokkos::create_mirror_view(state.m_w_i);
  auto h_vtheta  = Kokkos::create_mirror_view(state.m_vtheta_dp);
  auto h_phi     = Kokkos::create_mirror_view(state.m_phinh_i);

  auto h_fm      = Kokkos::create_mirror_view(forcing.m_fm);
  auto h_ft      = Kokkos::create_mirror_view(forcing.m_ft);
  auto h_fvtheta = Kokkos::create_mirror_view(forcing.m_fvtheta);
  auto h_fphi    = Kokkos::create_mirror_view(forcing.m_fphi);

  for (const bool hydrostatic : {true,false}) {
    std::cout << (hydrostatic ? " -> Hydrostatic mode\n" : " -> Non-hydrostatic mode\n");
    for (const MoistDry moisture : {MoistDry::DRY,MoistDry::MOIST}) {
      const bool moist = (moisture==MoistDry::MOIST);
      std::cout << (moist ? "  -> Moist case\n" : "  -> Dry case\n");

      // Reset state, tracers, and forcing to the original random values
      state.randomize(seed, 10*hv.ps0, hv.ps0);
      tracers.randomize(seed);
      forcing.randomize(seed);

      // Sync views
      sync_to_host(tracers.Q,q_f90);
      sync_to_host(tracers.fq,fq_f90);
      sync_to_host(tracers.qdp,qdp_f90);

      sync_to_host(state.m_v,v_f90);
      sync_to_host(state.m_w_i,w_f90);
      sync_to_host(state.m_vtheta_dp,vtheta_f90);
      sync_to_host(state.m_dp3d,dp_f90);
      sync_to_host(state.m_phinh_i,phinh_f90);
      // ps has same layout in cxx and f90
      Kokkos::deep_copy(ps_f90,state.m_ps_v);

      sync_to_host<3>(forcing.m_fm,fm_f90);
      sync_to_host(forcing.m_ft,ft_f90);
      sync_to_host(forcing.m_fvtheta,fvtheta_f90);
      sync_to_host(forcing.m_fphi,fphi_f90);

      p.theta_hydrostatic_mode = hydrostatic;

      // Create the functor
      FunctorsBuffersManager fbm;
      ForcingFunctor ff;

      // Create and set buffers in the forcing functor
      fbm.request_size(ff.requested_buffer_size());
      fbm.allocate();
      ff.init_buffers(fbm);

      // Run tracers forcing (cxx and f90)
      ff.tracers_forcing(dt,np1,np1_qdp,false,moisture);
      tracers_forcing_f90(dt,np1+1,np1_qdp+1,hydrostatic,moisture==MoistDry::MOIST);

      // Compare answers
      for (int ie=0; ie<num_elems; ++ie) {
        for (int igp=0; igp<NP; ++igp) {
          for (int jgp=0; jgp<NP; ++jgp) {
            for (int k=0; k<NUM_PHYSICAL_LEV; ++k) {
              const int ilev = k / VECTOR_SIZE;
              const int ivec = k % VECTOR_SIZE;

              REQUIRE(h_dp(ie,np1,igp,jgp,ilev)[ivec]==dp_f90(ie,np1,k,igp,jgp));
if(h_fphi(ie,igp,jgp,ilev)[ivec]!=fphi_f90(ie,k,igp,jgp)) {
  printf ("ie,k,igp,jgp: %d, %d, %d, %d.\n",ie,k,igp,jgp);
}
              REQUIRE(h_fphi(ie,igp,jgp,ilev)[ivec]==fphi_f90(ie,k,igp,jgp));
              REQUIRE(h_fvtheta(ie,igp,jgp,ilev)[ivec]==fvtheta_f90(ie,k,igp,jgp));

              for (int iq=0; iq<num_elems; ++iq) {
                REQUIRE(h_fq(ie,iq,igp,jgp,ilev)[ivec]==fq_f90(ie,iq,k,igp,jgp));
                REQUIRE(h_qdp(ie,np1_qdp,iq,igp,jgp,ilev)[ivec]==qdp_f90(ie,np1_qdp,iq,k,igp,jgp));
                REQUIRE(h_q(ie,iq,igp,jgp,ilev)[ivec]==q_f90(ie,iq,k,igp,jgp));
              }
            }
          }
        }
      }
    }
  }
}