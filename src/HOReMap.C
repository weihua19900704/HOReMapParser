#include "HOReMap.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// Constructor
HOReMap::HOReMap():
  m_max_hash ( 50000 ),
  m_hash_table(new std::vector<box> ( m_max_hash, box())),
  m_raw_data_n_columns ( 45 )
{}

HOReMap::~HOReMap(){
  if ( m_hash_table ) delete m_hash_table;
}

// Get values

int HOReMap::getHTR( const int & ring, const int & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSlot();
}

int HOReMap::getHTR( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSlot();
}

int HOReMap::getHTRFiber( const int & ring, const int & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getHTRFiber();
}

int HOReMap::getHTRFiber( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getHTRFiber();
}


int HOReMap::getFPGA( const int & ring, const int & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide();
}

int HOReMap::getFPGA( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide();
}


bool HOReMap::isHOX( const int & ring, const int & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].isHOX();
}

bool HOReMap::isHOX( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib ){
  if ( (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].getSide() == box::NONE ) {
    std::cout << "ERROR: INVALID COORDINATES!"  << std::endl;
    return -1;
  }
  return (*m_hash_table)[box_tools::getHash ( ring, sector, rm, rm_fib )].isHOX();
}


// Add a new 

// Process the data from the file (from constructor)

void HOReMap::addFile(const char* file_path, const int & crate_number )
{
  std::ifstream file (file_path);
  std::string line, entry;
  std::vector<std::string> tmp_v;
  std::vector<std::vector<std::string> > raw_data;
  while ( getline ( file, line ) )
  {
    // Clear white space from the end of the line
    tools::rtrim( line );
    tmp_v.clear();
    std::stringstream line_ss ( line );
    while ( std::getline ( line_ss, entry, '\t' ))
      tmp_v.push_back ( entry ) ;
    // Only save the vector if it has the right number of entries
    if ( tmp_v.size() != m_raw_data_n_columns ) continue;
    raw_data.push_back ( tmp_v );
  }  
  processRawData ( raw_data, crate_number );
}

// Make boxes from raw data

void HOReMap::processRawData( const std::vector<std::vector<std::string> > & raw_data, const int & crate_number ){

  const int n_small_rows_per_big_row = 4;
  const int n_small_rows = raw_data.size();
  const int n_big_rows = n_small_rows / n_small_rows_per_big_row;
  const int n_block_per_row = 9;

  for (int i_big_row = 0; i_big_row < n_big_rows; ++i_big_row){
    
    bool is_top ( i_big_row % 2 == 0 );
    box::box_side side = is_top ? box::TOP : box::BOTTOM;

    // "row" refers to the top row of a 4-row "big row"
    int row = i_big_row * n_small_rows_per_big_row;
    int ho_slot = is_top ? std::atoi(raw_data[row][0].c_str()) : std::atoi(raw_data[row-4][0].c_str());

    int n_columns = raw_data[row].size();
    for (int i_column = 0; i_column < n_columns; ++i_column){
      
      // If this is a bottom big-row, and four rows above is a number, that's a new slot
      if ( (!is_top) && tools::is_number ( raw_data[row-4][i_column]))
	ho_slot = std::atoi(raw_data[row-4][i_column].c_str());

      // Skip empty columns, but iterat
      if ( raw_data[row][i_column].empty() ) continue;

      // If this is a number, it describes the slot for HO boxes (not HOX boxes)
      // Take note of the slot and then move to the next column
      if ( ( is_top && tools::is_number ( raw_data[row][i_column]))){
	ho_slot = is_top ? std::atoi ( raw_data[row][i_column].c_str() ) :std::atoi ( raw_data[row-4][i_column].c_str() );
	continue;
      }

      // Block
      int block = 1 + 9 * (i_big_row / 3) + (i_column / 5);
      
      // This is a box.  Is it an HO box or an HOX box?
      // The third row of an HOX box has a "/" in it.
      // The third row of an HO box does not.
      
      bool is_hox ( raw_data[row + 2][i_column].find('/') != std::string::npos );
      
      // If this is an HOX box, use the HOX class to add an entry to the hash table

      if ( is_hox ) { 
	hox_box box ( raw_data, side, row, i_column);
	box.setBlock (block);
	box.setCrate (crate_number);
	(*m_hash_table)[box.getHash()] = box;
      }

      // If this is an HO box, use the HO class to add an entry to the hash table

      else { 
	ho_box box ( raw_data, side, row, i_column);
	box.setSlot(ho_slot);
	box.setBlock (block);
	box.setCrate(crate_number);
	(*m_hash_table)[box.getHash()] = box;
      }
    }
  }
}

void HOReMap::printHash()
{  
  for (int i = 0; i < m_max_hash; ++i)
  {
    if ( (*m_hash_table)[i].getSide() == box::NONE ) continue;
    int hash   = i;
    int rm_fib = (hash / 1    ) % 10;
    int rm     = (hash / 10   ) % 10;
    int sector = (hash / 100  ) % 100;
    int ring   = (hash / 10000) - 2;
    std::string fpga = (*m_hash_table)[i].getSide() == box::TOP ? std::string("top") : std::string ("bot");
    std::string type = (*m_hash_table)[i].isHOX() ? std::string ("HOX") : std::string ("HO");

    std::cout 
      << "hash = " << hash << "\t"
      << "ring = " << ring << "\t"
      << "sector = " << sector << "\t"
      << "rm = " << rm << "\t"
      << "rm_fib = " << rm_fib << "\t"
      << "slot = " << (*m_hash_table)[i].getSlot() << "\t"
      << "fpga = " << fpga << "\t"
      << "htr_fiber = " << (*m_hash_table)[i].getHTRFiber() << "\t" 
      << "crate = " << (*m_hash_table)[i].getCrate() << "\t"
      << "block = " << (*m_hash_table)[i].getBlock() << "\t"
      << "coupler = " << (*m_hash_table)[i].getCoupler() << "\t"
      << "type = " << type << std::endl;
  }
}

//class box
HOReMap::box::box():
  m_side(box::NONE),
  m_row(-1),
  m_column(-1),
  m_str_ring(""),
  m_str_sector(""),
  m_str_rm_rm_fib(""),
  m_int_slot(-1)
{}

HOReMap::box::box( const std::vector<std::vector<std::string> > & data,
                   const box_side & side, const int& row  , const int & column ):
  m_side          ( side   ),
  m_row           ( row    ),
  m_column        ( column ),
  m_str_ring      ( data[row  ][column] ),
  m_str_sector    ( data[row+1][column] ),
  m_str_rm_rm_fib ( data[row+3][column] )
{
  std::vector<std::string> entries;
  tools::split(m_str_rm_rm_fib, entries, '/');

  if      ( m_str_ring.compare ("M2" ) == 0 ) m_int_ring = -2;
  else if ( m_str_ring.compare ("M1" ) == 0 ) m_int_ring = -1;
  else if ( m_str_ring.compare ("YB0") == 0 ) m_int_ring =  0;
  else if ( m_str_ring.compare ("P1" ) == 0 ) m_int_ring =  1;
  else if ( m_str_ring.compare ("P2" ) == 0 ) m_int_ring =  2;

  m_int_sector  = std::stoi(m_str_sector.substr( m_str_sector.find_first_of("S", 0) + 1, m_str_sector.size()));
  m_int_rm      = std::stoi ( entries[0] );
  m_int_rm_fib  = std::stoi ( entries[1] );
  m_int_coupler = (column % 5) + (((row/4)% 3) * 4);

}

int HOReMap::box::getHash()
{
  return box_tools::getHash ( m_int_ring, m_int_sector, m_int_rm, m_int_rm_fib );
}

//class ho_box
HOReMap::ho_box::ho_box():
  box(),
  m_str_phi (""),
  m_str_htr_fiber("")
{}

HOReMap::ho_box::ho_box( const std::vector<std::vector<std::string> > & data,
                         const box_side & side,  const int& row  , const int & column ):
  box ( data, side , row, column ),
  m_str_phi  ( data[row+2][column] ),
  m_str_htr_fiber( data[data.size() - 1][column])
{
  std::vector<std::string> entries;
  tools::split(m_str_phi, entries, '-');

  m_int_phi_min   = std::stoi ( entries[0] );
  m_int_phi_max   = std::stoi ( entries[1] );
  m_int_htr_fiber = std::stoi(m_str_htr_fiber);
  m_is_hox = false;

}

//class hox_box
HOReMap::hox_box::hox_box( const std::vector<std::vector<std::string> > & data,
                           const box_side & side, const int& row  , const int & column ):
  box ( data, side, row, column ),
  m_str_crate_slot_htr_fib ( data[row+2][column] )
{
  std::vector<std::string> entries;
  tools::split(m_str_crate_slot_htr_fib, entries, '/');

  m_int_crate     = std::stoi ( entries[0] );
  m_int_slot      = std::stoi ( entries[1] );
  m_int_htr_fiber = std::stoi ( entries[2] );
  m_is_hox = true;
}



// namepspace tools
void tools::split ( const std::string & content,
                    std::vector<std::string> & output,
                    const char & delimiter ) 
{
  output.clear();
  std::istringstream ss (content);
  std::string entry;
  while ( std::getline(ss, entry, delimiter ) )
    output.push_back ( entry );
}

// trim from end
void tools::rtrim(std::string &content)
{
  content.erase(std::find_if(content.rbegin(), content.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), content.end());
}

// Is a string a number?
bool tools::is_number(const std::string& content) 
{
  std::string::const_iterator it = content.begin();
  while (it != content.end() && std::isdigit(*it)) ++it;
  return !content.empty() && it == content.end();
}

// namepspace box_tools
int box_tools::getHash( const int & ring,
                  const int & sector,
                  const int & rm,
                  const int & rm_fib ){

  int hash = 0;
  hash += ((ring + 2) * 10000);
  hash += (sector     * 100  );
  hash += (rm         * 10   );
  hash += (rm_fib     * 1    );

  return hash;
}


int box_tools::getHash( const std::string & ring,
                  const std::string & sector,
                  const int & rm,
                  const int & rm_fib )
{
  int i_sector = std::stoi(sector.substr( sector.find_first_of("S", 0) + 1, sector.size()));
  int i_ring = -100;

  if      ( ring.compare ("M2" ) == 0 ) i_ring = -2;
  else if ( ring.compare ("M1" ) == 0 ) i_ring = -1;
  else if ( ring.compare ("YB0") == 0 ) i_ring =  0;
  else if ( ring.compare ("P1" ) == 0 ) i_ring =  1;
  else if ( ring.compare ("P2" ) == 0 ) i_ring =  2;

  int hash = 0;
  hash += ((i_ring + 2) * 10000);
  hash += (i_sector     * 100  );
  hash += (rm           * 10   );
  hash += (rm_fib       * 1    );

  return hash;
}



