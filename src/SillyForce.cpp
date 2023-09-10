/*

Copyright (c) 2005-2023, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "SillyForce.hpp"

template <unsigned DIM>
SillyForce<DIM>::SillyForce()
        : AbstractForce<DIM>()
{
}

template <unsigned DIM>
void SillyForce<DIM>::AddForceContribution(AbstractCellPopulation<DIM>& rCellPopulation)
{
    // Throw an exception message if not using a VertexBasedCellPopulation
    if (dynamic_cast<VertexBasedCellPopulation<DIM>*>(&rCellPopulation) == nullptr)
    {
        EXCEPTION("SillyForce is to be used with a VertexBasedCellPopulation only");
    }

    const c_vector<double, DIM> centroid = rCellPopulation.GetCentroidOfCellPopulation();

    // Iterate over vertices in the cell population
    for (unsigned node_index = 0; node_index < rCellPopulation.GetNumNodes(); node_index++)
    {
        Node<DIM>* p_this_node = rCellPopulation.GetNode(node_index);

        c_vector<double, DIM> vec_from_centroid = rCellPopulation.rGetMesh().GetVectorFromAtoB(centroid, p_this_node->rGetLocation());

        c_vector<double, DIM> force_on_node = zero_vector<double>(DIM);
        if constexpr (DIM == 2)
        {
            force_on_node[0] = -vec_from_centroid[1];
            force_on_node[1] = vec_from_centroid[0];
        }

        p_this_node->AddAppliedForceContribution(force_on_node * mStrengthMultiplier);
    }
}

template <unsigned DIM>
double SillyForce<DIM>::GetStrengthMultiplier()
{
    return mStrengthMultiplier;
}

template <unsigned DIM>
void SillyForce<DIM>::SetStrengthMultiplier(double strengthMultiplier)
{
    mStrengthMultiplier = strengthMultiplier;
}

template <unsigned DIM>
void SillyForce<DIM>::OutputForceParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<StrengthMultiplier>" << mStrengthMultiplier << "</StrengthMultiplier>\n";

    // Call method on direct parent class
    AbstractForce<DIM>::OutputForceParameters(rParamsFile);
}

// Explicit instantiation
template class SillyForce<1>;
template class SillyForce<2>;
template class SillyForce<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(SillyForce)
