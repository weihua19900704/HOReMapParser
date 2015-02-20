#ifndef HOREMAP_H
#define HOREMAP_H

#include <vector>
#include <string>

class HOReMap 
{   
 public:

  // Constructor + destructor
  HOReMap ();
  ~HOReMap();

  // Build the HOReMap
  void addFile(const char* file_path, const int & crate_number);

  // Print the hash 
  void printHash();
  
  // Getter functions

  int getHTR     ( const int &         ring, const int &         sector, const int & rm, const int & rm_fib );
  int getHTR     ( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib );
  
  int getHTRFiber( const int &         ring, const int &         sector, const int & rm, const int & rm_fib );
  int getHTRFiber( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib );

  int getFPGA    ( const int &         ring, const int &         sector, const int & rm, const int & rm_fib );
  int getFPGA    ( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib );

  bool isHOX     ( const int &         ring, const int &         sector, const int & rm, const int & rm_fib );
  bool isHOX     ( const std::string & ring, const std::string & sector, const int & rm, const int & rm_fib );
  
 private:

  class box 
  {
   public:
    // Define sides
    enum box_side { NONE, TOP, BOTTOM };

    // Constructors
    box();
    box( const std::vector<std::vector<std::string> > & data,
         const box_side & side, const int & row , const int  & column );
    // Getter functions 
    int      getRing()      { return m_int_ring;       }
    int      getSector()    { return m_int_sector;     }
    int      getRM()        { return m_int_rm;         }
    int      getRMFib()     { return m_int_rm_fib;     }
    box_side getSide()      { return m_side;           }
    int      getSlot()      { return m_int_slot;       }
    int      getHTRFiber()  { return m_int_htr_fiber;  }
    bool     isHOX()        { return m_is_hox;         }
    int      getCoupler()   { return m_int_coupler;    }
    int      getBlock()     { return m_int_block;      }
    int      getCrate()     { return m_int_crate;      }

    // Setter functions
    void     setBlock(int b){ m_int_block = b;         }
    void     setCrate(int c){ m_int_crate = c;         }

    // Hash
    int getHash();

   protected:
    // Raw integers from constructor
    box_side m_side;
    int      m_row;
    int      m_column;

    // Strings taken from raw data
    std::string m_str_ring;
    std::string m_str_sector;
    std::string m_str_rm_rm_fib;

    // Processed integers derived from strings
    int m_int_slot;
    int m_int_ring;
    int m_int_sector;
    int m_int_rm;
    int m_int_rm_fib;
    int m_int_sign;
    int m_int_htr_fiber;
    int m_int_crate;
    int m_int_block;
    int m_int_coupler;

    // Is this HOX?
    bool m_is_hox;
  };


  class ho_box : public box 
  {
   public:
    // constructors
    ho_box ( const std::vector<std::vector<std::string> > & data,
             const box_side & side, const int& row  , const int & column );
    ho_box ();
    // Setter functions
    void setSlot(const int& s){ m_int_slot = s; }

   protected:

    // Strings taken from raw data
    std::string m_str_phi;
    std::string m_str_htr_fiber;

    // Processed integers derived from strings
    int m_int_phi_min;
    int m_int_phi_max;
  };

  class hox_box : public box 
  {
   public:
    // constructor 
    hox_box ( const std::vector<std::vector<std::string> > & data,
              const box_side & side, const int& row  , const int & column );

   protected:
    // Strings taken from raw data
    const std::string m_str_crate_slot_htr_fib;
    const std::string m_str_crate;
    const std::string m_str_slot;
    const std::string m_str_htr_fib;

    // Processed integers derived from strings
    int m_int_crate;
  };

  // Private functions
  void processRawData(const std::vector<std::vector<std::string> > & raw_data, const int & crate_number);
  
  // Member items
  
  // Hash table info
  const int m_max_hash;
  std::vector<box> * m_hash_table;
  
  // Raw data info
  const size_t m_raw_data_n_columns;

};

namespace tools 
{
  // Split a string into a vector of strings that are split by a delimiter
  void split ( const std::string & content,
               std::vector<std::string> & output,
               const char & delimiter );
  // Clean white space from end of line
  void rtrim (std::string &content);
  // Is this string a number?
  bool is_number(const std::string & content);
};

namespace box_tools 
{
  int getHash( const int & ring,
               const int & sector,
               const int & rm,
               const int & rm_fib );

  int getHash( const std::string & ring,
               const std::string & sector,
               const int & rm,
               const int & rm_fib );
};


#endif
