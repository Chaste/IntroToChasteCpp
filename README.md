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
* Once you have created the repository on GitHub (with the same name) and committed some changes, use the following commands to push your project to GitHub:
```
git remote add origin https://github.com/<username>/my_chaste_proj.git
git push -u origin master
```
* If the above command gives you the error `fatal: remote origin already exists`, then instead use the commands:
```
git remote set-url origin https://github.com/<username>/my_chaste_proj.git
git push -u origin master
```
* Your project and its revision history is now safely stored on GitHub.
