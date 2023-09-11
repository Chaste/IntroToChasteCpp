# Introduction to the Chaste C++ interface

This repository was created for the [2023 Chaste workshop in Oxford](https://chaste.github.io/workshops/2023-09-11/).
It introduces some simplex examples of vertex model simulations, and then examples of how simulations can be customised by writing new C++ classes.

## Structure

The simulations are found in the following test suite:
- [test/TestCustomVertexSimulations.hpp](./test/TestCustomVertexSimulations.hpp)

The custom Chaste classes are defined in the following files:
- [src/SillyVertexBasedDivisionRule.hpp](./src/SillyVertexBasedDivisionRule.hpp)
- [src/SillyForce.hpp](./src/SillyForce.hpp)
- [src/SillySimulationModifier.hpp](./src/SillySimulationModifier.hpp)

## Chaste user projects

* There are a few ways to use Chaste's source code. Professional C++ developers may wish to link to Chaste as an external C++ library rather than use the User Project framework described below. People new to C++ may be tempted to directly alter code in the Chaste source folders; this should generally be avoided as we won't know whether any problems you may run into are down to Chaste or your changes to it!

* Instead of these options, 'User Projects' allow you to use Chaste source code and have the benefit of using the Chaste build/testing framework, by putting User Projects under the projects folder. User Projects work exactly like new Chaste modules (`global`, `heart`, `cell_based`, etc.) and can depend on any of the Chaste modules (or indeed other User Projects). We tend to supply User Projects to accompany and reproduce research articles.

* Instructions on how to create your own User Project from a 'template' project can also be found [here](https://chaste.cs.ox.ac.uk/trac/wiki/ChasteGuides/UserProjects). Alternatively, if you're using Chaste via Docker, you can run the provided script `new_project.sh` (in the `scripts` directory) and pass the name that you want to call your project e.g. `new_project.sh my_chaste_proj`.

* ⚠️ When you create a new user proejct, make sure you run the [setup_project.py](https://github.com/Chaste/template_project/blob/main/setup_project.py) script to ensure it relies on the correct Chaste libraries.

* Once you have created the repository on GitHub (with the same name) and committed some changes, use the following commands to push your project to GitHub:
  ```
  git remote add origin https://github.com/<username>/my_chaste_proj.git
  git push -u origin main
  ```

* If the above command gives you the error `fatal: remote origin already exists`, then instead use the commands:
  ```
  git remote set-url origin https://github.com/<username>/my_chaste_proj.git
  git push -u origin main
  ```

* Your project and its revision history is now safely stored on GitHub.

## Practical session

### 1. User project structure

- Look through the structure of the user project and make sure you understand what all the files are there for.
- Go through the steps to create a new user project, making sure you have run the [setup_project.py](https://github.com/Chaste/template_project/blob/main/setup_project.py) script.

### 2. Run the test suite

- Read through the code in [test/TestCustomVertexSimulations.hpp](./test/TestCustomVertexSimulations.hpp) and make sure you undersand what it is doing.
- Compile and run the test suite in [test/TestCustomVertexSimulations.hpp](./test/TestCustomVertexSimulations.hpp).
- Open the output in ParaView and see what each simulation has produced.

### 3. Read though the custom classes

- Look at the code in the [src](./src) directory and make sure you understand it.
- Make some modifications to one or more of those classes, and see how that modification affects the output of the relevant test.

### 4. Make a custom class

- Create a cusom class. This might be a division rule, force, or simulation modifier similar to the existing custom classes, or, it could be something different such as a new cell writer.

### 5. Write a node based simulation with a custom class

- Take one of the simulations in the test suite in [test/TestCustomVertexSimulations.hpp](./test/TestCustomVertexSimulations.hpp), and re-write it as a node based simulation. You may find one of the tutorials useful: [https://chaste.cs.ox.ac.uk/trac/wiki/UserTutorials#Cell-basedChaste](https://chaste.cs.ox.ac.uk/trac/wiki/UserTutorials#Cell-basedChaste)
- Write a custom class that would make sense for a node based simulation, and add it to your new simulation.
