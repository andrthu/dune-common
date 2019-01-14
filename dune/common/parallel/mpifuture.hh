// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_COMMON_PARALLEL_MPIFUTURE_HH
#define DUNE_COMMON_PARALLEL_MPIFUTURE_HH

#include "future.hh"
#include "mpidata.hh"

#if HAVE_MPI
namespace Dune{

  /*! \brief Provides a future-like object for MPI communication.  It contains
    the object that will be received and might contain also a sending object,
    which must be hold (keep alive) until the communication has been completed.
   */
  template<class R, class S = void>
  class MPIFuture{
    template<class T>
    friend class when_all_MPIFuture;

    template<class T>
    friend class when_any_MPIFuture;
    mutable MPI_Request req_;
    mutable MPI_Status status_;
    std::unique_ptr<MPIData<R>> data_;
    std::unique_ptr<MPIData<S>> send_data_;
    bool valid_;
    friend class Communication<MPI_Comm>;
  public:
    MPIFuture(bool valid = false) :
      req_(MPI_REQUEST_NULL),
      valid_(valid)
    {}

    // Hide this constructor if R or S is void
    template<class V = R, class U = S>
    MPIFuture(V&& recv_data, U&& send_data, typename std::enable_if_t<!std::is_void<V>::value && !std::is_void<U>::value>* = 0) :
      req_(MPI_REQUEST_NULL)
      , data_(std::make_unique<MPIData<R>>(getMPIData(std::forward<R>(recv_data))))
      , send_data_(std::make_unique<MPIData<S>>(getMPIData(std::forward<S>(send_data))))
      , valid_(true)
    {}

    // hide this constructor if R is void
    template<class V = R>
    MPIFuture(V&& recv_data, typename std::enable_if_t<!std::is_void<V>::value>* = 0) :
      req_(MPI_REQUEST_NULL)
      , data_(std::make_unique<MPIData<R>>(getMPIData(std::forward<R>(recv_data))))
      , valid_(true)
    {}

    ~MPIFuture() {
      if(req_ != MPI_REQUEST_NULL){
        try{ // might fail when it is a collective communication
          MPI_Cancel(&req_);
          MPI_Request_free(&req_);
        }catch(...){
        }
      }
    }

    MPIFuture(MPIFuture&& f)
      : req_(MPI_REQUEST_NULL)
      , valid_(false)
    {
      std::swap(req_, f.req_);
      std::swap(status_, f.status_);
      std::swap(data_, f.data_);
      std::swap(send_data_, f.send_data_);
      std::swap(valid_, f.valid_);
    }

    MPIFuture& operator=(MPIFuture&& f){
      std::swap(req_, f.req_);
      std::swap(status_, f.status_);
      std::swap(data_, f.data_);
      std::swap(send_data_, f.send_data_);
      std::swap(valid_, f.valid_);
      return *this;
    }

    bool valid() const{
      return valid_;
    }

    void wait(){
      if(!valid_)
        DUNE_THROW(InvalidFutureException, "The MPIFuture is not valid!");
      MPI_Wait(&req_, &status_);
    }

    bool ready() const{
      int flag = -1;
      MPI_Test(&req_, &flag, &status_);
      return flag;
    }

    R get() {
      if(!valid_)
        DUNE_THROW(InvalidFutureException, "The MPIFuture is not valid!");
      wait();
      valid_ = false;
      return data_->get();
    }

    S get_send_data(){
      wait();
      return send_data_.get();
    }
  };
}
#endif
#endif
