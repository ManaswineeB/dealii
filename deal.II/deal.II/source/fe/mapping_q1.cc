//---------------------------------------------------------------------------
//    $Id$
//    Version: $Name$
//
//    Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//---------------------------------------------------------------------------

#include <base/tensor.h>
#include <base/quadrature.h>
#include <base/qprojector.h>
#include <base/memory_consumption.h>
#include <lac/full_matrix.h>
#include <grid/tria.h>
#include <grid/tria_iterator.h>
#include <dofs/dof_accessor.h>
#include <fe/fe_values.h>
#include <fe/mapping_q1.h>

#include <cmath>
#include <algorithm>
#include <memory>

DEAL_II_NAMESPACE_OPEN



template <int dim>
const unsigned int MappingQ1<dim>::n_shape_functions;



template<int dim>
MappingQ1<dim>::InternalData::InternalData (const unsigned int n_shape_functions)
		:
		is_mapping_q1_data(true),
		n_shape_functions (n_shape_functions)
{}



template <int dim>
unsigned int
MappingQ1<dim>::InternalData::memory_consumption () const
{
  return (Mapping<dim>::InternalDataBase::memory_consumption() +
	  MemoryConsumption::memory_consumption (shape_values) +
	  MemoryConsumption::memory_consumption (shape_derivatives) +
	  MemoryConsumption::memory_consumption (covariant) +
	  MemoryConsumption::memory_consumption (contravariant) +
	  MemoryConsumption::memory_consumption (unit_tangentials) +
	  MemoryConsumption::memory_consumption (aux) +
	  MemoryConsumption::memory_consumption (mapping_support_points) +
	  MemoryConsumption::memory_consumption (cell_of_current_support_points) +
	  MemoryConsumption::memory_consumption (is_mapping_q1_data) +
	  MemoryConsumption::memory_consumption (n_shape_functions));
}



template <int dim>
MappingQ1<dim>::MappingQ1 ()
{}



template<int dim>
void
MappingQ1<dim>::compute_shapes (const std::vector<Point<dim> > &unit_points,
				InternalData &data) const
{
  if (data.is_mapping_q1_data)
    MappingQ1<dim>::compute_shapes_virtual(unit_points, data);
  else
    compute_shapes_virtual(unit_points, data);
}


#if (deal_II_dimension == 1)


template<>
void
MappingQ1<1>::compute_shapes_virtual (const std::vector<Point<1> > &unit_points,
				      InternalData& data) const
{
  const unsigned int n_points=unit_points.size();
  for (unsigned int k = 0 ; k < n_points ; ++k)
    {
      double x = unit_points[k](0);
      
      if (data.shape_values.size()!=0)
	{
	  Assert(data.shape_values.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.shape(k,0) = 1.-x;
	  data.shape(k,1) = x;
	}
      if (data.shape_derivatives.size()!=0)
	{
	  Assert(data.shape_derivatives.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.derivative(k,0)[0] = -1.;
	  data.derivative(k,1)[0] = 1.;
	}
      if (data.shape_second_derivatives.size()!=0)
	{
	  Assert(data.shape_second_derivatives.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.second_derivative(k,0)[0][0] = 0;
	  data.second_derivative(k,1)[0][0] = 0;
	}

    }
}

#endif

#if (deal_II_dimension == 2)


template<>
void
MappingQ1<2>::compute_shapes_virtual (const std::vector<Point<2> > &unit_points,
				      InternalData &data) const
{
  const unsigned int n_points=unit_points.size();
  for (unsigned int k = 0 ; k < n_points ; ++k)
    {
      double x = unit_points[k](0);
      double y = unit_points[k](1);
      
      if (data.shape_values.size()!=0)
	{
	  Assert(data.shape_values.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.shape(k,0) = (1.-x)*(1.-y);
	  data.shape(k,1) = x*(1.-y);
	  data.shape(k,2) = (1.-x)*y;
	  data.shape(k,3) = x*y;
	}
      if (data.shape_derivatives.size()!=0)
	{
	  Assert(data.shape_derivatives.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.derivative(k,0)[0] = (y-1.);
	  data.derivative(k,1)[0] = (1.-y);
	  data.derivative(k,2)[0] = -y;
	  data.derivative(k,3)[0] = y;
	  data.derivative(k,0)[1] = (x-1.);
	  data.derivative(k,1)[1] = -x;
	  data.derivative(k,2)[1] = (1.-x);
	  data.derivative(k,3)[1] = x;
	}
      if (data.shape_second_derivatives.size()!=0)
	{
	  Assert(data.shape_second_derivatives.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.second_derivative(k,0)[0][0] = 0;
	  data.second_derivative(k,1)[0][0] = 0;
	  data.second_derivative(k,2)[0][0] = 0;
	  data.second_derivative(k,3)[0][0] = 0;
	  data.second_derivative(k,0)[0][1] = 1.;
	  data.second_derivative(k,1)[0][1] = -1.;
	  data.second_derivative(k,2)[0][1] = -1.;
	  data.second_derivative(k,3)[0][1] = 1.;
	  data.second_derivative(k,0)[1][0] = 1.;
	  data.second_derivative(k,1)[1][0] = -1.;
	  data.second_derivative(k,2)[1][0] = -1.;
	  data.second_derivative(k,3)[1][0] = 1.;
	  data.second_derivative(k,0)[1][1] = 0;
	  data.second_derivative(k,1)[1][1] = 0;
	  data.second_derivative(k,2)[1][1] = 0;
	  data.second_derivative(k,3)[1][1] = 0;
	}
    }
}

#endif

#if (deal_II_dimension == 3)


template<>
void
MappingQ1<3>::compute_shapes_virtual (const std::vector<Point<3> > &unit_points,
				      InternalData &data) const
{
  const unsigned int n_points=unit_points.size();
  for (unsigned int k = 0 ; k < n_points ; ++k)
    {
      double x = unit_points[k](0);
      double y = unit_points[k](1);
      double z = unit_points[k](2);
      
      if (data.shape_values.size()!=0)
	{
	  Assert(data.shape_values.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.shape(k,0) = (1.-x)*(1.-y)*(1.-z);
	  data.shape(k,1) = x*(1.-y)*(1.-z);
	  data.shape(k,2) = (1.-x)*y*(1.-z);
	  data.shape(k,3) = x*y*(1.-z);
	  data.shape(k,4) = (1.-x)*(1.-y)*z;
	  data.shape(k,5) = x*(1.-y)*z;
	  data.shape(k,6) = (1.-x)*y*z;
	  data.shape(k,7) = x*y*z;
	}
      if (data.shape_derivatives.size()!=0)
	{
	  Assert(data.shape_derivatives.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.derivative(k,0)[0] = (y-1.)*(1.-z);
	  data.derivative(k,1)[0] = (1.-y)*(1.-z);
	  data.derivative(k,2)[0] = -y*(1.-z);
	  data.derivative(k,3)[0] = y*(1.-z);
	  data.derivative(k,4)[0] = (y-1.)*z;
	  data.derivative(k,5)[0] = (1.-y)*z;
	  data.derivative(k,6)[0] = -y*z;
	  data.derivative(k,7)[0] = y*z;
	  data.derivative(k,0)[1] = (x-1.)*(1.-z);
	  data.derivative(k,1)[1] = -x*(1.-z);
	  data.derivative(k,2)[1] = (1.-x)*(1.-z);
	  data.derivative(k,3)[1] = x*(1.-z);
	  data.derivative(k,4)[1] = (x-1.)*z;
	  data.derivative(k,5)[1] = -x*z;
	  data.derivative(k,6)[1] = (1.-x)*z;
	  data.derivative(k,7)[1] = x*z;
	  data.derivative(k,0)[2] = (x-1)*(1.-y);
	  data.derivative(k,1)[2] = x*(y-1.);
	  data.derivative(k,2)[2] = (x-1.)*y;
	  data.derivative(k,3)[2] = -x*y;
	  data.derivative(k,4)[2] = (1.-x)*(1.-y);
	  data.derivative(k,5)[2] = x*(1.-y);
	  data.derivative(k,6)[2] = (1.-x)*y;
	  data.derivative(k,7)[2] = x*y;
	}
      if (data.shape_second_derivatives.size()!=0)
	{
	  Assert(data.shape_second_derivatives.size()==n_shape_functions*n_points,
		 ExcInternalError());
	  data.second_derivative(k,0)[0][0] = 0;
	  data.second_derivative(k,1)[0][0] = 0;
	  data.second_derivative(k,2)[0][0] = 0;
	  data.second_derivative(k,3)[0][0] = 0;
	  data.second_derivative(k,4)[0][0] = 0;
	  data.second_derivative(k,5)[0][0] = 0;
	  data.second_derivative(k,6)[0][0] = 0;
	  data.second_derivative(k,7)[0][0] = 0;
	  data.second_derivative(k,0)[1][1] = 0;
	  data.second_derivative(k,1)[1][1] = 0;
	  data.second_derivative(k,2)[1][1] = 0;
	  data.second_derivative(k,3)[1][1] = 0;
	  data.second_derivative(k,4)[1][1] = 0;
	  data.second_derivative(k,5)[1][1] = 0;
	  data.second_derivative(k,6)[1][1] = 0;
	  data.second_derivative(k,7)[1][1] = 0;
	  data.second_derivative(k,0)[2][2] = 0;
	  data.second_derivative(k,1)[2][2] = 0;
	  data.second_derivative(k,2)[2][2] = 0;
	  data.second_derivative(k,3)[2][2] = 0;
	  data.second_derivative(k,4)[2][2] = 0;
	  data.second_derivative(k,5)[2][2] = 0;
	  data.second_derivative(k,6)[2][2] = 0;
	  data.second_derivative(k,7)[2][2] = 0;

	  data.second_derivative(k,0)[0][1] = (1.-z);
	  data.second_derivative(k,1)[0][1] = -(1.-z);
	  data.second_derivative(k,2)[0][1] = -(1.-z);
	  data.second_derivative(k,3)[0][1] = (1.-z);
	  data.second_derivative(k,4)[0][1] = z;
	  data.second_derivative(k,5)[0][1] = -z;
	  data.second_derivative(k,6)[0][1] = -z;
	  data.second_derivative(k,7)[0][1] = z;
	  data.second_derivative(k,0)[1][0] = (1.-z);
	  data.second_derivative(k,1)[1][0] = -(1.-z);
	  data.second_derivative(k,2)[1][0] = -(1.-z);
	  data.second_derivative(k,3)[1][0] = (1.-z);
	  data.second_derivative(k,4)[1][0] = z;
	  data.second_derivative(k,5)[1][0] = -z;
	  data.second_derivative(k,6)[1][0] = -z;
	  data.second_derivative(k,7)[1][0] = z;

	  data.second_derivative(k,0)[0][2] = (1.-y);
	  data.second_derivative(k,1)[0][2] = -(1.-y);
	  data.second_derivative(k,2)[0][2] = y;
	  data.second_derivative(k,3)[0][2] = -y;
	  data.second_derivative(k,4)[0][2] = -(1.-y);
	  data.second_derivative(k,5)[0][2] = (1.-y);
	  data.second_derivative(k,6)[0][2] = -y;
	  data.second_derivative(k,7)[0][2] = y;
	  data.second_derivative(k,0)[2][0] = (1.-y);
	  data.second_derivative(k,1)[2][0] = -(1.-y);
	  data.second_derivative(k,2)[2][0] = y;
	  data.second_derivative(k,3)[2][0] = -y;
	  data.second_derivative(k,4)[2][0] = -(1.-y);
	  data.second_derivative(k,5)[2][0] = (1.-y);
	  data.second_derivative(k,6)[2][0] = -y;
	  data.second_derivative(k,7)[2][0] = y;
	  
	  data.second_derivative(k,0)[1][2] = (1.-x);
	  data.second_derivative(k,1)[1][2] = x;
	  data.second_derivative(k,2)[1][2] = -(1.-x);
	  data.second_derivative(k,3)[1][2] = -x;
	  data.second_derivative(k,4)[1][2] = -(1.-x);
	  data.second_derivative(k,5)[1][2] = -x;
	  data.second_derivative(k,6)[1][2] = (1.-x);
	  data.second_derivative(k,7)[1][2] = x;
	  data.second_derivative(k,0)[2][1] = (1.-x);
	  data.second_derivative(k,1)[2][1] = x;
	  data.second_derivative(k,2)[2][1] = -(1.-x);
	  data.second_derivative(k,3)[2][1] = -x;
	  data.second_derivative(k,4)[2][1] = -(1.-x);
	  data.second_derivative(k,5)[2][1] = -x;
	  data.second_derivative(k,6)[2][1] = (1.-x);
	  data.second_derivative(k,7)[2][1] = x;
	}
    }
}

#endif

template <int dim>
UpdateFlags
MappingQ1<dim>::update_once (const UpdateFlags in) const
{
  UpdateFlags out = UpdateFlags(in & (update_transformation_values
				      | update_transformation_gradients));

				   // Shape function values
  if (in & update_quadrature_points)
    out |= update_transformation_values;

				   // Shape function gradients
  if (in & (update_covariant_transformation
	    | update_contravariant_transformation
	    | update_JxW_values
	    | update_boundary_forms
	    | update_normal_vectors
	    | update_jacobians
	    | update_jacobian_grads
	    | update_inverse_jacobians))
    out |= update_transformation_gradients;

  return out;
}



template <int dim>
UpdateFlags
MappingQ1<dim>::update_each (const UpdateFlags in) const
{
				   // Select flags of concern for the
				   // transformation.
  UpdateFlags out = UpdateFlags(in & (update_quadrature_points
				      | update_covariant_transformation
				      | update_contravariant_transformation
				      | update_JxW_values
				      | update_cell_JxW_values
				      | update_boundary_forms
				      | update_normal_vectors
				      | update_jacobians
				      | update_jacobian_grads
				      | update_inverse_jacobians));

				   // add a few flags. note that some
				   // flags appear in both conditions
				   // and in subsequents set
				   // operations. this leads to some
				   // circular logic. the only way to
				   // treat this is to iterate. since
				   // there are 4 if-clauses in the
				   // loop, it will take at most 3
				   // iterations to converge. do them:
  for (unsigned int i=0; i<4; ++i)
    {
				       // The following is a little incorrect:
				       // If not applied on a face,
				       // update_boundary_forms does not
				       // make sense. On the other hand,
				       // it is necessary on a
				       // face. Currently,
				       // update_boundary_forms is simply
				       // ignored for the interior of a
				       // cell.
      if (out & (update_JxW_values
		 | update_normal_vectors))
	out |= update_boundary_forms;
  
      if (out & (update_covariant_transformation
		 | update_JxW_values
		 | update_jacobians
		 | update_jacobian_grads
		 | update_boundary_forms
		 | update_normal_vectors))
	out |= update_contravariant_transformation;
      
      if (out & (update_inverse_jacobians))
	out |= update_covariant_transformation;

				       // The contravariant transformation
				       // is a Piola transformation, which
				       // requires the determinant of the
				       // Jacobi matrix of the transformation.
				       // Therefore these values have to
				       // updated for each cell.
      if (out & update_contravariant_transformation)
	out |= update_JxW_values | update_cell_JxW_values;
    }
  
  return out;
}


template <int dim>
void
MappingQ1<dim>::compute_data (const UpdateFlags      update_flags,
			      const Quadrature<dim> &q,
			      const unsigned int     n_original_q_points,
			      InternalData          &data) const
{
  const unsigned int n_q_points = q.size();

  data.update_once = update_once(update_flags);
  data.update_each = update_each(update_flags);
  data.update_flags = data.update_once | data.update_each;

  const UpdateFlags flags(data.update_flags);
  
  if (flags & update_transformation_values)
    data.shape_values.resize(data.n_shape_functions * n_q_points);

  if (flags & update_transformation_gradients)
    data.shape_derivatives.resize(data.n_shape_functions * n_q_points);

  if (flags & update_covariant_transformation)
    data.covariant.resize(n_original_q_points);

  if (flags & update_contravariant_transformation)
    data.contravariant.resize(n_original_q_points);

  if (flags & update_jacobian_grads)
    data.shape_second_derivatives.resize(data.n_shape_functions * n_q_points);
  
  compute_shapes (q.get_points(), data);
}



template <int dim>
typename Mapping<dim>::InternalDataBase *
MappingQ1<dim>::get_data (const UpdateFlags update_flags,
			  const Quadrature<dim>& q) const
{
  InternalData* data = new InternalData(n_shape_functions);
  compute_data (update_flags, q, q.size(), *data);
  return data;
}


#if deal_II_dimension == 1

template <>
void
MappingQ1<1>::compute_face_data (const UpdateFlags update_flags,
                                 const Quadrature<1>& q,
                                 const unsigned int n_original_q_points,
                                 InternalData& data) const
{
  compute_data (update_flags, q, n_original_q_points, data);
}

#else

template <int dim>
void
MappingQ1<dim>::compute_face_data (const UpdateFlags update_flags,
				   const Quadrature<dim>& q,
				   const unsigned int n_original_q_points,
				   InternalData& data) const
{
  compute_data (update_flags, q, n_original_q_points, data);

  if (data.update_flags & update_boundary_forms)
    {
      data.aux.resize (dim-1, std::vector<Tensor<1,dim> > (n_original_q_points));
      
				       // Compute tangentials to the
				       // unit cell.
      const unsigned int nfaces = GeometryInfo<dim>::faces_per_cell;
      data.unit_tangentials.resize (nfaces*(dim-1),
                                    std::vector<Tensor<1,dim> > (n_original_q_points));
      if (dim==2)
	{
					   // ensure a counterclock wise
					   // orientation of tangentials
	  static const int tangential_orientation[4]={-1,1,1,-1};
	  for (unsigned int i=0; i<nfaces; ++i)
	    {
	      Tensor<1,dim> tang;	      
	      tang[1-i/2]=tangential_orientation[i];
	      std::fill (data.unit_tangentials[i].begin(),
			 data.unit_tangentials[i].end(), tang);
	    }
	}
      else if (dim==3)
	{
	  for (unsigned int i=0; i<nfaces; ++i)
	    {
	      Tensor<1,dim> tang1, tang2;

	      const unsigned int nd=
		GeometryInfo<dim>::unit_normal_direction[i];
	      
					       // first tangential
					       // vector in direction
					       // of the (nd+1)%3 axis
					       // and inverted in case
					       // of unit inward normal
	      tang1[(nd+1)%dim]=GeometryInfo<dim>::unit_normal_orientation[i];
					       // second tangential
					       // vector in direction
					       // of the (nd+2)%3 axis
	      tang2[(nd+2)%dim]=1.;

					       // same unit tangents
					       // for all quadrature
					       // points on this face
	      std::fill (data.unit_tangentials[i].begin(),
			 data.unit_tangentials[i].end(), tang1);
	      std::fill (data.unit_tangentials[nfaces+i].begin(),
			 data.unit_tangentials[nfaces+i].end(), tang2);
	    }
	}
    }
}

#endif

  

template <int dim>
typename Mapping<dim>::InternalDataBase *
MappingQ1<dim>::get_face_data (const UpdateFlags        update_flags,
			       const Quadrature<dim-1> &quadrature) const
{
  InternalData* data = new InternalData(n_shape_functions);
  compute_face_data (update_flags,
		     QProjector<dim>::project_to_all_faces(quadrature),
		     quadrature.size(),
		     *data);

  return data;
}



template <int dim>
typename Mapping<dim>::InternalDataBase *
MappingQ1<dim>::get_subface_data (const UpdateFlags update_flags,
				  const Quadrature<dim-1>& quadrature) const
{
  InternalData* data = new InternalData(n_shape_functions);
  compute_face_data (update_flags,
		     QProjector<dim>::project_to_all_subfaces(quadrature),
		     quadrature.size(),
		     *data);

  return data;
}




template <int dim>
void
MappingQ1<dim>::compute_fill (const typename Triangulation<dim>::cell_iterator &cell,
			      const unsigned int       n_q_points,
			      const DataSetDescriptor  data_set,
			      InternalData            &data,
			      std::vector<Point<dim> > &quadrature_points) const
{
  const UpdateFlags update_flags(data.current_update_flags());

  if (update_flags & update_quadrature_points)
    {
      Assert (quadrature_points.size() == n_q_points,
	      ExcDimensionMismatch(quadrature_points.size(), n_q_points));
      std::fill(quadrature_points.begin(),
		quadrature_points.end(),
		Point<dim>());
    }

  if (update_flags & update_covariant_transformation)
    {
      Assert (data.covariant.size() == n_q_points,
	      ExcDimensionMismatch(data.covariant.size(), n_q_points));
    }
  
  if (update_flags & update_contravariant_transformation)
    {
      Assert (data.contravariant.size() == n_q_points,
	      ExcDimensionMismatch(data.contravariant.size(), n_q_points));
      std::fill(data.contravariant.begin(),
		data.contravariant.end(),
		Tensor<2,dim>());
    }

				   // if necessary, recompute the
				   // support points of the
				   // transformation of this cell
				   // (note that we need to first
				   // check the triangulation pointer,
				   // since otherwise the second test
				   // might trigger an exception if
				   // the triangulations are not the
				   // same)
  if ((data.mapping_support_points.size() == 0)
      ||
      (&cell->get_triangulation() !=
       &data.cell_of_current_support_points->get_triangulation())
      ||
      (cell != data.cell_of_current_support_points))
    {
      compute_mapping_support_points(cell, data.mapping_support_points);
      data.cell_of_current_support_points = cell;
    }

                                   // first compute quadrature points
  if (update_flags & update_quadrature_points)
    for (unsigned int point=0; point<n_q_points; ++point)
      for (unsigned int k=0; k<data.n_shape_functions; ++k)
        quadrature_points[point]
          += data.shape(point+data_set,k)
             * data.mapping_support_points[k];
  
                                   // then Jacobians
  if (update_flags & update_contravariant_transformation)
    for (unsigned int point=0; point<n_q_points; ++point)
      for (unsigned int k=0; k<data.n_shape_functions; ++k)
        for (unsigned int i=0; i<dim; ++i)
          for (unsigned int j=0; j<dim; ++j)
	    data.contravariant[point][i][j]
	      += (data.derivative(point+data_set, k)[j]
		  *
		  data.mapping_support_points[k][i]);

				       // invert contravariant for
				       // covariant transformation
				       // matrices
  if (update_flags & update_covariant_transformation)
    for (unsigned int point=0; point<n_q_points; ++point)
      data.covariant[point] = invert(data.contravariant[point]);
}


template <int dim>
void
MappingQ1<dim>::compute_mapping_support_points(
  const typename Triangulation<dim>::cell_iterator &cell,
  std::vector<Point<dim> > &a) const
{
  a.resize(GeometryInfo<dim>::vertices_per_cell);

  for (unsigned int i=0; i<GeometryInfo<dim>::vertices_per_cell; ++i)
    a[i] = cell->vertex(i);
}



template <int dim>
void
MappingQ1<dim>::fill_fe_values (const typename Triangulation<dim>::cell_iterator &cell,
				const Quadrature<dim>                     &q,
				typename Mapping<dim>::InternalDataBase   &mapping_data,
				std::vector<Point<dim> >                  &quadrature_points,
				std::vector<double>                       &JxW_values,
				std::vector<Tensor<2,dim> >               &jacobians,
				std::vector<Tensor<3,dim> >               &jacobian_grads,
				std::vector<Tensor<2,dim> >               &inverse_jacobians) const
{
  InternalData *data_ptr = dynamic_cast<InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  InternalData &data=*data_ptr;

  const unsigned int n_q_points=q.size();
  
  compute_fill (cell, n_q_points, DataSetDescriptor::cell (),
                data, quadrature_points);

  
  const UpdateFlags update_flags(data.current_update_flags());
  const std::vector<double> &weights=q.get_weights();

				   // Multiply quadrature weights
				   // by Jacobian determinants
  if (update_flags & update_JxW_values)
    {      
      Assert (JxW_values.size() == n_q_points,
	      ExcDimensionMismatch(JxW_values.size(), n_q_points));
      for (unsigned int point=0; point<n_q_points; ++point)
	JxW_values[point]
	  = determinant(data.contravariant[point])*weights[point];
    }

				   // copy values from InternalData to vector
				   // given by reference
  if (update_flags & update_jacobians)
    {      
      Assert (jacobians.size() == n_q_points,
	      ExcDimensionMismatch(jacobians.size(), n_q_points));
      for (unsigned int point=0; point<n_q_points; ++point)
	jacobians[point]
	  = data.contravariant[point];
    }
				   // calculate values of the derivatives of the
				   // Jacobians. do it here, since we only do it
				   // for cells, not faces.
  if (update_flags & update_jacobian_grads)
    {
      Assert (jacobian_grads.size() == n_q_points,
	      ExcDimensionMismatch(jacobian_grads.size(), n_q_points));
      std::fill(jacobian_grads.begin(),
		jacobian_grads.end(),
		Tensor<3,dim>());

      for (unsigned int point=0; point<n_q_points; ++point)
	for (unsigned int k=0; k<data.n_shape_functions; ++k)
	  for (unsigned int i=0; i<dim; ++i)
	    for (unsigned int j=0; j<dim; ++j)
	      for (unsigned int l=0; l<dim; ++l)
		jacobian_grads[point][i][j][l]
		  += (data.second_derivative(point+DataSetDescriptor::cell (), k)[j][l]
		      *
		      data.mapping_support_points[k][i]);
    }
				   // copy values from InternalData to vector
				   // given by reference
  if (update_flags & update_inverse_jacobians)
    {      
      Assert (inverse_jacobians.size() == n_q_points,
	      ExcDimensionMismatch(inverse_jacobians.size(), n_q_points));
      for (unsigned int point=0; point<n_q_points; ++point)
	inverse_jacobians[point]
	  = transpose(data.covariant[point]);
    }
}



template <int dim>
void
MappingQ1<dim>::compute_fill_face (const typename Triangulation<dim>::cell_iterator &cell,
				   const unsigned int      face_no,
				   const unsigned int      subface_no,
				   const unsigned int      n_q_points,
				   const DataSetDescriptor data_set,
				   const std::vector<double>   &weights,
				   InternalData           &data,
				   std::vector<Point<dim> >    &quadrature_points,
				   std::vector<double>         &JxW_values,
				   std::vector<Tensor<1,dim> > &boundary_forms,
				   std::vector<Point<dim> >    &normal_vectors,
				   std::vector<double>         &cell_JxW_values) const
{
  compute_fill (cell, n_q_points, data_set, data, quadrature_points);

  const UpdateFlags update_flags(data.current_update_flags());
  
  if (update_flags & update_boundary_forms)
    {
      Assert (boundary_forms.size()==n_q_points,
	      ExcDimensionMismatch(boundary_forms.size(), n_q_points));
      if (update_flags & update_normal_vectors)
	Assert (normal_vectors.size()==n_q_points,
		ExcDimensionMismatch(normal_vectors.size(), n_q_points));
      if (update_flags & update_JxW_values)
	Assert (JxW_values.size() == n_q_points,
		ExcDimensionMismatch(JxW_values.size(), n_q_points));
      
      
      Assert (data.aux[0].size() <= data.unit_tangentials[face_no].size(),
	      ExcInternalError());
      transform_contravariant(data.unit_tangentials[face_no], 0,
                              data.aux[0],
			      data);

      typename std::vector<Tensor<1,dim> >::iterator
	result = boundary_forms.begin();
      const typename std::vector<Tensor<1,dim> >::iterator
	end = boundary_forms.end();
      
      switch (dim)
	{
	  case 2:
          {
            for (unsigned int i=0; result != end; ++result, ++i)
              cross_product (*result, data.aux[0][i]);
            break;
          };

	  case 3:
          {
            Assert (face_no+GeometryInfo<dim>::faces_per_cell <
                    data.unit_tangentials.size(),
                    ExcInternalError());
            Assert (data.aux[1].size() <=
                    data.unit_tangentials[face_no+GeometryInfo<dim>::faces_per_cell].size(),
                    ExcInternalError());
            transform_contravariant(data.unit_tangentials[
                                      face_no+GeometryInfo<dim>::faces_per_cell], 0,
                                    data.aux[1],
                                    data);
            for (unsigned int i=0; result != end; ++result, ++i)
              cross_product (*result, data.aux[0][i], data.aux[1][i]);

            break;
          };

	  default:
                Assert(false, ExcNotImplemented());
	}
      
      if (update_flags & (update_normal_vectors
			  | update_JxW_values))
	for (unsigned int i=0;i<boundary_forms.size();++i)
	  {
	    const double f = std::sqrt(contract(boundary_forms[i],
                                                boundary_forms[i]));
	    if (update_flags & update_JxW_values)
	      {
		JxW_values[i] = f * weights[i];
		if (subface_no!=deal_II_numbers::invalid_unsigned_int)
		  {
		    const double area_ratio=GeometryInfo<dim>::subface_ratio(
		      cell->subface_case(face_no), subface_no);
		    JxW_values[i] *= area_ratio;
		  }
	      }
	    if (update_flags & update_normal_vectors)
              normal_vectors[i] = boundary_forms[i] / f;
	  }
    }

  // If the Piola transformation is to be used, the
  // new flag, update_cell_JxW_values has to be set.
  // It triggers the creation of values for the determinant
  // of the mapping function on 
  if (update_flags & update_cell_JxW_values)
    {      
      Assert (cell_JxW_values.size() == n_q_points,
	      ExcDimensionMismatch(cell_JxW_values.size(), n_q_points));
      Assert (data.contravariant.size() == n_q_points,
	      ExcDimensionMismatch(data.contravariant.size(), n_q_points));
      Assert (weights.size() == n_q_points,
	      ExcDimensionMismatch(weights.size(), n_q_points));
      for (unsigned int point=0; point<n_q_points; ++point)
	cell_JxW_values[point]
	  = determinant(data.contravariant[point])*weights[point];
    }
}


template <int dim>
void
MappingQ1<dim>::fill_fe_face_values (const typename Triangulation<dim>::cell_iterator &cell,
				     const unsigned int       face_no,
				     const Quadrature<dim-1> &q,
				     typename Mapping<dim>::InternalDataBase &mapping_data,
				     std::vector<Point<dim> >     &quadrature_points,
				     std::vector<double>          &JxW_values,
				     std::vector<Tensor<1,dim> >  &boundary_forms,
				     std::vector<Point<dim> >     &normal_vectors,
				     std::vector<double>          &cell_JxW_values) const
{
  InternalData *data_ptr = dynamic_cast<InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  InternalData &data=*data_ptr;

  const unsigned int n_q_points = q.size();
  
  compute_fill_face (cell, face_no, deal_II_numbers::invalid_unsigned_int,
		     n_q_points,
		     DataSetDescriptor::face (face_no,
                                              cell->face_orientation(face_no),
                                              cell->face_flip(face_no),
                                              cell->face_rotation(face_no),
                                              n_q_points),
		     q.get_weights(),
		     data,
		     quadrature_points,
		     JxW_values,
		     boundary_forms,
		     normal_vectors,
		     cell_JxW_values);
}


template <int dim>
void
MappingQ1<dim>::fill_fe_subface_values (const typename Triangulation<dim>::cell_iterator &cell,
					const unsigned int       face_no,
					const unsigned int       sub_no,
					const Quadrature<dim-1> &q,
					typename Mapping<dim>::InternalDataBase &mapping_data,
					std::vector<Point<dim> >     &quadrature_points,
					std::vector<double>          &JxW_values,
					std::vector<Tensor<1,dim> >  &boundary_forms,
					std::vector<Point<dim> >     &normal_vectors,
					std::vector<double>          &cell_JxW_values) const
{
  InternalData *data_ptr = dynamic_cast<InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  InternalData &data=*data_ptr;

  const unsigned int n_q_points = q.size();
  
  compute_fill_face (cell, face_no, sub_no,
		     n_q_points,
		     DataSetDescriptor::subface (face_no, sub_no,
						 cell->face_orientation(face_no),
						 cell->face_flip(face_no),
						 cell->face_rotation(face_no),
						 n_q_points,
						 cell->subface_case(face_no)),
		     q.get_weights(),
		     data,
		     quadrature_points,
		     JxW_values,
		     boundary_forms,
		     normal_vectors,
		     cell_JxW_values);
}


#if (deal_II_dimension == 1)

template <>
void
MappingQ1<1>::compute_fill_face (const Triangulation<1>::cell_iterator &,
				 const unsigned int,
				 const unsigned int,
				 const unsigned int,
				 const DataSetDescriptor,
				 const std::vector<double> &,
				 InternalData &,
				 std::vector<Point<1> > &,
				 std::vector<double> &,
				 std::vector<Tensor<1,1> > &,
				 std::vector<Point<1> > &,
				 std::vector<double>          &/*cell_JxW_values*/) const
{
  Assert(false, ExcNotImplemented());
}


template <>
void
MappingQ1<1>::fill_fe_face_values (const Triangulation<1>::cell_iterator &,
				   const unsigned,
				   const Quadrature<0>&,
				   Mapping<1>::InternalDataBase&,
				   std::vector<Point<1> >&,
				   std::vector<double>&,
				   std::vector<Tensor<1,1> >&,
				   std::vector<Point<1> >&,
				   std::vector<double>          &/*cell_JxW_values*/) const
{
  Assert(false, ExcNotImplemented());
}


template <>
void
MappingQ1<1>::fill_fe_subface_values (const Triangulation<1>::cell_iterator &,
				      const unsigned,
				      const unsigned,
				      const Quadrature<0>&,
				      Mapping<1>::InternalDataBase&,
				      std::vector<Point<1> >&,
				      std::vector<double>&,
				      std::vector<Tensor<1,1> >&,
				      std::vector<Point<1> >&,
				      std::vector<double>          &/*cell_JxW_values*/) const
{
  Assert(false, ExcNotImplemented());
}
#endif


template <int dim>
void
MappingQ1<dim>::transform_covariant (
  const VectorSlice<const std::vector<Tensor<1,dim> > > input,
  const unsigned int                 offset,
  VectorSlice<std::vector<Tensor<1,dim> > > output,
  const typename Mapping<dim>::InternalDataBase &mapping_data) const
{
  const InternalData *data_ptr = dynamic_cast<const InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  const InternalData &data=*data_ptr;

  Assert (data.update_flags & update_covariant_transformation,
	  typename FEValuesBase<dim>::ExcAccessToUninitializedField());

  Assert (output.size() + offset <= input.size(), ExcInternalError());

  for (unsigned int i=0; i<output.size(); ++i)
    contract (output[i], input[i+offset], data.covariant[i]);
}



template <int dim>
void
MappingQ1<dim>::transform_covariant (
  const VectorSlice<const std::vector<Tensor<2,dim> > > input,
  const unsigned int                 offset,
  VectorSlice<std::vector<Tensor<2,dim> > > output,
  const typename Mapping<dim>::InternalDataBase &mapping_data) const
{
  const InternalData *data_ptr = dynamic_cast<const InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  const InternalData &data=*data_ptr;

  Assert (data.update_flags & update_covariant_transformation,
	  typename FEValuesBase<dim>::ExcAccessToUninitializedField());

  Assert (output.size() + offset <= input.size(), ExcInternalError());

  for (unsigned int i=0; i<output.size(); ++i)
    contract (output[i], input[i+offset], data.covariant[i]);
}



template <int dim>
void
MappingQ1<dim>::
transform_contravariant (
  const VectorSlice<const std::vector<Tensor<1,dim> > > input,
  const unsigned int                 offset,
  VectorSlice<std::vector<Tensor<1,dim> > > output,
  const typename Mapping<dim>::InternalDataBase &mapping_data) const
{
  const InternalData* data_ptr = dynamic_cast<const InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  const InternalData &data=*data_ptr;

  Assert (data.update_flags & update_contravariant_transformation,
	  typename FEValuesBase<dim>::ExcAccessToUninitializedField());

  Assert (output.size() + offset <= input.size(), ExcInternalError());

  for (unsigned int i=0; i<output.size(); ++i)
    contract (output[i], data.contravariant[i], input[i+offset]);
}



template <int dim>
void
MappingQ1<dim>::transform_contravariant (
  const VectorSlice<const std::vector<Tensor<2,dim> > > input,
  const unsigned int                 offset,
  VectorSlice<std::vector<Tensor<2,dim> > > output,
  const typename Mapping<dim>::InternalDataBase &mapping_data) const
{
  const InternalData* data_ptr = dynamic_cast<const InternalData *> (&mapping_data);
  Assert(data_ptr!=0, ExcInternalError());
  const InternalData &data=*data_ptr;

  Assert (data.update_flags & update_contravariant_transformation,
	  typename FEValuesBase<dim>::ExcAccessToUninitializedField());

  Assert (output.size() + offset <= input.size(), ExcInternalError());

  for (unsigned int i=0; i<output.size(); ++i)
    contract (output[i], data.contravariant[i], input[i+offset]);
}



template <int dim>
Point<dim>
MappingQ1<dim>::
transform_unit_to_real_cell (const typename Triangulation<dim>::cell_iterator &cell,
                             const Point<dim>                                 &p) const
{
				   // Use the get_data function to
				   // create an InternalData with data
				   // vectors of the right size and
				   // transformation shape values
				   // already computed at point p.
  const Quadrature<dim> point_quadrature(p);

  std::auto_ptr<InternalData>
    mdata (dynamic_cast<InternalData *> (
             get_data(update_transformation_values, point_quadrature)));

				   // compute the mapping support
				   // points
  compute_mapping_support_points(cell, mdata->mapping_support_points);

  return transform_unit_to_real_cell_internal(*mdata);
}



template <int dim>
Point<dim>
MappingQ1<dim>::
transform_unit_to_real_cell_internal (const InternalData &data) const
{
  const unsigned int n_mapping_points=data.mapping_support_points.size();
  Assert(data.shape_values.size()==n_mapping_points, ExcInternalError());
  
				   // use now the InternalData to
				   // compute the point in real space.
  Point<dim> p_real;
  for (unsigned int i=0; i<data.mapping_support_points.size(); ++i)
    p_real += data.mapping_support_points[i] * data.shape(0,i);

  return p_real;
}



template <int dim>
Point<dim>
MappingQ1<dim>::
transform_real_to_unit_cell (const typename Triangulation<dim>::cell_iterator &cell,
                             const Point<dim>                                 &p) const
{
				   // Let the start value of the
				   // newton iteration be the center
				   // of the unit cell
  Point<dim> p_unit;
  for (unsigned int i=0; i<dim; ++i)
    p_unit(i)=0.5;

				   // Use the get_data function to
				   // create an InternalData with data
				   // vectors of the right size and
				   // transformation shape values and
				   // derivatives already computed at
				   // point p_unit.
  const Quadrature<dim> point_quadrature(p_unit);
  std::auto_ptr<InternalData>
    mdata(dynamic_cast<InternalData *> (
            MappingQ1<dim>::get_data(update_transformation_values
                                     | update_transformation_gradients,
                                     point_quadrature)));

  MappingQ1<dim>::compute_mapping_support_points (cell,
                                                  mdata->mapping_support_points);
  Assert(mdata->mapping_support_points.size() ==
         GeometryInfo<dim>::vertices_per_cell,
	 ExcInternalError());

				   // perform the newton iteration.
  transform_real_to_unit_cell_internal(cell, p, *mdata, p_unit);
  
  return p_unit;
}



template <int dim>
void
MappingQ1<dim>::
transform_real_to_unit_cell_internal
(const typename Triangulation<dim>::cell_iterator &cell,
 const Point<dim>                                 &p,
 InternalData                                     &mdata,
 Point<dim>                                       &p_unit) const
{
  const unsigned int n_shapes=mdata.shape_values.size();
  Assert(n_shapes!=0, ExcInternalError());
  Assert(mdata.shape_derivatives.size()==n_shapes, ExcInternalError());
  
  std::vector<Point<dim> > &points=mdata.mapping_support_points;
  Assert(points.size()==n_shapes, ExcInternalError());
  
				   // Newton iteration to solve
				   // f(x)=p(x)-p=0
				   // x_{n+1}=x_n-[f'(x)]^{-1}f(x)
  
				   // The start value is set to be the
				   // center of the unit cell.
  
				   // The shape values and derivatives
				   // of the mapping at this point are
				   // previously computed.

  
				   // f(x)
  Point<dim> p_real(transform_unit_to_real_cell_internal(mdata));
  Point<dim> f = p_real-p;

  const double eps=1e-15*cell->diameter();
  unsigned int loop=0;
  while (f.square()>eps*eps && loop++<10)
    {      
				       // f'(x)
      Tensor<2,dim> df;
      for (unsigned int k=0; k<mdata.n_shape_functions; ++k)
	{
	  const Tensor<1,dim> &grad_transform=mdata.derivative(0,k);
	  const Point<dim> &point=points[k];
	  
	  for (unsigned int i=0; i<dim; ++i)
	    for (unsigned int j=0; j<dim; ++j)
	      df[i][j]+=point[i]*grad_transform[j];
	}
      
				       // Solve  [f'(x)]d=f(x)
      Tensor<1,dim> d;
      Tensor<2,dim> df_1;

      df_1 = invert(df);
      contract (d, df_1, static_cast<const Tensor<1,dim>&>(f));

				       // update of p_unit
      p_unit -= d;
				       // shape values and derivatives
				       // at new p_unit point
      compute_shapes(std::vector<Point<dim> > (1, p_unit), mdata);
      
				       // f(x)
      p_real = transform_unit_to_real_cell_internal(mdata);
      f = p_real-p;
    }
}



template <int dim>
Mapping<dim> *
MappingQ1<dim>::clone () const
{
  return new MappingQ1<dim>(*this);
}

//---------------------------------------------------------------------------


template <int dim> MappingQ1<dim> StaticMappingQ1<dim>::mapping;



//---------------------------------------------------------------------------

template class MappingQ1<deal_II_dimension>;
template struct StaticMappingQ1<deal_II_dimension>;



DEAL_II_NAMESPACE_CLOSE
