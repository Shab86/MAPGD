#ifndef _DATA_H_
#define _DATA_H_

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "typedef.h"

//! A class which can be written as flat text file or into an SQL database.
/*! Data can be written in a plain text representation (the overloaded 
 * \>\> and \<\<), in a binary representation which requires accurate 
 * information from the size() function, or be given to an SQL database 
 * (the sql_ functions). Additionally, all Data must have a static 
 * Registration, which uses the static table_name to write the derived class 
 * into new_data_. The static member new_data_ is a map between strings and 
 * constructors for the derived classes, so that derived Data classes 
 * can be created from strings. Ultimately my goal is to require Data classes
 * to define a transformations between each other, so that mapgd can convert 
 * between exteranal data schemes and internal schemes as easily as possible, 
 * and maybe even aid in determining when data in the SQL data base needs to 
 * be updated. Notes to self: I can implement his be defining a bijection 
 * between the sets {X, Y, ... } and {Z}, meaning that I can construct 
 * {X, Y, ... } whenever I know {Z} and visa versa. In order to know how to 
 * define a bijection we need to define injective and surjective function 
 * between Data. If {X, Y} are surjective on {Z} but not injective, then we can
 * define a bijection by removing elements from {X} or {Y}. TODO: Find a way to 
 * establish which elements of {X} or {Y} are extraneous.
 * Also, is this injective/surjective language actually helpful?
 */
class Data {
private:
	//! The read function must be defined in the child class.
	static const std::string file_name;
	//! The read function must be defined in the child class.
	static const std::string table_name;
protected:
	//! The read function must be defined in the child class.
	virtual void read(std::istream& str) = 0;
	//! The write function must be defined in the child class.
	virtual void write(std::ostream& str) const = 0;

/*	//! T can be constructed form this, but this can not be constructed from T.
	template <class T, typename... Args> 
	void 
	injection_ (T&) const { 
		injection_{}
	}
	//! T cannot be constructed form this, but this can be constructed from T.
	template <class T> 
	void
	surjection_ (const T&) { }

	template<typename T, typename... Args> 
	T& convert(const Args &args ...)
	{
		
		args.injection_(T);
	}*/

public:
	Data(){};
	Data(std::vector <std::string> &){};
	virtual const std::string get_file_name() const {return this->file_name;};
        virtual const std::string get_table_name() const {return this->table_name;};

	/**
	 * \defgroup SQL functions
	 * @{
	 */

	//! Return the names of the columns, along with variable type
	/* E.g. Column_name_1 integer, Column_name_2 integer, ...
 	 */ 
        virtual const std::string sql_header(void) const {return "";};
	//! Return the names of the columns
	/* E.g. Column_name_1, Column_name_2, ...
 	 */ 
        virtual const std::string sql_column_names(void) const {return "";};
	//! Return the values to be placed in columns
	/* E.g. 1, 0, ...
 	 */ 
	virtual const std::string sql_values(void) const {return "";};
	//! Reads the values...
	/* E.g. 1, 0, ...
 	 */ 
	virtual void sql_read(std::istream &);

	/*
	 *@}
         */

	/**
	 * \defgroup basic IO
	 * @{
	 */
	//! Use the \<\< operator to write Data in text mode.
	friend std::ostream& operator << (std::ostream&, const Data &);	
	//! Use the \>\> operator to read Data in text mode.
	friend std::istream& operator >> (std::istream&, Data &);		

	//! The size of the class in bytes. 
	/* If size() is undefined in the child class, then we don't want to 
	 * read or write any at all, so we return 0.
	 */
        virtual size_t size(void) const {return 0;};
	/*
	 *@}
         */

	//! Constructs an instance of the class Registered w/ string.
	static Data * new_from_str (const std::string &, const std::vector<std::string> &);
};

//! Data which has an absolute position.
/* Indexed_data can be stored in indexed files.
 */
class Indexed_data : public virtual Data {
protected:
	id1_t abs_pos_;
public:
	Indexed_data(){};
	Indexed_data(std::vector <std::string> &){};
	id1_t get_abs_pos (void) const;	  //!< Indexed data needs to associate each datum with a position in the genome.
	void set_abs_pos (const id1_t &); //!< Indexed data needs to associate each datum with a position in the genome. 
};

//! A static initializer for every translation unit.
/*  This initializes the m_data_ctor map which should not be visible
 *  outside of ...
 */
static struct Registry_initalizer {
	Registry_initalizer ();
	~Registry_initalizer ();
} Registry_initalizer;

//! A class which registers a child of Data in Data::new_data_.
/*! All children of Data should have a static declaration of  
 *  Registration so that they can be created by sql_read and 
 *  sql_write.
 */
class Registration 
{
public:
	//!< The constructor of Registration, which stores a constructor for a Data class to new_data_
	Registration (const std::string &str, Data*(*fn)(const std::vector <std::string> &) );
	//!< The constructor of Registration, which stores a constructor for a Data class to new_data_
	~Registration (void);
private:
	std::string name_;
};

#endif
