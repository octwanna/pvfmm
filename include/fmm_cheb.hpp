/**
 * \file fmm_cheb.hpp
 * \author Dhairya Malhotra, dhairya.malhotra88@gmail.com
 * \date 3-07-2011
 * \brief This file contains the definition of the FMM_Cheb class.
 * This handles all the translations through matrix multiplications.
 */

#ifndef _FMM_CHEB_HPP_
#define _FMM_CHEB_HPP_

#include <pvfmm_common.hpp>
#include <mpi.h>
#include <matrix.hpp>
#include <precomp_mat.hpp>
#include <cheb_utils.hpp>
#include <cheb_node.hpp>
#include <fmm_pts.hpp>

template <class FMMNode>
class FMM_Cheb: public FMM_Pts<FMMNode>{

 public:

  typedef typename FMMNode::Real_t Real_t;
  typedef FMMNode FMMNode_t;

  /**
   * \brief This class contains FMM specific data that each node contains
   * along with the functions functions for manipulating the data.
   */
  class FMMData: public FMM_Pts<FMMNode>::FMMData{

   public:

    virtual FMM_Data<Real_t>* NewData(){ return new FMMData;}

    //FMM specific node data.
    Vector<Real_t> cheb_out;
  };

  /**
   * \brief Constructor.
   */
  FMM_Cheb(){};

  /**
   * \brief Virtual destructor.
   */
  virtual ~FMM_Cheb();

  /**
   * \brief Initialize all the translation matrices (or load from file).
   * \param[in] mult_order Order of multipole expansion.
   * \param[in] cheb_deg Degree of Chebyshev polynomials.
   * \param[in] kernel Kernel functions and related data.
   */
  void Initialize(int mult_order, int cheb_deg, const MPI_Comm& comm, const Kernel<Real_t>* kernel, const Kernel<Real_t>* aux_kernel=NULL);

  /**
   * \brief Number of source points per box (or the parameter describing the
   * order of approximation of source distribution).
   */
  int& ChebDeg(){return cheb_deg;}

  virtual void CollectNodeData(std::vector<FMMNode*>& all_nodes, std::vector<Matrix<Real_t> >& buff, std::vector<Vector<FMMNode_t*> >& n_list, std::vector<size_t> extra_size = std::vector<size_t>(0));

  /**
   * \brief Initialize multipole expansions for the given array of leaf nodes
   * at a given level.
   */
  virtual void InitMultipole(FMMNode**, size_t n, int level);

  /**
   * \brief Compute X-List intractions.
   */
  virtual void X_ListSetup(SetupData<Real_t>& setup_data, std::vector<Matrix<Real_t> >& node_data, std::vector<Vector<FMMNode_t*> >& n_list, int level, bool device);
  virtual void X_List     (SetupData<Real_t>& setup_data, bool device=false);

  /**
   * \brief Compute target potential from the local expansion.
   */
  virtual void Down2TargetSetup(SetupData<Real_t>& setup_data, std::vector<Matrix<Real_t> >& node_data, std::vector<Vector<FMMNode_t*> >& n_list, int level, bool device);
  virtual void Down2Target     (SetupData<Real_t>& setup_data, bool device=false);

  /**
   * \brief Compute W-List intractions.
   */
  virtual void W_ListSetup(SetupData<Real_t>& setup_data, std::vector<Matrix<Real_t> >& node_data, std::vector<Vector<FMMNode_t*> >& n_list, int level, bool device);
  virtual void W_List     (SetupData<Real_t>& setup_data, bool device=false);

  /**
   * \brief Compute U-List intractions.
   */
  virtual void U_ListSetup(SetupData<Real_t>& setup_data, std::vector<Matrix<Real_t> >& node_data, std::vector<Vector<FMMNode_t*> >& n_list, int level, bool device);
  virtual void U_List     (SetupData<Real_t>& setup_data, bool device=false);

  virtual void PostProcessing(std::vector<FMMNode_t*>& nodes);

  /**
   * \brief For each node, copy FMM output from FMM_Data to the node.
   */
  virtual void CopyOutput(FMMNode** nodes, size_t n){
    for(size_t i=0;i<n;i++){
      nodes[i]->DataDOF()=this->kernel.ker_dim[1];
      if(nodes[i]->IsLeaf() && !nodes[i]->IsGhost()){
        Vector<Real_t>& cheb_data=nodes[i]->ChebData();
        Vector<Real_t>& cheb_out =((FMMData*)nodes[i]->FMMData())->cheb_out;
        if(cheb_data.Dim()!=cheb_out.Dim()) cheb_data.ReInit(0);
        cheb_data = cheb_out;
      }
    }
    FMM_Pts<FMMNode>::CopyOutput(nodes,n);
  }


 protected:

  virtual Permutation<Real_t>& PrecompPerm(Mat_Type type, Perm_Type perm_indx);

  virtual Matrix<Real_t>& Precomp(int level, Mat_Type type, size_t mat_indx);

 private:

  int cheb_deg;            //Order of Cheb. approx.

};

#include <fmm_cheb.txx>

#endif

