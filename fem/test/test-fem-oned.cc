// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// $Id$

#include <config.h>

#include <iostream>

#include <dune/grid/onedgrid.hh>

#include "femcheck.cc"

int main () {
  try {

    Dune::SimpleVector<double> coords(6);
    coords[0] = -1;
    coords[1] = -0.4;
    coords[2] = 0.1;
    coords[3] = 0.35;
    coords[4] = 0.38;
    coords[5] = 1;

    // extra-environment to check destruction
    {
      std::cout << std::endl << "OneDGrid<1,1>" << std::endl << std::endl;
      Dune::OneDGrid<1,1> grid(coords);
      femCheck(grid);
    };

  } catch (Dune::Exception &e) {
    std::cerr << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }

  return 0;
};
