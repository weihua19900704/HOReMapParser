#include <iostream>
#include "HOReMap.h"

int main()
{  
  HOReMap myHOReMap;
  myHOReMap.addFile ("data/HOremap_Feb2015_crate3.txt", 3);
  myHOReMap.addFile ("data/HOremap_Feb2015_crate6.txt", 6);
  myHOReMap.addFile ("data/HOremap_Feb2015_crate7.txt", 7);
  myHOReMap.addFile ("data/HOremap_Feb2015_crate13.txt", 13);
  // myHOReMap.printHash();

  int htr       = myHOReMap.getHTR      ( "P2", "S01", 2, 6 );
  int fpga      = myHOReMap.getFPGA     ( "P2", "S01", 2, 6 );
  int htr_fiber = myHOReMap.getHTRFiber ( "P2", "S01", 2, 6 );
  bool is_hox   = myHOReMap.isHOX       ( "P2", "S01", 2, 6 );
  
  std::cout << "HTR  = " << htr << std::endl;
  std::cout << "FPGA = " << fpga << std::endl;
  std::cout << "htr_fiber = " << htr_fiber << std::endl;
  std::cout << "Is HOX? ";
  if ( is_hox ) std::cout << "YES!" << std::endl;
  else          std::cout << "NO!"  << std::endl;

}
