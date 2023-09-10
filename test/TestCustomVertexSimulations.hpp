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

#ifndef TESTCUSTOMVERTEXSIMULATIONS_HPP_
#define TESTCUSTOMVERTEXSIMULATIONS_HPP_

// These first headers relate to the Chaste infrastructure and can be found at the top of every test suite
#include <cxxtest/TestSuite.h>
#include "AbstractCellBasedTestSuite.hpp"
#include "CheckpointArchiveTypes.hpp"

// Some utility headers that give us access to common Chaste objects and macros
#include "RandomNumberGenerator.hpp"
#include "SmartPointers.hpp"

// The mesh generator we will be using to provide the initial geometry of the vertex-based cell population
#include "VoronoiVertexMeshGenerator.hpp"

// Headers related to cells, cell types, and cell cycle models
#include "Cell.hpp"
#include "CellsGenerator.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "LabelDependentBernoulliTrialCellCycleModel.hpp"
#include "NoCellCycleModel.hpp"
#include "TransitCellProliferativeType.hpp"

// Headers related to the cell population, including writers and division rules
#include "CellLabel.hpp"
#include "CellLabelWriter.hpp"
#include "CellVolumesWriter.hpp"
#include "HeterotypicBoundaryLengthWriter.hpp"
#include "VertexBasedCellPopulation.hpp"
#include "VonMisesVertexBasedDivisionRule.hpp"

// Headers relating to the simulation itself, including force laws
#include "FarhadifarForce.hpp"
#include "NagaiHondaDifferentialAdhesionForce.hpp"
#include "OffLatticeSimulation.hpp"

// Custom headers from this user project
#include "SillyForce.hpp"
#include "SillySimulationModifier.hpp"
#include "SillyVertexBasedDivisionRule.hpp"

// Finally, we include a header that enforces running this test only on one process
#include "FakePetscSetup.hpp"

class TestCustomVertexSimulations : public AbstractCellBasedTestSuite
{
public:

    /**
     * We start with the simplest possible vertex simulation: a small population that relaxes towards equilibrium.
     * This is very close to a C++ equivalent of 1.1 from the Colab notebook.
     */
    void Test01Relaxation()
    {
        // The first step is to create a mesh. For this we can use one of several vertex mesh generators.
        // The mesh essentially represents the geometry in the simulation.
        RandomNumberGenerator::Instance()->Reseed(1); // This allows us to fix the randomness for reproducible simulations
        VoronoiVertexMeshGenerator generator(6, 6, 1); // Parameters are: cells across, cells up, and number of relaxation steps
        boost::shared_ptr<MutableVertexMesh<2, 2> > p_mesh = generator.GetMesh();
        p_mesh->SetDistanceForT3SwapChecking(1.0);

        // Next, we create some cells. These essentially represent the biology in the simulation.
        // Here, we choose to have cells that will not divide, and which have no cell cycle model.
        std::vector<CellPtr> cells;
        MAKE_PTR(DifferentiatedCellProliferativeType, p_cell_type);
        CellsGenerator<NoCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_cell_type);

        // Next, we create the cell population. This object can be modified in several ways, one of which is by adding
        // writers, which will add information to the files generated during the simulation.
        VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.AddCellWriter<CellVolumesWriter>();

        // Then, we create the simulation object, with which we can specify details about the time stepping and the
        // output directory...
        OffLatticeSimulation<2> simulation(cell_population);
        simulation.SetOutputDirectory("Pratical01Relaxation");
        simulation.SetEndTime(100.0);
        simulation.SetDt(0.01);
        simulation.SetSamplingTimestepMultiple(10);

        // ... and also the mechanics such as forces, which may be driven by the geometry or by the biology.
        MAKE_PTR(FarhadifarForce<2>, p_force);
        p_force->SetAreaElasticityParameter(1.0);
        p_force->SetPerimeterContractilityParameter(0.04);
        p_force->SetLineTensionParameter(0.12);
        p_force->SetBoundaryLineTensionParameter(0.12);

        simulation.AddForce(p_force);

        // Finally, we solve the simulation.
        simulation.Solve();
    }

    /**
     * Next, we add cell division to an initially identical configuration to above.
     * This is very close to a C++ equivalent of 2.1 from the Colab notebook.
     */
    void Test02OrientedCellDivision()
    {
        RandomNumberGenerator::Instance()->Reseed(1);
        VoronoiVertexMeshGenerator generator(6, 6, 1); // Parameters are: cells across, cells up, and number of relaxation steps
        boost::shared_ptr<MutableVertexMesh<2, 2> > p_mesh = generator.GetMesh();

        // The first change from the previous simulation is that the cells have a different type, and they have a
        // different cell cycle model. These changes allow the cells to divide.
        std::vector<CellPtr> cells;
        MAKE_PTR(TransitCellProliferativeType, p_cell_type);
        CellsGenerator<LabelDependentBernoulliTrialCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_cell_type);

        VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.AddCellWriter<CellVolumesWriter>();

        // The next change is to add a division rule, which governs precisely what happens when a cell divides in two.
        // Go back to the Colab notebook for a full description of the Von Mises division rule.
        MAKE_PTR(VonMisesVertexBasedDivisionRule<2>, p_division_rule);
        p_division_rule->SetMeanParameter(1.57);
        p_division_rule->SetConcentrationParameter(1.0);
        cell_population.SetVertexBasedDivisionRule(p_division_rule);

        OffLatticeSimulation<2> simulation(cell_population);
        simulation.SetOutputDirectory("Pratical02OrientedCellDivision");
        simulation.SetEndTime(50.0);
        simulation.SetDt(0.01);

        simulation.SetSamplingTimestepMultiple(25);

        MAKE_PTR(FarhadifarForce<2>, p_force);
        simulation.AddForce(p_force);

        simulation.Solve();
    }

    /**
     * Third up, we run a cell sorting simulation.
     * This is very close to a C++ equivalent of 3.1 from the Colab notebook.
     */
    void Test03CellSorting()
    {
        RandomNumberGenerator::Instance()->Reseed(1);
        VoronoiVertexMeshGenerator generator(9, 9, 1); // Parameters are: cells across, cells up, and number of relaxation steps
        boost::shared_ptr<MutableVertexMesh<2, 2> > p_mesh = generator.GetMesh();
        p_mesh->SetDistanceForT3SwapChecking(1.0);

        std::vector<CellPtr> cells;
        MAKE_PTR(DifferentiatedCellProliferativeType, p_cell_type);
        CellsGenerator<NoCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_cell_type);

        // This block demonstrates how we can use a 'cell property' to randomly label approximately half the cells.
        MAKE_PTR(CellLabel, p_cell_label);
        for (auto& p_cell : cells)
        {
            if (RandomNumberGenerator::Instance()->ranf() < 0.5)
            {
                p_cell->AddCellProperty(p_cell_label);
            }
        }

        // We make sure to add relevant writers, including one that writes the labels, and one that writes the
        // summary statistic relevant to cell sorting, the 'heteroptypic length'.
        VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.AddCellWriter<CellVolumesWriter>();
        cell_population.AddCellWriter<CellLabelWriter>();
        cell_population.AddPopulationWriter<HeterotypicBoundaryLengthWriter>();

        OffLatticeSimulation<2> simulation(cell_population);
        simulation.SetOutputDirectory("Pratical03CellSorting");
        simulation.SetEndTime(20.0);
        simulation.SetDt(0.01);

        simulation.SetSamplingTimestepMultiple(10);

        // We choose a different force here; again, see the Colab notebook for full details.
        MAKE_PTR(NagaiHondaDifferentialAdhesionForce<2>, p_force);
        p_force->SetNagaiHondaDeformationEnergyParameter(55.0);
        p_force->SetNagaiHondaMembraneSurfaceEnergyParameter(0.0);
        p_force->SetNagaiHondaCellCellAdhesionEnergyParameter(1.0);
        p_force->SetNagaiHondaLabelledCellCellAdhesionEnergyParameter(6.0);
        p_force->SetNagaiHondaLabelledCellLabelledCellAdhesionEnergyParameter(3.0);
        p_force->SetNagaiHondaCellBoundaryAdhesionEnergyParameter(12.0);
        p_force->SetNagaiHondaLabelledCellBoundaryAdhesionEnergyParameter(40.0);
        simulation.AddForce(p_force);

        simulation.Solve();
    }

    /**
     * Now we get into custom classes, and we're in the territory of things that it would not be possible to do with
     * the current Python interface.
     *
     * This simulation is similar to Test02, but with a custom division rule.
     */
    void Test04CustomDivisionRule()
    {
        RandomNumberGenerator::Instance()->Reseed(1);
        VoronoiVertexMeshGenerator generator(6, 6, 1); // Parameters are: cells across, cells up, and number of relaxation steps
        boost::shared_ptr<MutableVertexMesh<2, 2> > p_mesh = generator.GetMesh();
        p_mesh->SetDistanceForT3SwapChecking(1.0);

        std::vector<CellPtr> cells;
        MAKE_PTR(TransitCellProliferativeType, p_cell_type);
        CellsGenerator<LabelDependentBernoulliTrialCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_cell_type);

        // As an aside, here's an example of how we can tweak parameters in the cell cycle model.
        for (auto& p_cell : cells)
        {
            dynamic_cast<LabelDependentBernoulliTrialCellCycleModel*>(p_cell->GetCellCycleModel())->SetDivisionProbability(0.05);
        }

        VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.AddCellWriter<CellVolumesWriter>();

        // Here we make use of our own custom class, found in IntroToChasteCpp/src/SillyVertexBasedDivisionRule.*pp
        MAKE_PTR(SillyVertexBasedDivisionRule<2>, p_division_rule);
        p_division_rule->SetPeriod(100.0);
        cell_population.SetVertexBasedDivisionRule(p_division_rule);

        OffLatticeSimulation<2> simulation(cell_population);
        simulation.SetOutputDirectory("Pratical04CustomDivisionRule");
        simulation.SetEndTime(50.0);
        simulation.SetDt(0.01);

        simulation.SetSamplingTimestepMultiple(25);

        MAKE_PTR(FarhadifarForce<2>, p_force);
        simulation.AddForce(p_force);

        simulation.Solve();
    }

    /**
     * Next up we use a custom force.
     *
     * This simulation is similar to Test01, but with a custom force.
     */
    void Test05CustomForce()
    {
        RandomNumberGenerator::Instance()->Reseed(1);
        VoronoiVertexMeshGenerator generator(9, 9, 2); // Parameters are: cells across, cells up, and number of relaxation steps
        boost::shared_ptr<MutableVertexMesh<2, 2> > p_mesh = generator.GetMesh();
        p_mesh->SetDistanceForT3SwapChecking(1.0);

        std::vector<CellPtr> cells;
        MAKE_PTR(DifferentiatedCellProliferativeType, p_cell_type);
        CellsGenerator<NoCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_cell_type);

        VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.AddCellWriter<CellVolumesWriter>();

        OffLatticeSimulation<2> simulation(cell_population);
        simulation.SetOutputDirectory("Pratical05CustomForce");
        simulation.SetEndTime(100.0);
        simulation.SetDt(0.01);

        simulation.SetSamplingTimestepMultiple(50);

        // Note that we can add as many forces as we like. Each is independently calculated and together they are
        // cumulative.
        MAKE_PTR(FarhadifarForce<2>, p_farhadifar_force);
        MAKE_PTR(SillyForce<2>, p_silly_force);
        p_silly_force->SetStrengthMultiplier(0.15);

        simulation.AddForce(p_farhadifar_force);
        simulation.AddForce(p_silly_force);

        simulation.Solve();
    }

    /**
     * Finally, the most drastic custom class: a simulation modifier. This gives us the total freedom to reach into the
     * cell population and the mesh and perform arbitrary changes. This allows us to hijack the geometry and biology in
     * order to make changes.
     *
     * Simulation modifiers should be used when you need to make a change that cannot sensibly be modelled by a more
     * specific custom class, but gives almost total freedom to influence simulations.
     *
     * This simulation is similar to Test03, but with a custom simulation modifier.
     */
    void Test06CustomSimulationModifier()
    {
        RandomNumberGenerator::Instance()->Reseed(2);
        VoronoiVertexMeshGenerator generator(9, 9, 1); // Parameters are: cells across, cells up, and number of relaxation steps
        boost::shared_ptr<MutableVertexMesh<2, 2> > p_mesh = generator.GetMesh();
        p_mesh->SetDistanceForT3SwapChecking(1.0);

        std::vector<CellPtr> cells;
        MAKE_PTR(DifferentiatedCellProliferativeType, p_cell_type);
        CellsGenerator<NoCellCycleModel, 2> cells_generator;
        cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_cell_type);

        MAKE_PTR(CellLabel, p_cell_label);
        for (auto& p_cell : cells)
        {
            if (RandomNumberGenerator::Instance()->ranf() < 0.5)
            {
                p_cell->AddCellProperty(p_cell_label);
            }
        }

        VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);
        cell_population.AddCellWriter<CellVolumesWriter>();
        cell_population.AddCellWriter<CellLabelWriter>();
        cell_population.AddPopulationWriter<HeterotypicBoundaryLengthWriter>();

        OffLatticeSimulation<2> simulation(cell_population);
        simulation.SetOutputDirectory("Pratical06CustomSimulationModifier");
        simulation.SetEndTime(49.9);
        simulation.SetDt(0.01);

        simulation.SetSamplingTimestepMultiple(50);

        MAKE_PTR(NagaiHondaDifferentialAdhesionForce<2>, p_force);
        p_force->SetNagaiHondaDeformationEnergyParameter(55.0);
        p_force->SetNagaiHondaMembraneSurfaceEnergyParameter(0.0);
        p_force->SetNagaiHondaCellCellAdhesionEnergyParameter(1.0);
        p_force->SetNagaiHondaLabelledCellCellAdhesionEnergyParameter(6.0);
        p_force->SetNagaiHondaLabelledCellLabelledCellAdhesionEnergyParameter(3.0);
        p_force->SetNagaiHondaCellBoundaryAdhesionEnergyParameter(12.0);
        p_force->SetNagaiHondaLabelledCellBoundaryAdhesionEnergyParameter(40.0);
        simulation.AddForce(p_force);

        // We add the simulation modifier here.
        MAKE_PTR(SillySimulationModifier<2>, p_sim_modifier);
        simulation.AddSimulationModifier(p_sim_modifier);

        simulation.Solve();
    }
};

#endif /* TESTCUSTOMVERTEXSIMULATIONS_HPP_ */
