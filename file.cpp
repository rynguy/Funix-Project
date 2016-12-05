//Authors: Armante Washington & Ryan Nguyen
//file.cpp

#include "file.h"
#include "permissions.h"
#include "Time.h"
#include <cstring>

using namespace std;

File::File(const char* nam, const Permissions permission): perms(permission) 
{
  name = new char[strlen(nam) + 1];
  strcpy(name, nam);
}

File::File(const File& rhs): perms(rhs.perms)
{
  name = new char[strlen(rhs.name) + 1];
  strcpy(name, rhs.name);
}

File::~File()
{
  delete[] name;
}

void File::changePerms(short permissions)
{
  perms.chmodFile(permissions);
} // changePerms()

bool File::checkPerms(short permissions, const char *user) const
{
  return perms.check(permissions, user);
}  // check()

void File::print(bool var) const
{
  if(var)
  {
    perms.print();
    cout << ' ';
    modTime.print();
    cout << ' ' << name << endl;
  } //if true
  else
  {	
    cout << name << " ";
  } // else 
}//print()

void File::rename(const char* nam)
{
  name = new char[strlen(nam) + 1];
  strcpy(name, nam);
}//rename()

void File::touch()
{
  modTime.update();
}//touch()

void File::chown(const char* user)
{
  perms.setOwner(user);
}//chown()

const char* File::getName() const
{
  return name;
}//getName()

bool File::isFile() const
{
  short value = perms.getValue();
  short octal = (((value >> 9) & 7) & 1);
  return octal == 0;
}//isFile

bool File::operator== (const File &rhs) const
{
  return !strcmp(name, rhs.name);
} // operator==

bool File::operator< (const File &rhs) const
{
  return strcmp(name, rhs.name) < 0;
} // operator<

void operator<< (ostream &os, File &file)
{
  File *f = &file;
  f->write(os);
}// operator<<

ostream &File::write(ostream& os) const
{
  os << 'F' << ' ' << name << " " << modTime << " " << perms << endl;

  return os; 
}//write

istream &operator>> (istream &is, File &file)
{
  char temp[80];
  is >> temp;
 
  if(file.name)
    delete [] file.name;

  file.name = new char[strlen(temp) + 1];
  strcpy(file.name, temp);
  
  is.ignore();  // eat space char
  is >> file.modTime >> file.perms;
  return is;
}//operator>>
